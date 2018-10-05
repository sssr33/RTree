#include "stdafx.h"
#include "RTree.h"
#include "algorithm2.h"

RTree::Node::Node()
    : box(AABB::CreateEmpty()), data(nullptr)
{}

RTree::Node::Node(const AABB &box)
    : box(box), data(nullptr)
{}

RTree::Node::Node(const AABB &box, void *data)
    : box(box), data(data)
{}

bool RTree::Node::IsLeaf() const {
    bool leaf = this->child.empty() || (!this->child.empty() && this->child[0].child.empty());
    return leaf;
}




RTree::RTree(size_t maxNodeCap)
    : maxNodeCap(maxNodeCap)
    , minNodeCap(maxNodeCap / 2)
    //, treeHeight(1)
{}

RTree::~RTree() {
}

void RTree::Insert(const AABB &aabb, void *data) {
    Node *leafA = nullptr;
    Node *leafB = nullptr;
    Node splitB; // tmp node for splitting

    //this->ReserveTmpParent();

    leafA = this->ChooseLeaf(aabb);
    auto &child = leafA->child;

    child.push_back(RTree::Node(aabb, data));
    leafA->box = AABB::Union(leafA->box, aabb);

    if (child.size() > this->maxNodeCap) {
        this->SplitNode(*leafA, splitB);
        leafB = &splitB;
    }

    /*if (child.size() <= this->maxNodeCap) {
        leafA->box = AABB::Union(leafA->box, aabb);
    }
    else {
        this->SplitNode(*leafA, splitB);
        leafB = &splitB;
    }*/

    /*
    <AdjustTree> can be used to add <aabb> to the tree as <leafB, splitB>
    node after calling <ChooseLeaf> and adding <leafA> to <this->tmpParent>.
    Buf for now original algorithm is used to make it easy to setps in original paper.
    */
    this->AdjustTree(leafA, leafB, splitB);

    if (leafB) {
        // root split
        assert(leafA);
        assert(!leafA->child.empty() && !leafB->child.empty());

        // grow tree taller
        auto newBox = AABB::Union(leafA->box, leafB->box);
        auto newRoot = Node(newBox);

        newRoot.child.reserve(2);
        newRoot.child.push_back(std::move(*leafA));
        newRoot.child.push_back(std::move(*leafB));

        this->root = std::move(newRoot);
    }

    this->tmpParent.clear();
}

void RTree::Delete(const AABB &aabb, void *data) {

}

//void RTree::ReserveTmpParent() {
//    // get enough space for parents
//    this->tmpParent.reserve(this->treeHeight);
//    this->tmpParent.clear();
//}

RTree::Node *RTree::ChooseLeaf(const AABB &aabb) {
    RTree::Node *leaf = &this->root;

    while (!leaf->IsLeaf()) {
        int enlargment = INT_MAX;
        int area = INT_MAX;
        size_t childCount = std::numeric_limits<size_t>::max();
        RTree::Node *next = nullptr;
        auto &child = leaf->child;

        for (auto &i : child) {
            int oldArea = i.box.Area();
            int newArea = AABB::Union(aabb, i.box).Area();
            int curEnlargment = newArea - oldArea;

            if (curEnlargment < enlargment ||
                (curEnlargment == enlargment && (oldArea < area ||
                    (oldArea == area && i.child.size() < childCount))))
            {
                childCount = i.child.size();
                area = oldArea;
                enlargment = curEnlargment;
                next = &i;
            }
        }

        this->tmpParent.push_back(leaf);
        leaf = next;
    }

    return leaf;
}

void RTree::AdjustTree(Node *&leafA, Node *&leafB, Node &splitB) {
    while(!this->tmpParent.empty()) {
        auto parentAB = this->tmpParent.back();
        this->tmpParent.pop_back();

        if (leafB) {
            auto &childAB = parentAB->child;

            childAB.push_back(std::move(*leafB));
            leafB = nullptr;

            if (childAB.size() > this->maxNodeCap) {
                this->SplitNode(*parentAB, splitB);
                leafB = &splitB;
            }
        }
        else {
            parentAB->box = AABB::Union(leafA->box, parentAB->box);
        }

        leafA = parentAB;
    }
}

void RTree::SplitNode(Node &n, Node &nb) {
    Node na;
    size_t seedIdxA, seedIdxB;

    this->PickSeed(n.child.data(), n.child.size(), seedIdxA, seedIdxB);

    auto &seedNodeA = n.child[seedIdxA];
    auto &seedNodeB = n.child[seedIdxB];

    na.box = seedNodeA.box;
    nb.box = seedNodeB.box;

    na.child.push_back(std::move(seedNodeA));
    nb.child.push_back(std::move(seedNodeB));

    fastRemoveByIdx(n.child, seedIdxA);
    fastRemoveByIdx(n.child, seedIdxB);

    // split remaining entries of the node
    for (auto &i : n.child) {
        this->PickGroup(i, na, nb);
    }

    // update current <n> node
    n = std::move(na);
}

void RTree::PickSeed(const Node *nodes, size_t count, size_t &seedAIdx, size_t &seedBIdx) {
    assert(count >= this->maxNodeCap);

    int maxWastedArea = 0;

    // check all pairs without i == j and not checking mirror cases i != j and j != i, only i != j
    for (size_t i = 1; i < count; i++) {
        auto &a = nodes[i].box;
        auto areaA = a.Area();

        for (size_t j = 0; j < i; j++) {
            AABB pairBound;
            auto &b = nodes[j].box;

            pairBound = AABB::Union(a, b);

            auto areaB = b.Area();
            auto areaPairBound = pairBound.Area();
            auto pairBoundWastedArea = areaPairBound - areaA - areaB;

            if (pairBoundWastedArea > maxWastedArea) {
                maxWastedArea = pairBoundWastedArea;
                seedAIdx = i;
                seedBIdx = j;
            }
        }
    }
}

void RTree::PickGroup(Node &curNode, Node &na, Node &nb) {
    AABB newBoxA, newBoxB;

    newBoxA = AABB::Union(curNode.box, na.box);
    newBoxB = AABB::Union(curNode.box, nb.box);

    auto areaA = na.box.Area();
    auto areaB = nb.box.Area();
    auto newAreaA = newBoxA.Area();
    auto newAreaB = newBoxB.Area();

    auto areaAInc = newAreaA - areaA;
    auto areaBInc = newAreaB - areaB;

    if (areaBInc < areaAInc) {
        nb.box = newBoxB;
        nb.child.push_back(std::move(curNode));
    }
    else {
        na.box = newBoxA;
        na.child.push_back(std::move(curNode));
    }
}

RTree::Node *RTree::FindLead(const AABB &aabb, void *data) {
    RTree::Node *leaf = &this->root;

    while (!leaf->IsLeaf()) {

    }

    return leaf;
}
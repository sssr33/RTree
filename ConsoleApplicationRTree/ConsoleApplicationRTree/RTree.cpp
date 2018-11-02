#include "pch.h"
#include "RTree.h"
#include "Helpers.h"

#include <cassert>
#include <algorithm>

RTree::RTree(size_t minEntryCount, size_t maxEntryCount)
    : minEntryCount(minEntryCount)
    , maxEntryCount(maxEntryCount)
    , root(std::make_shared<Node>())
{

}

RTree::~RTree()
{}

void RTree::Insert(Rect entry) {
    //std::shared_ptr<Node> leaf2;
    auto leaf = this->ChooseLeaf(entry);

    leaf->Add(std::make_shared<Node>(std::move(entry), true));

    if (leaf->Count() > this->maxEntryCount) {
        auto split = this->SplitNode(leaf);
        assert(split.leaf1 && split.leaf2);

        auto parent = leaf->parent.lock();
        if (parent) {
            auto it = std::find(parent->child.begin(), parent->child.end(), leaf);
            parent->child.erase(it);

            // <parent> will be valid after <leaf> was removed and new <leafs> were added
            // this is because <leafs> have same rectangle as <leaf> which were added previously and consequently updated <parents>'s rectandle inside <Add> method
            parent->Add(split.leaf1);
            parent->Add(split.leaf2);

            this->AdjustTree(split.leaf1, split.leaf2);
        }

        // must be always not NULL
        assert(split.leaf1);

        // after <SplitNode> and <AdjustTree> leaf2 must be null or it must be root node because of root split
        assert(!split.leaf2 || (split.leaf2 && split.leaf2->Root()));

        this->GrowTreeTaller(std::move(split.leaf1), std::move(split.leaf2));
    }
}

std::shared_ptr<Node> RTree::ChooseLeaf(const Rect &entry) {
    auto n = this->root;

    while (!n->Leaf()) {
        n = this->ChooseSubtree(entry, *n);
    }

    return n;
}

std::shared_ptr<Node> RTree::ChooseSubtree(const Rect &entry, const Node &n) {
    size_t idx = 0;
    int minEnlargement = (std::numeric_limits<int>::max)();
    int minArea = (std::numeric_limits<int>::max)();

    for (size_t i = 0; i < n.Count(); i++) {
        auto &child = n.child[i];
        auto &childRect = child->rect;

        auto childArea = childRect.Area();
        auto enlargement = childRect.United(entry).Area() - childArea;

        if (enlargement < minEnlargement) {
            idx = i;
            minEnlargement = enlargement;
            minArea = childArea;
        }
        else if (enlargement == minEnlargement && childArea < minArea) {
            idx = i;
            minArea = childArea;
        }
    }

    auto subtree = n.child[idx];

    return subtree;
}

void RTree::GrowTreeTaller(std::shared_ptr<Node> root1, std::shared_ptr<Node> root2) {
    if (!root1->Root() || !root2) {
        return;
    }

    assert(root2->Root());

    auto rootRect = root1->rect.United(root2->rect);

    this->root = std::make_shared<Node>(std::move(rootRect));

    this->root->Add(std::move(root1));
    this->root->Add(std::move(root2));
}

RTree::SplitNodeRes RTree::SplitNode(std::shared_ptr<Node> &node) {
    auto seedsIdx = this->PickSeeds(node);
    auto group1 = std::make_shared<Node>();
    auto group2 = std::make_shared<Node>();

    group1->Add(std::move(node->child[seedsIdx.seed1Idx]));
    group2->Add(std::move(node->child[seedsIdx.seed2Idx]));

    Helpers::UnorderedRemove(node->child, seedsIdx.seed1Idx, seedsIdx.seed2Idx);

    while (!node->child.empty()) {
        // <TryAssignAll> optimization may result in not so good slipts as using <PickNext>
        /*if (this->TryAssignAll(node, group1)) {
            break;
        }
        if (this->TryAssignAll(node, group2)) {
            break;
        }*/

        Node *nextGroup = nullptr;
        auto pickRes = this->PickNext(node, group1, group2);

        if (pickRes.group1AreaIncrease < pickRes.group2AreaIncrease) {
            nextGroup = group1.get();
        }
        else if (pickRes.group2AreaIncrease < pickRes.group1AreaIncrease) {
            nextGroup = group2.get();
        }
        else {
            int area1 = group1->rect.Area();
            int area2 = group2->rect.Area();

            if (area1 < area2) {
                nextGroup = group1.get();
            }
            else if (area2 < area1) {
                nextGroup = group2.get();
            }
            else if (group1->Count() < group2->Count()) {
                nextGroup = group1.get();
            }
            else {
                nextGroup = group2.get();
            }
        }

        nextGroup->Add(Helpers::UnorderedPop(node->child, pickRes.nextIdx));
    }

    RTree::SplitNodeRes res;

    res.leaf1 = std::move(group1);
    res.leaf2 = std::move(group2);

    return res;
}

void RTree::AdjustTree(std::shared_ptr<Node> &leaf1, std::shared_ptr<Node> &leaf2) {
    auto &node1 = leaf1;
    auto &node2 = leaf2;

    while (!node1->Root()) {
        assert((!node2) || (node2->parent.lock() == node1->parent.lock()));

        auto parent = node1->parent.lock();

        // must be true because node1 is not a root
        assert(parent);

        if (parent->Count() > this->maxEntryCount) {
            auto split = this->SplitNode(parent);
            assert(split.leaf1 && split.leaf2);

            auto parent2 = parent->parent.lock();
            if (parent2) {
                auto it = std::find(parent2->child.begin(), parent2->child.end(), parent);
                parent2->child.erase(it);

                // <parent> will be valid after <leaf> was removed and new <leafs> were added
                // this is because <leafs> have same rectangle as <leaf> which were added previously and consequently updated <parents>'s rectandle inside <Add> method
                parent2->Add(split.leaf1);
                parent2->Add(split.leaf2);
            }

            node1 = std::move(split.leaf1);
            node2 = std::move(split.leaf2);
        }
        else if(leaf2 == nullptr) {
            // when no split were performed then rectangle must be adjusted
            // this is needed because lower node is grew up

            Rect r = Rect::Degenerated();

            for (auto &i : parent->child) {
                r.Union(i->rect);
            }

            parent->rect = r;

            node1 = std::move(parent);
        }
        else {
            // node was split on previous iteration
            // rectangle of its parent is therefore valid
            // because no further slip is needed node2 is set to null to force algorithm to update rectangle of upper parents because whey are NOT valid
            node1 = std::move(parent);
            node2 = nullptr;
        }
    }
}

RTree::PickSeedsResult RTree::PickSeeds(const std::shared_ptr<Node> &node) {
    PickSeedsResult res;
    int maxDiff = (std::numeric_limits<int>::min)();

    res.seed1Idx = 0;
    res.seed2Idx = 0;

    // node->Count() - 1 is used to not perfrom inner cycle which will start at j == Count() and will result in no op
    for (size_t i = 0; i < node->Count() - 1; i++) {
        const auto &rectI = node->child[i]->rect;
        auto areaI = rectI.Area();

        // use j = i + 1 to not include i == j cases as well as mirrored indexes
        for (size_t j = i + 1; j < node->Count(); j++) {
            const auto &rectJ = node->child[j]->rect;
            auto areaJ = rectJ.Area();
            auto areaIJ = rectI.United(rectJ).Area();

            auto diff = areaIJ - areaJ - areaI;

            if (diff > maxDiff) {
                maxDiff = diff;
                res.seed1Idx = i;
                res.seed2Idx = j;
            }
        }
    }

    return res;
}

RTree::PickNextResult RTree::PickNext(const std::shared_ptr<Node> &node, const std::shared_ptr<Node> &group1, const std::shared_ptr<Node> &group2) {
    PickNextResult res;
    int maxDiff = (std::numeric_limits<int>::min)();
    const auto &rect1 = group1->rect;
    const auto &rect2 = group2->rect;

    auto group1Area = rect1.Area();
    auto group2Area = rect2.Area();

    for (size_t i = 0; i < node->child.size(); i++) {
        auto areaIncrease1 = rect1.United(node->child[i]->rect).Area() - group1Area;
        auto areaIncrease2 = rect2.United(node->child[i]->rect).Area() - group2Area;

        auto diff = std::abs(areaIncrease1 - areaIncrease2);

        if (diff > maxDiff) {
            maxDiff = diff;
            res.nextIdx = i;
            res.group1AreaIncrease = areaIncrease1;
            res.group2AreaIncrease = areaIncrease2;
        }
    }

    return res;
}

bool RTree::TryAssignAll(std::shared_ptr<Node> &from, std::shared_ptr<Node> &to) {
    if (from->Count() + to->Count() <= this->minEntryCount) {
        while (from->Count() != 0) {
            to->Add(std::move(from->child.back()));
            from->child.pop_back();
        }
    }

    return false;
}
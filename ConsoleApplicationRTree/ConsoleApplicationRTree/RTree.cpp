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
    std::shared_ptr<Node> leaf2;
    auto leaf1 = this->ChooseLeaf(entry);

    leaf1->Add(std::make_shared<Node>(std::move(entry), true));

    if (leaf1->Count() > this->maxEntryCount) {
        auto split = this->SplitNode(std::move(leaf1));
        assert(split.leaf1 && split.leaf2);

        leaf1 = std::move(split.leaf1);
        leaf2 = std::move(split.leaf2);
    }

    // leaf2 must not have parent
    // <AdjustTree> will fix this
    assert(!leaf2 || (leaf2 && leaf2->parent.expired()));

    // parent's rect of leaf1 is invalid
    // <AdjustTree> will fix this
    this->AdjustTree(leaf1, leaf2);

    this->GrowTreeTaller(std::move(leaf1), std::move(leaf2));
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

RTree::SplitNodeRes RTree::SplitNode(std::shared_ptr<Node> node) {
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

    auto parent = node->parent.lock();
    if (parent) {
        res.leaf1->parent = parent;

        auto it = std::find(parent->child.begin(), parent->child.end(), node);
        parent->child.erase(it);

        parent->Add(res.leaf1);
    }

    return res;
}

void RTree::AdjustTree(std::shared_ptr<Node> &leaf1, std::shared_ptr<Node> &leaf2) {
    auto &node1 = leaf1;
    auto &node2 = leaf2;

    while (!node1->Root()) {
        assert(false);
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
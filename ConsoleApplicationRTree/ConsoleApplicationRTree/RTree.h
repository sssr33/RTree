#pragma once
#include "Node.h"

#include <memory>

class RTree {
public:
    RTree(size_t minEntryCount, size_t maxEntryCount);
    ~RTree();

    void Insert(Rect entry);

private:
    size_t minEntryCount;
    size_t maxEntryCount;
    std::shared_ptr<Node> root;

    std::shared_ptr<Node> ChooseLeaf(const Rect &entry);
    std::shared_ptr<Node> ChooseSubtree(const Rect &entry, const Node &n);
    void GrowTreeTaller(std::shared_ptr<Node> root1, std::shared_ptr<Node> root2);

    struct SplitNodeRes {
        std::shared_ptr<Node> leaf1;
        std::shared_ptr<Node> leaf2;
    };

    SplitNodeRes SplitNode(std::shared_ptr<Node> node);
    void AdjustTree(std::shared_ptr<Node> &leaf1, std::shared_ptr<Node> &leaf2);

    // quadratic cost algorithm
    struct PickSeedsResult {
        size_t seed1Idx;
        size_t seed2Idx;
    };
    PickSeedsResult PickSeeds(const std::shared_ptr<Node> &node);

    struct PickNextResult {
        size_t nextIdx;
        int group1AreaIncrease;
        int group2AreaIncrease;
    };
    PickNextResult PickNext(const std::shared_ptr<Node> &node, const std::shared_ptr<Node> &group1, const std::shared_ptr<Node> &group2);
    bool TryAssignAll(std::shared_ptr<Node> &from, std::shared_ptr<Node> &to);
};

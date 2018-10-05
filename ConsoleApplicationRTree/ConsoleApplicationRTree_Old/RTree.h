#pragma once
#include "Structs.h"

#include <vector>
#include <cstdint>

class RTree {
    struct Node {
        AABB box;
        std::vector<Node> child;
        void *data;

        Node();
        Node(const AABB &box);
        Node(const AABB &box, void *data);

        bool IsLeaf() const;
    };
public:
    RTree(size_t maxNodeCap);
    ~RTree();

    void Insert(const AABB &aabb, void *data);
    void Delete(const AABB &aabb, void *data);

private:
    const size_t maxNodeCap;
    const size_t minNodeCap;

    Node root;
    //size_t treeHeight;

    // used to hold parents during operations
    // in order to not waste memory in nodes
    std::vector<Node*> tmpParent;

    //void ReserveTmpParent();

    Node *ChooseLeaf(const AABB &aabb);
    void AdjustTree(Node *&leafA, Node *&leafB, Node &splitB);

    void SplitNode(Node &n, Node &nb);
    void PickSeed(const Node *nodes, size_t count, size_t &seedAIdx, size_t &seedBIdx);
    void PickGroup(Node &curNode, Node &na, Node &nb);

    Node *FindLead(const AABB &aabb, void *data);
};
// ConsoleApplicationRTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Structs.h"
#include "algorithm2.h"
#include "RTree.h"

#include <algorithm>
#include <limits>
#include <vector>
#include <memory>

const size_t M = 2;
const size_t m = M / 2;

struct Node {
    AABB box;
    std::vector<Node> child;
    void *data;

    Node()
        : box(AABB::CreateEmpty()), data(nullptr)
    {}

    Node(const AABB &box)
        : box(box), data(nullptr)
    {}
};

void SplitNode(Node &n, Node &na, Node &nb);
void PickSeed(const Node *nodes, size_t count, size_t &seedAIdx, size_t &seedBIdx);
void PickGroup(Node &curNode, Node &na, Node &nb);

int main() {
    RTree tree(2);

    std::vector<int> data;

    data.resize(128);

    tree.Insert(AABB::FromPtSize({ 0, 0 }, { 4, 2 }), &data[0]);
    tree.Insert(AABB::FromPtSize({ 4, 0 }, { 2, 10 }), &data[1]);
    tree.Insert(AABB::FromPtSize({ 6, 0 }, { 2, 10 }), &data[2]);
    tree.Insert(AABB::FromPtSize({ 8, 0 }, { 4, 2 }), &data[3]);

    /*Node root, l1, l2;

    root.child.reserve(4);

    root.child.push_back(AABB::FromPtSize({ 0, 0 }, { 4, 2 }));
    root.child.push_back(AABB::FromPtSize({ 4, 0 }, { 2, 10 }));
    root.child.push_back(AABB::FromPtSize({ 6, 0 }, { 2, 10 }));
    root.child.push_back(AABB::FromPtSize({ 8, 0 }, { 4, 2 }));

    SplitNode(root, l1, l2);*/

    return 0;
}

void SplitNode(Node &n, Node &na, Node &nb) {
    size_t seedIdxA, seedIdxB;

    PickSeed(n.child.data(), n.child.size(), seedIdxA, seedIdxB);

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
        PickGroup(i, na, nb);
    }
}

void PickSeed(const Node *nodes, size_t count, size_t &seedAIdx, size_t &seedBIdx) {
    assert(count >= M);

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

void PickGroup(Node &curNode, Node &na, Node &nb) {
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
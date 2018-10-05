#pragma once
#include "Rect.h"

#include <memory>
#include <vector>

struct Node : std::enable_shared_from_this<Node> {
    Rect rect;

    Node();
    Node(Rect rect);
    Node(Rect rect, bool hasData);

    // leaf
    bool Leaf() const;
    bool Root() const;
    bool DataNode() const;

    size_t Count() const;

    void Add(std::shared_ptr<Node> node);

    std::vector<std::shared_ptr<Node>> child;
    std::weak_ptr<Node> parent;

private:
    bool hasData;
};
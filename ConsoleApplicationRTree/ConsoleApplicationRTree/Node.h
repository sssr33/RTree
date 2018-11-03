#pragma once
#include "Rect.h"

#include <memory>
#include <vector>

struct Node : std::enable_shared_from_this<Node> {
    Rect rect;

    Node();
    Node(Rect rect);
    Node(Rect rect, bool hasData);
    Node(const Node &) = delete;
    Node(Node &&other);

    Node &operator=(Node other);

    friend void swap(Node &a, Node &b);

    // leaf
    bool Leaf() const;
    bool Root() const;
    bool DataNode() const;

    size_t Count() const;

    void Add(std::shared_ptr<Node> node);

    std::shared_ptr<Node> Clone() const;

    bool CompareContent(const std::shared_ptr<const Node> &other) const;

    std::vector<std::shared_ptr<Node>> child;
    std::weak_ptr<Node> parent;

private:
    bool hasData;

    static std::shared_ptr<Node> Copy(const std::shared_ptr<const Node> &other, const std::shared_ptr<Node> &_this, const Node &replaceWithThis);
};
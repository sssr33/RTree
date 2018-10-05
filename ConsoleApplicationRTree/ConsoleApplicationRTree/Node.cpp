#include "pch.h"
#include "Node.h"

Node::Node()
    : rect(Rect::Degenerated())
    , hasData(false)
{}

Node::Node(Rect rect)
    : rect(rect)
    , hasData(false)
{}

Node::Node(Rect rect, bool hasData)
    : rect(rect)
    , hasData(hasData)
{}

bool Node::Leaf() const {
    bool leaf =
        this->child.empty() // empty node
        || this->child.front()->DataNode()
        ;
    return leaf;
}

bool Node::Root() const {
    bool root = this->parent.expired();
    return root;
}

bool Node::DataNode() const {
    return this->hasData;
}

size_t Node::Count() const {
    size_t count = this->child.size();
    return count;
}

void Node::Add(std::shared_ptr<Node> node) {
    node->parent = this->shared_from_this();

    this->rect.Union(node->rect);
    this->child.push_back(std::move(node));
}
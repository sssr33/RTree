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

//Node::Node(const Node &other) {
//    auto otherRoot = other.shared_from_this();
//    auto otherParent = otherRoot->parent.lock();
//
//    while (otherParent) {
//        otherRoot = otherParent;
//        otherParent = otherRoot->parent.lock();
//    }
//
//    auto thisShared = this->shared_from_this();
//
//    Node::Copy(otherRoot, thisShared, other);
//}

Node::Node(Node &&other)
    : Node()
{
    swap(*this, other);
}

Node &Node::operator=(Node other) {
    swap(*this, other);
    return *this;
}

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

std::shared_ptr<Node> Node::Clone() const {
    auto thisRoot = this->shared_from_this();
    auto thisParent = this->parent.lock();

    while (thisParent) {
        thisRoot = thisParent;
        thisParent = thisRoot->parent.lock();
    }

    auto clone = std::make_shared<Node>();

    Node::Copy(thisRoot, clone, *this);

    return clone;
}

bool Node::CompareContent(const std::shared_ptr<const Node> &other) const {
    bool same = false;

    same =
        this->rect == other->rect
        && this->hasData == other->hasData
        && this->child.size() == other->child.size();

    for (size_t i = 0; same && i < this->child.size(); i++) {
        same = same && this->child[i]->CompareContent(other->child[i]);
    }

    return same;
}

std::shared_ptr<Node> Node::Copy(const std::shared_ptr<const Node> &other, const std::shared_ptr<Node> &_this, const Node &replaceWithThis) {
    std::shared_ptr<Node> copy;

    if (other.get() != &replaceWithThis) {
        copy = std::make_shared<Node>();
    }
    else {
        copy = _this;
    }

    copy->hasData = other->hasData;
    copy->rect = other->rect;
    copy->child.reserve(other->child.size());

    for (auto &i : other->child) {
        auto childCopy = Node::Copy(i, _this, replaceWithThis);

        childCopy->parent = copy;

        copy->child.push_back(std::move(childCopy));
    }

    return copy;
}

void swap(Node &a, Node &b) {
    using std::swap;
    swap(a.child, b.child);
    swap(a.hasData, b.hasData);
    swap(a.parent, b.parent);
    swap(a.rect, b.rect);

    auto updateRefs = [](Node &n, Node &old) {
        for (auto &i : n.child) {
            // set new parent for childs
            i->parent = n.weak_from_this();
        }

        auto parent = n.parent.lock();
        if (parent) {
            // update n's entry in parent
            for (auto &i : parent->child) {
                if (i.get() == &old) {
                    i = n.shared_from_this();
                    break;
                }
            }
        }
    };

    updateRefs(a, b);
    updateRefs(b, a);
}
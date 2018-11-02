#pragma once

#include <memory>

/*
As for now it's intended to use with shared_ptr and (private) nested classes-components that passed
around in non public way as pointers and must keep parent class alive by holding shared_ptr<CompositObject>.
Also these classes can be used to workaround limitations of std::static_pointer_cast<some-not-public-inhirited-class>(this->shared_from_this())
by using cast to CompositObject(which can be public inhirited without exposing anything important) and using ComponentRef to keep component alive while not std::static_pointer_cast to it.

Just in case when std::static_pointer_cast is much better/faster-to-code option friend method cam be used:
template<class _Ty1, class _Ty2>
friend std::shared_ptr<_Ty1> std::static_pointer_cast(const std::shared_ptr<_Ty2>& _Other) _NOEXCEPT; // copied from <memory> header
*/

// Just an empty object which sould be used as a mark for objects that consist of components.
struct CompositObject {};

// Be careful when creating(not copying or moving) instances of this class.
// component *should* be a member of parent in order to keep component alive during lifetime of ComponentRef
template<class T>
class ComponentRef {
public:
    ComponentRef()
        : component(nullptr)
    {}

    ComponentRef(T *component, const std::shared_ptr<CompositObject> &parent)
        : component(component), aliveParentRef(parent)
    {}

    T *operator->() {
        return this->component;
    }

    const T *operator->() const {
        return this->component;
    }

private:
    T *component;
    std::shared_ptr<CompositObject> aliveParentRef;
};
#pragma once

#include <memory>

template<class T>
class UPtrVirtualDeleterBase {
public:
    UPtrVirtualDeleterBase() {}
    virtual ~UPtrVirtualDeleterBase() {}

    virtual void operator()(T *obj) = 0;
};

template<class T>
struct UPtrVirtualDeleter {
    std::unique_ptr<UPtrVirtualDeleterBase<T>> virtDeleter;

    void operator()(T *obj) {
        (*this->virtDeleter)(obj);
    }
};

template<class T>
class unique_ptr_v : public std::unique_ptr<T, UPtrVirtualDeleter<T>> { };
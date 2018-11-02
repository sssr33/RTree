#pragma once
#include "MemoryPool.h"
#include "..\CompositObject.h"

#include <memory>

// used privately by memory pool so that it can't be used to delete objects not created by memory pool
class SmartMemoryPoolDestroy {
public:
    virtual void destroySmart(void *v) = 0;
};

struct SharedMemoryPoolSmartPtrDeleter {
    void *ptr; // store ptr to original object to destroy it correctly
    ComponentRef<SmartMemoryPoolDestroy> pool;

    void operator()(void *notused);
};

struct UniqueMemoryPoolSmartPtrDeleter {
    void *ptr; // store ptr to original object to destroy it correctly
    SmartMemoryPoolDestroy *pool;

    void operator()(void *notused);
};

typedef SharedMemoryPoolSmartPtrDeleter SMemPoolDel;
typedef UniqueMemoryPoolSmartPtrDeleter UMemPoolDel;

template <typename Base, typename Del, typename T, size_t BlockSize = 4096>
class SmartMemoryPool : public MemoryPool<T, BlockSize> {
public:
    const char *name;

    SmartMemoryPool() noexcept : name(nullptr) {}

    SmartMemoryPool(const SmartMemoryPool& memoryPool) noexcept
        : MemoryPool(memoryPool), name(memoryPool.name)
    {}

    SmartMemoryPool(SmartMemoryPool&& memoryPool) noexcept
        : MemoryPool(std::move(memoryPool)), name(std::move(memoryPool.name))
    {}

    template <class U> SmartMemoryPool(const SmartMemoryPool<Base, Del, U>& memoryPool) noexcept
        : MemoryPool(memoryPool), name(memoryPool.name)
    {}

    SmartMemoryPool& operator=(const SmartMemoryPool& memoryPool) = delete;

    SmartMemoryPool& operator=(SmartMemoryPool&& memoryPool) noexcept {
        if (this != &memoryPool) {
            MemoryPool::operator=(std::move(memoryPool));
            this->name = std::move(memoryPool.name);
        }

        return *this;
    }

    template <class... Args> std::unique_ptr<T, Del> newElementUnique(Args&&... args) {
        std::unique_ptr<T, Del> uptr;
        auto ptr = this->newElement(std::forward<Args>(args)...);

        uptr.reset(ptr);
        uptr.get_deleter().ptr = ptr;
        this->SetPoolToDeleterBase(uptr.get_deleter());

        return uptr;
    }

    template <class... Args> std::shared_ptr<T> newElementShared(Args&&... args) {
        Del deleter;
        std::shared_ptr<T> sptr;
        auto ptr = this->newElement(std::forward<Args>(args)...);

        deleter.ptr = ptr;
        this->SetPoolToDeleterBase(deleter);

        sptr = std::shared_ptr<T>(ptr, deleter);

        return sptr;
    }

private:

    void SetPoolToDeleterBase(Del &deleter) {
        static_cast<Base*>(this)->SetPoolToDeleter(deleter); // SetPoolToDeleter must be implemented by inherited class
    }
};

// Use when objects can be alive after pool out of scope
// use make() to create new instance
template <typename T, size_t BlockSize = 4096>
class SharedMemoryPool :
    public SmartMemoryPool<SharedMemoryPool<T, BlockSize>, SMemPoolDel, T, BlockSize>
    , public std::enable_shared_from_this<SharedMemoryPool<T, BlockSize>>
    , public CompositObject
    , private SmartMemoryPoolDestroy
{
    friend class SmartMemoryPool<SharedMemoryPool<T, BlockSize>, SMemPoolDel, T, BlockSize>;

    // https://stackoverflow.com/a/8147213
    //struct access : public SharedMemoryPool, public  {};

    // https://stackoverflow.com/questions/8147027/how-do-i-call-stdmake-shared-on-a-class-with-only-protected-or-private-const/
    struct this_is_private {
        explicit this_is_private(int) {}
    };

public:
    explicit SharedMemoryPool(const this_is_private&) {}

    virtual ~SharedMemoryPool() { }

    static std::shared_ptr<SharedMemoryPool> make() {
        return std::make_shared<SharedMemoryPool>(this_is_private{ 0 });
    }

private:
    SharedMemoryPool() {}

    struct SmartMemoryPoolDestroyComponent : public SmartMemoryPoolDestroy {
        std::shared_ptr<SharedMemoryPool> parent;

        void destroySmart(void *v) override {
            parent->destroy((T*)v);
        }
    };

    void SetPoolToDeleter(SMemPoolDel &deleter) {
        auto composit = std::static_pointer_cast<CompositObject>(this->shared_from_this());
        auto component = static_cast<SmartMemoryPoolDestroy*>(this);

        deleter.pool = ComponentRef<SmartMemoryPoolDestroy>(component, composit);
    }

    void destroySmart(void *v) override {
        this->deleteElement((T*)v);
    }
};

// Use when objects *will be* destroyed *before* pool out of scope
// use make() to create new instance
template <typename T, size_t BlockSize = 4096>
class UniqueMemoryPool :
    public SmartMemoryPool<UniqueMemoryPool<T, BlockSize>, UMemPoolDel, T, BlockSize>
    , private SmartMemoryPoolDestroy
{
    friend class SmartMemoryPool<UniqueMemoryPool<T, BlockSize>, UMemPoolDel, T, BlockSize>;
public:
    UniqueMemoryPool() noexcept {}

    UniqueMemoryPool(const UniqueMemoryPool& memoryPool) noexcept
        : SmartMemoryPool(memoryPool)
    {}

    UniqueMemoryPool(UniqueMemoryPool&& memoryPool) noexcept
        : SmartMemoryPool(std::move(memoryPool))
    {}

    template <class U> UniqueMemoryPool(const UniqueMemoryPool<U>& memoryPool) noexcept
        : SmartMemoryPool(memoryPool)
    {}

    UniqueMemoryPool& operator=(const UniqueMemoryPool& memoryPool) = delete;

    UniqueMemoryPool& operator=(UniqueMemoryPool&& memoryPool) noexcept {
        if (this != &memoryPool) {
            SmartMemoryPool::operator=(std::move(memoryPool));
        }

        return *this;
    }

private:
    void SetPoolToDeleter(UMemPoolDel &deleter) {
        deleter.pool = this;
    }

    void destroySmart(void *v) override {
        this->deleteElement((T*)v);
    }
};
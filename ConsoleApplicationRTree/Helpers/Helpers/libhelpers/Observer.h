#pragma once
#include "ObservableToken.h"

#include <assert.h>

template<class Arg1>
class ObservableBase {
public:
    typedef std::function<void(Arg1)> FnType;
    typedef ObservableToken<Arg1> TokenType;

    ObservableToken<Arg1> Add(const FnType &fn) {
        ObservableToken<Arg1> token;

        token.sharedFn = std::make_shared<FnType>(fn);

        this->observers.push_back(token.sharedFn);

        token.iterator = --this->observers.end();
#ifdef _DEBUG
        token.observable = this;
#endif

        return token;
    }

    void Remove(const ObservableToken<Arg1> &v) {
        assert(v.observable == this);
        this->observers.erase(v.iterator);
    }

    void RemoveAll() {
        this->observers.clear();
    }

    bool HaveObservers() {
        return !this->observers.empty();
    }

protected:
    std::list<std::weak_ptr<FnType>> observers;
};

// implemetation of Observer pattern based on std::function
template<class Arg1>
class Observable : public ObservableBase<Arg1> {
public:
    void Notify(Arg1 arg) {
        this->observers.remove_if(
            [&](const std::weak_ptr<FnType> &item)
        {
            bool remove = true;
            auto locked = item.lock();

            if (locked) {
                remove = false;
                (*locked)(arg);
            }

            return remove;
        });
    }
};

template<>
class Observable<void> : public ObservableBase<void> {
public:
    void Notify() {
        this->observers.remove_if(
            [&](const std::weak_ptr<FnType> &item)
        {
            bool remove = true;
            auto locked = item.lock();

            if (locked) {
                remove = false;
                (*locked)();
            }

            return remove;
        });
    }
};
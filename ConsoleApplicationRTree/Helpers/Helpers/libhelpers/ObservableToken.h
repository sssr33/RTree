#pragma once

#include <functional>
#include <memory>
#include <list>

#ifdef _DEBUG
template<class Arg1>
class ObservableBase;
#endif

template<class Arg1>
struct ObservableToken {
    typedef std::function<void(Arg1)> FnType;

    std::shared_ptr<FnType> sharedFn;
    typename std::list<std::weak_ptr<FnType>>::iterator iterator;

#ifdef _DEBUG
    // used in debug mode to check if token deleted from right observable.
    ObservableBase<Arg1> *observable;
#endif
};
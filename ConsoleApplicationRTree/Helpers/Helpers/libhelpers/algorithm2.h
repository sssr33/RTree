#pragma once

#include <algorithm>
#include <vector>
#include <assert.h>

template<class T, class V>
bool removeValue(T &container, const V &value) {
    auto remIt = std::remove(container.begin(), container.end(), value);
    bool erasedSomething = remIt != container.end();
    container.erase(remIt, container.end());
    return erasedSomething;
}

template<class T>
struct FastRemoveByIdxNoBackCleanCmp {
    bool operator()(T &backVal) const {
        return false; // don't remove anything from back
    }
};

template<class T, class A>
FastRemoveByIdxNoBackCleanCmp<T> MakeFastRemoveByIdxNoBackCleanCmp(const std::vector<T, A> &v) {
    return FastRemoveByIdxNoBackCleanCmp<T>();
}

template<class T>
struct FastRemoveByIdxBackSwapFn {
    void operator()(T &idxVal, T &backVal, size_t idx, size_t backIdx) const {
        // do nothing
    }
};

template<class T, class A>
FastRemoveByIdxBackSwapFn<T> MakeFastRemoveByIdxBackSwapFn(const std::vector<T, A> &v) {
    return FastRemoveByIdxBackSwapFn<T>();
}

template<class T, class A, class BackCleaner = FastRemoveByIdxNoBackCleanCmp<T>>
void fastRemoveBackCleanerHelper(std::vector<T, A> &vec, const BackCleaner &backCleaner = FastRemoveByIdxNoBackCleanCmp<T>()) {
    auto backEmpty = [&]() {
        return !vec.empty() && backCleaner(vec.back());
    };

    while (backEmpty()) {
        vec.pop_back();
    }
}

/* Fast algorithm for removing element from any position. WARNING!!! Item order will be changed! Back item is moved to <idx>!
BackCleaner - returns true to delete item. checks if item at back can be considered as "empty". Use FastRemoveByIdxNoBackCleanCmp/MakeFastRemoveByIdxNoBackCleanCmp if item can't be in "empty" state.
BackSwapFn - returns void. performs custom action when item from back moved to <idx>. Use FastRemoveByIdxBackSwapFn/MakeFastRemoveByIdxBackSwapFn if no action is needed.
*/
template<class T, class A, class BackCleaner = FastRemoveByIdxNoBackCleanCmp<T>, class BackSwapFn = FastRemoveByIdxBackSwapFn<T>>
void fastRemoveByIdx(std::vector<T, A> &vec, size_t idx, const BackCleaner &backCleaner = FastRemoveByIdxNoBackCleanCmp<T>(), const BackSwapFn &backSwapFn = FastRemoveByIdxBackSwapFn<T>()) {
    assert(idx < vec.size());

    fastRemoveBackCleanerHelper(vec, backCleaner);

    if (idx >= vec.size() || vec.empty()) {
        return; // was removed during CleanBack
    }

    if (idx == vec.size() - 1) {
        backSwapFn(vec.back(), vec.back(), vec.size() - 1, vec.size() - 1);
        vec.pop_back();
        return;
    }

    backSwapFn(vec[idx], vec.back(), idx, vec.size() - 1);
    vec[idx] = std::move(vec.back());
    vec.pop_back();
}

template<class T, class A, class BackSwapFn = FastRemoveByIdxBackSwapFn<T>, class BackCleaner = FastRemoveByIdxNoBackCleanCmp<T>>
void fastRemoveByIdx2(std::vector<T, A> &vec, size_t idx, const BackSwapFn &backSwapFn = FastRemoveByIdxBackSwapFn<T>(), const BackCleaner &backCleaner = FastRemoveByIdxNoBackCleanCmp<T>()) {
    fastRemoveByIdx(vec, idx, backCleaner, backSwapFn);
}

/* Fast remove_if. WARNING!!! Item order will be changed! Back item is moved to <removed item idx>!
Bkwd version performs less backSwapFn calls but performs more reverse iterations.
Both versions perform <vec.size()> iterations.
Template params:
Pred - returns true to delete item. decides if itemshould be removed.
BackCleaner - returns true to delete item. checks if item at back can be considered as "empty". Use FastRemoveByIdxNoBackCleanCmp/MakeFastRemoveByIdxNoBackCleanCmp if item can't be in "empty" state.
BackSwapFn - returns void. performs custom action when item from back moved to <idx>. Use FastRemoveByIdxBackSwapFn/MakeFastRemoveByIdxBackSwapFn if no action is needed.
*/
template<class T, class A, class Pred, class BackCleaner = FastRemoveByIdxNoBackCleanCmp<T>, class BackSwapFn = FastRemoveByIdxBackSwapFn<T>>
void fastRemoveIfBkwd(std::vector<T, A> &vec, const Pred &pred, const BackCleaner &backCleaner = FastRemoveByIdxNoBackCleanCmp<T>(), const BackSwapFn &backSwapFn = FastRemoveByIdxBackSwapFn<T>()) {
    if (vec.empty()) {
        return;
    }

    // back cleaner can be combined with pred but I'm not sure if it's slower or not to iterate backwards
    // so just leave more simple code
    //fastRemoveBackCleanerHelper(vec, backCleaner);

    fastRemoveBackCleanerHelper(vec, [&](T &backVal) { return backCleaner(backVal) || pred(backVal); });

    if (vec.empty()) {
        return;
    }

    size_t i = 0;
    size_t size = vec.size();

    while (i < size) {
        if (pred(vec[i])) {
            backSwapFn(vec[i], vec.back(), i, size - 1);
            vec[i] = std::move(vec.back());
            vec.pop_back();
            size--;
        }
        else {
            i++;
        }
    }
}

template<class T, class A, class Pred, class BackSwapFn = FastRemoveByIdxBackSwapFn<T>, class BackCleaner = FastRemoveByIdxNoBackCleanCmp<T>>
void fastRemoveIfBkwd2(std::vector<T, A> &vec, const Pred &pred, const BackSwapFn &backSwapFn = FastRemoveByIdxBackSwapFn<T>(), const BackCleaner &backCleaner = FastRemoveByIdxNoBackCleanCmp<T>()) {
    fastRemoveIfBkwd(vec, pred, backCleaner, backSwapFn);
}

/* Fast remove_if. WARNING!!! Item order will be changed! Back item is moved to <removed item idx>!
Fwd version is better if iteration in forward direction is preffered and backSwapFn can be performed on more items than it may in Bkwd version.
Both versions perform <vec.size()> iterations.
Template params:
Pred - returns true to delete item. decides if itemshould be removed.
BackCleaner - returns true to delete item. checks if item at back can be considered as "empty". Use FastRemoveByIdxNoBackCleanCmp/MakeFastRemoveByIdxNoBackCleanCmp if item can't be in "empty" state.
BackSwapFn - returns void. performs custom action when item from back moved to <idx>. Use FastRemoveByIdxBackSwapFn/MakeFastRemoveByIdxBackSwapFn if no action is needed.
*/
template<class T, class A, class Pred, class BackCleaner = FastRemoveByIdxNoBackCleanCmp<T>, class BackSwapFn = FastRemoveByIdxBackSwapFn<T>>
void fastRemoveIfFwd(std::vector<T, A> &vec, const Pred &pred, const BackCleaner &backCleaner = FastRemoveByIdxNoBackCleanCmp<T>(), const BackSwapFn &backSwapFn = FastRemoveByIdxBackSwapFn<T>()) {
    if (vec.empty()) {
        return;
    }

    // back cleaner can be combined with pred but I'm not sure if it's slower or not to iterate backwards
    // so just leave more simple code
    fastRemoveBackCleanerHelper(vec, backCleaner);

    if (vec.empty()) {
        return;
    }

    size_t i = 0;
    size_t size = vec.size();

    while (i < size) {
        if (pred(vec[i])) {
            backSwapFn(vec[i], vec.back(), i, size - 1);
            vec[i] = std::move(vec.back());
            vec.pop_back();
            size--;
        }
        else {
            i++;
        }
    }
}

template<class T, class A, class Pred, class BackSwapFn = FastRemoveByIdxBackSwapFn<T>, class BackCleaner = FastRemoveByIdxNoBackCleanCmp<T>>
void fastRemoveIfFwd2(std::vector<T, A> &vec, const Pred &pred, const BackSwapFn &backSwapFn = FastRemoveByIdxBackSwapFn<T>(), const BackCleaner &backCleaner = FastRemoveByIdxNoBackCleanCmp<T>()) {
    fastRemoveIfFwd(vec, pred, backCleaner, backSwapFn);
}
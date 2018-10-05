#pragma once

#include <vector>
#include <algorithm>

class Helpers {
public:
    // remove element from vector with O(1) complexity but moves item from back to idx
    template<class T> static void UnorderedRemove(std::vector<T> &vec, size_t idx) {
        vec[idx] = std::move(vec.back());
        vec.pop_back();
    }

    // removes 2 items handling indexes dependencies
    template<class T> static void UnorderedRemove(std::vector<T> &vec, size_t idx1, size_t idx2) {
        Helpers::UnorderedRemove(vec, (std::max)(idx1, idx2)); // remove greater idx first to no adjust second idx
        Helpers::UnorderedRemove(vec, (std::min)(idx1, idx2));
    }

    // does same thing as <UnorderedRemove> but returns removed item
    template<class T> static T UnorderedPop(std::vector<T> &vec, size_t idx) {
        auto item = std::move(vec[idx]);
        Helpers::UnorderedRemove(vec, idx);
        return item;
    }
};
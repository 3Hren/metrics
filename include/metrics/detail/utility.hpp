#pragma once

#include <utility>

#ifndef __cpp_lib_exchange_function

namespace std {

template<typename T, typename U = T>
T
exchange(T& curr, U&& next) {
    T prev = std::move(curr);
    curr = std::forward<U>(next);
    return prev;
}

} // namespace std

#endif

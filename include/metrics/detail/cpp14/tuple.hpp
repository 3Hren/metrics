#pragma once

#include <tuple>

namespace cpp14 {

/// Returns the number of elements in a variadic list [Other...] that are identical to T.
template<typename T, typename... Other>
struct count;

template<typename T>
struct count<T> {
    static constexpr std::size_t value = 0;
};

template<typename T, typename... Other>
struct count<T, T, Other...> {
    static constexpr std::size_t value = 1 + count<T, Other...>::value;
};

template<typename T, typename U, typename... Other>
struct count<T, U, Other...> {
    static constexpr std::size_t value = count<T, Other...>::value;
};

/// Returns `true` if the type T is unique in a variadic list [U, Other...].
template<typename T, typename... Other>
struct is_unique;

template<typename T, typename U, typename... Other>
struct is_unique<T, U, Other...> {
    static constexpr std::size_t value = count<T, U, Other...>::value == 1;
};

namespace detail {

template <class T, std::size_t N, class... Args>
struct get_index {
    static constexpr std::size_t value = N;
};

template <class T, std::size_t N, class... Args>
struct get_index<T, N, T, Args...> {
    static constexpr std::size_t value = N;
};

template <class T, std::size_t N, class U, class... Args>
struct get_index<T, N, U, Args...> {
    static constexpr std::size_t value = get_index<T, N + 1, Args...>::value;
};

}  // namespace detail

/// Extracts the element of the tuple t whose type is T. Fails to compile if the tuple has more than
/// one element of that type.
template<class T, class... Types>
constexpr
T&
get(std::tuple<Types...>& t) {
    static_assert(is_unique<T, Types...>::value, "type can only occur once in type list");
    return std::get<detail::get_index<T, 0, Types...>::value>(t);
}

/// Extracts the element of the tuple t whose type is T. Fails to compile if the tuple has more than
/// one element of that type.
template<class T, class... Types>
constexpr
const T&
get(const std::tuple<Types...>& t) {
    static_assert(is_unique<T, Types...>::value, "type can only occur once in type list");
    return std::get<detail::get_index<T, 0, Types...>::value>(t);
}

}  // namespace cpp14

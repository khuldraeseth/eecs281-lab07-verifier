#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>


template <typename T, typename... Ts>
struct Count : std::integral_constant<std::size_t, 0> {};

template <typename T, typename... Ts>
constexpr std::size_t count = Count<T, Ts...>::value;

template <typename T, typename... Ts>
struct Count<T, T, Ts...> : std::integral_constant<std::size_t, 1 + count<T, Ts...>> {};

template <typename T, typename U, typename... Ts>
struct Count<T, U, Ts...> : std::integral_constant<std::size_t, count<T, Ts...>> {};

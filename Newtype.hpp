#pragma once

#include <utility>


// `Newtype<T, Tag>` is a distinct type that acts very much like `T`.
// When `Tag1` and `Tag2` are distinct types, `Newtype<T, Tag1>` and `Newtype<T, Tag2>` are distinct types.
template <typename T, typename Tag>
struct Newtype {
    T value;

    Newtype() = delete;

    template <typename U>
    // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
    explicit Newtype(U&& value)
        : value { std::forward<T>(value) } {}

    operator T&() { return value; }
    operator T const&() const { return value; }
};

#pragma once


#include <utility>

#include "Newtype.hpp"


// `Kwarg<T, Tag>` is a type that acts very much like `Newtype<T, Tag>` but also allows assignment.
// This makes function calls like `f(key = 0, value = 1)` possible, where `key` and `value` are `Kwarg`s.
template <typename T, typename Tag>
struct Kwarg : Newtype<T, Tag> {
    template <typename... Ts>
    explicit Kwarg(Ts&&... args)
        : Newtype<T, Tag> { std::forward<Ts>(args)... } {}

    struct NamedParam {
        template <typename U>
        // NOLINTNEXTLINE(cppcoreguidelines-c-copy-assignment-signature)
        auto operator=(U&& value) const -> Kwarg {
            return Kwarg<U, Tag> { std::forward<U>(value) };
        }
    };

    // This is a little sus. Might remove.
    template <typename U, typename UTag>
    operator Kwarg<U, UTag>() const {
        return Kwarg<U, UTag> { this->value };
    }
};

#pragma once


#include <utility>

#include "Newtype.hpp"


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

    template <typename U, typename Uag>
    operator Kwarg<U, Uag>() const {
        return Kwarg<U, Uag> { this->value };
    }
};

#pragma once


template <typename T, typename Tag>
struct Newtype {
    T value;

    Newtype() = delete;
    template <typename U>
    explicit Newtype(U const& value)
        : value { static_cast<T>(value) } {}

    operator T() const { return value; }
};

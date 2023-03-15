#pragma once


template <typename T>
class Consumer {
public:
    virtual auto operator()(T const& other) const -> void = 0;

    Consumer() = default;
    virtual ~Consumer() = default;
    Consumer(Consumer const&) = default;
    Consumer(Consumer&&) noexcept = default;
    auto operator=(Consumer const&) -> Consumer& = default;
    auto operator=(Consumer&&) noexcept -> Consumer& = default;
};

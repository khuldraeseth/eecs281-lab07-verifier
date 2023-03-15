#pragma once

#include "Consumer.hpp"


template <typename T>
class NotEqualityChecker : public Consumer<T> {
public:
    auto operator()(T const&) const -> void override {}
};

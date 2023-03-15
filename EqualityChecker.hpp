#pragma once

#include <iostream>
#include <stdexcept>

#include "Consumer.hpp"


template <typename T>
class EqualityChecker : public Consumer<T> {
public:
    EqualityChecker(T const& val)
        : val { val } {}

    auto operator()(T const& other) const -> void override {
        if (val != other) {
            std::cerr << "Expected " << val << ", got " << other << std::endl;
            throw std::runtime_error("EqualityChecker");
        }
    }

private:
    T const val {};
};

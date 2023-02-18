#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>

#include "ProbeListener.hpp"


template <typename K>
class NotifyingKey {
    K key {};

    ProbeListener<K>* listener;
    auto notify() const -> void {
        if (listener == nullptr) {
            std::cerr << "Attempted to compare a meaningless key for equality" << std::endl;
            throw std::runtime_error("NotifyingKey");
        }
        listener->notify(*this);
    }

public:
    NotifyingKey() = default;

    template <typename... Args>
    explicit NotifyingKey(ProbeListener<K>& listener, Args&&... args)
        : key(std::forward<Args>(args)...)
        , listener { &listener } {}

    auto operator==(NotifyingKey const& other) const -> bool {
        notify();
        other.notify();
        return key == other.key;
    }

    template <typename K1, typename Hash>
    friend struct NotifyingKeyHash;
};


template <typename K, typename Hash>
struct NotifyingKeyHash {
    auto operator()(NotifyingKey<K> const& key) const -> std::size_t { return Hash {}(key.key); }
};

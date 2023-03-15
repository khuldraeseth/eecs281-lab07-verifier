#pragma once

#include <cstdint>
#include <stdexcept>
#include <utility>

#include "ProbeListener.hpp"


template <typename K>
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class NotifyingKey {
    K key {};
    ProbeListener<K>* listener {};

    auto notify(NotifyingKey<K> const& other) const -> void {
        if (listener == nullptr) {
            return;
        }
        listener->notify(other);
    }

public:
    NotifyingKey() = default;

    template <typename... Args>
    explicit NotifyingKey(ProbeListener<K>& listener, Args&&... args)
        : key(std::forward<Args>(args)...)
        , listener { &listener } {
        listener.addSubscriber(*this);
    }

    NotifyingKey(NotifyingKey const& other)
        : key { other.key }
        , listener { other.listener } {
        if (listener == nullptr) {
            return;
        }
        listener->addSubscriber(*this);
    }

    NotifyingKey(NotifyingKey&& other) noexcept
        : key { std::move(other.key) }
        , listener { std::move(other.listener) } {
        other.unsubscribe();

        if (listener == nullptr) {
            return;
        }
        listener->addSubscriber(*this);
    }

    auto operator=(NotifyingKey const& other) -> NotifyingKey& {
        if (&other == this) {
            return *this;
        }

        unsubscribe();

        key = other.key;
        listener = other.listener;

        if (listener != nullptr) {
            listener->addSubscriber(*this);
        }

        return *this;
    }

    auto operator=(NotifyingKey&& other) noexcept -> NotifyingKey& {
        if (&other == this) {
            return *this;
        }

        unsubscribe();

        key = std::move(other.key);
        listener = std::move(other.listener);

        other.unsubscribe();

        if (listener != nullptr) {
            listener->addSubscriber(*this);
        }

        return *this;
    }

    ~NotifyingKey() { unsubscribe(); }

    auto operator==(NotifyingKey const& other) const -> bool {
        notify(other);
        other.notify(*this);
        return key == other.key;
    }

    auto unsubscribe() -> void {
        if (listener == nullptr) {
            return;
        }

        listener->removeSubscriber(*this);
        return unsafeUnsubscribe();
    }

    auto unsafeUnsubscribe() -> void { listener = nullptr; }

    template <typename K1, typename Hash>
    friend struct NotifyingKeyHash;
};


template <typename K, typename Hash>
struct NotifyingKeyHash {
    auto operator()(NotifyingKey<K> const& key) const -> std::size_t { return Hash {}(key.key); }
};

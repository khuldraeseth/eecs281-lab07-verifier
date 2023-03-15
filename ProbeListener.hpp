#pragma once

#include <functional>
#include <unordered_set>


template <typename K>
class NotifyingKey;


template <typename K>
class ProbeListener {
public:
    virtual auto notify(NotifyingKey<K> const& key) -> void = 0;
    virtual auto finalize() const -> void = 0;

    auto addSubscriber(NotifyingKey<K>& key) -> void { subscribers.insert(&key); }

    auto removeSubscriber(NotifyingKey<K>& key) -> void { subscribers.erase(&key); }

    ProbeListener() = default;
    ProbeListener(ProbeListener const&) = default;
    ProbeListener(ProbeListener&&) noexcept = default;
    auto operator=(ProbeListener const&) -> ProbeListener& = default;
    auto operator=(ProbeListener&&) noexcept -> ProbeListener& = default;

    virtual ~ProbeListener() {
        for (auto subscriber : subscribers) {
            subscriber->unsafeUnsubscribe();
        }
    }

private:
    std::unordered_set<NotifyingKey<K>*> subscribers {};
};

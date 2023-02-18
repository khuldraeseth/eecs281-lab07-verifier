#pragma once


template <typename K>
class NotifyingKey;


template <typename K>
class ProbeListener {
public:
    virtual auto notify(NotifyingKey<K> const& key) -> void = 0;
};

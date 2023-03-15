#pragma once

#include "ProbeListener.hpp"


template <typename K>
class NotProbeSequenceVerifier : public ProbeListener<K> {
public:
    auto notify(NotifyingKey<K> const&) -> void override {}
    auto finalize() const -> void override {}
};

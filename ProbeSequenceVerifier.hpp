#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

#include "HashTable.hpp"
#include "NotifyingKey.hpp"
#include "ProbeListener.hpp"
#include "Verifier.hpp"


using Index = std::size_t;


template <typename K, typename V, typename Hash = std::hash<NotifyingKey<K>>>
class ProbeSequenceVerifier : public ProbeListener<K> {
    using Table = HashTable<NotifyingKey<K>, V, NotifyingKeyHash<K, Hash>>;
    using Bucket = typename Table::Bucket;
    static constexpr auto offset = offsetof(Bucket, key);

    std::vector<Bucket> const& buckets;

    std::vector<Index> const probeSequence {};
    std::vector<Index>::size_type nextProbe {};

    auto handleProbe(Index index) -> void {
        if (nextProbe == probeSequence.size()) {
            std::cerr << "Probe sequence too long" << std::endl;
            throw std::runtime_error("ProbeSequenceVerifier");
        }
        if (index != probeSequence[nextProbe]) {
            std::cerr << "Probe sequence mismatch: expected " << probeSequence[nextProbe] << ", got " << index
                      << std::endl;
            throw std::runtime_error("ProbeSequenceVerifier");
        }
        ++nextProbe;
    }

public:
    explicit ProbeSequenceVerifier(Table const& table, std::vector<Index> probeSequence)
        : buckets { Verifier::buckets(table) }
        , probeSequence { std::move(probeSequence) } {}

    auto notify(NotifyingKey<K> const& key) -> void override {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        auto const* bucket = reinterpret_cast<Bucket const*>(std::prev(reinterpret_cast<char const*>(&key), offset));
        auto const index = std::distance(buckets.data(), bucket);
        if (index < 0 or index >= static_cast<std::ptrdiff_t>(buckets.size())) {
            return;
        }
        handleProbe(static_cast<Index>(index));
    }

    auto finalize() const -> void override {
        if (nextProbe != probeSequence.size()) {
            std::cerr << "Probe sequence too short" << std::endl;
            throw std::runtime_error("ProbeSequenceVerifier");
        }
    }
};

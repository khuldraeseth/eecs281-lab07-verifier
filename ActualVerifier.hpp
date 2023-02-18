#pragma once


#include <cstddef>
#include <functional>
#include <iostream>
#include <type_traits>

#include "../hashtable.hpp"
#include "Kwargs.hpp"
#include "NotifyingKey.hpp"
#include "ProbeSequenceVerifier.hpp"
#include "Verifier.hpp"


template <typename K, typename V, typename H = std::hash<K>>
class ActualVerifier {
    struct StaticVerifier {
        using RawTable = HashTable<K, V, H>;

        struct PODRawTable {
            decltype(Verifier::numElements(RawTable {})) numElements;
            decltype(Verifier::numDeleted(RawTable {})) numDeleted;
            decltype(Verifier::buckets(RawTable {})) buckets;
        };

        static_assert(std::is_same<decltype(Verifier::numElements(RawTable {})), std::size_t>::value,
                      "Incorrect type for HashTable::numElements, expected std::size_t");

        static_assert(std::is_same<decltype(Verifier::numDeleted(RawTable {})), std::size_t>::value,
                      "Incorrect type for HashTable::numDeleted, expected std::size_t");

        static_assert(
          std::is_same<decltype(Verifier::buckets(RawTable {})), std::vector<typename RawTable::Bucket>>::value,
          "Incorrect type for HashTable::buckets, expected std::vector<Bucket>");

        static_assert(sizeof(RawTable) == sizeof(PODRawTable),
                      "Incorrect hash table layout. Did you add, remove, or rearrange members?");

        using RawBucket = typename RawTable::Bucket;

        struct PODBucket {
            Status status;
            K key;
            V value;
        };

        static_assert(std::is_same<decltype(RawBucket {}.status), Status>::value,
                      "Incorrect type for Bucket::status, expected Status");

        static_assert(std::is_same<decltype(RawBucket {}.key), K>::value, "Incorrect type for Bucket::key, expected K");

        static_assert(std::is_same<decltype(RawBucket {}.val), V>::value, "Incorrect type for Bucket::val, expected V");
    };

    HashTable<NotifyingKey<K>, V, NotifyingKeyHash<K, H>> table {};

public:
    using ExpectedSize = Kwarg<std::size_t, struct ExpectedSizeTag>;
    using Key = Kwarg<K, struct KeyTag>;
    using Value = Kwarg<V, struct ValueTag>;
    using ProbeSequence = Kwarg<std::vector<std::size_t>, struct ProbeSequenceTag>;

    auto size(ExpectedSize expected) const -> ActualVerifier const& {
        auto const numBuckets = Verifier::buckets(table).size();
        if (expected > numBuckets) {
            std::cerr << "Expected size " << expected << " is greater than number of buckets " << numBuckets
                      << std::endl;
            throw std::runtime_error("size");
        }

        auto const actual = table.size();
        if (actual != expected) {
            std::cerr << "Expected size " << expected << ", got " << actual << std::endl;
            throw std::runtime_error("size");
        }

        return *this;
    }

    auto size(ExpectedSize expected) -> ActualVerifier& {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        return const_cast<ActualVerifier&>(const_cast<ActualVerifier const&>(*this).size(expected));
    }

    auto insert(Key key, Value value, ProbeSequence probeSequence) -> ActualVerifier& {
#pragma message "TODO: implement insert"
        std::cout << "Insert called" << std::endl;
        ProbeSequenceVerifier<K, V, H> psv(table, probeSequence);
        NotifyingKey<K> notifyingKey { psv, key };
        static_cast<void>(table.insert(notifyingKey, value));
        psv.finalize();
        return *this;
    }

    auto erase(Key key, ProbeSequence probeSequence) -> ActualVerifier& {
#pragma message "TODO: implement erase"
        std::cout << "Erase called" << std::endl;
        ProbeSequenceVerifier<K, V, H> psv(table, probeSequence);
        NotifyingKey<K> notifyingKey { psv, key };
        static_cast<void>(table.erase(notifyingKey));
        psv.finalize();
        return *this;
    }

    auto subscript(Key key, ProbeSequence probeSequence) -> ActualVerifier& {
#pragma message "TODO: implement subscript"
        std::cout << "Subscript called" << std::endl;
        ProbeSequenceVerifier<K, V, H> psv(table, probeSequence);
        NotifyingKey<K> notifyingKey { psv, key };
        static_cast<void>(table[notifyingKey]);
        psv.finalize();
        return *this;
    }
};

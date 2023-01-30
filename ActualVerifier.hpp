#pragma once


#include <cstddef>
#include <functional>
#include <iostream>
#include <type_traits>

#include "../hashtable.hpp"
#include "Kwargs.hpp"
#include "Verifier.hpp"


template <typename K, typename V, typename H = std::hash<K>>
class ActualVerifier {
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

    static_assert(std::is_same<decltype(Verifier::buckets(RawTable {})), std::vector<typename RawTable::Bucket>>::value,
                  "Incorrect type for HashTable::buckets, expected std::vector<Bucket>");

    static_assert(sizeof(RawTable) == sizeof(PODRawTable),
                  "Incorrect hash table layout. Did you add, remove, or rearrange members?");

    // TODO: replace keys with some wrapper that can be used to verify probe sequences
    HashTable<K, V, H> table {};

public:
    using ExpectedSize = Kwarg<std::size_t, struct ExpectedSizeTag>;
    using Key = Kwarg<K, struct KeyTag>;
    using Value = Kwarg<V, struct ValueTag>;

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

    auto insert(Key key, Value value) -> ActualVerifier& {
#pragma message "TODO: implement insert"
        static_cast<void>(table.insert(key, value));
        return *this;
    }

    auto erase(Key key) -> ActualVerifier& {
#pragma message "TODO: implement erase"
        static_cast<void>(table.erase(key));
        return *this;
    }

    auto subscript(Key key) -> ActualVerifier& {
#pragma message "TODO: implement subscript"
        static_cast<void>(table[key]);
        return *this;
    }
};

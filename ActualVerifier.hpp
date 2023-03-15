#pragma once


#include <cstddef>
#include <functional>
#include <iostream>
#include <tuple>
#include <type_traits>

#include "../hashtable.hpp"
#include "EqualityChecker.hpp"
#include "Kwargs.hpp"
#include "NotEqualityChecker.hpp"
#include "NotProbeSequenceVerifier.hpp"
#include "NotifyingKey.hpp"
#include "ProbeListener.hpp"
#include "ProbeSequenceVerifier.hpp"
#include "TypeTraits.hpp"
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
    using Key = Kwarg<K, struct KeyTag>;
    using Value = Kwarg<V, struct ValueTag>;
    using ExpectedSize = Kwarg<std::size_t, struct ExpectedSizeTag>;
    using ProbeSequence = Kwarg<std::vector<std::size_t>, struct ProbeSequenceTag>;
    using InsertResult = Kwarg<bool, struct InsertResultTag>;

    template <typename... Args>
    auto size(Args&&... args) const -> ActualVerifier const& {
        return sizeImpl(std::make_tuple(std::forward<Args>(args)...));
    }

    template <typename... Args>
    auto size(Args&&... args) -> ActualVerifier& {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        return const_cast<ActualVerifier&>(const_cast<ActualVerifier const&>(*this).size(std::forward<Args>(args)...));
    }

    template <typename... Args>
    auto insert(Args&&... args) -> ActualVerifier& {
        return insertImpl(std::make_tuple(std::forward<Args>(args)...));
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

private:
    template <typename... Args>
    auto sizeImpl(std::tuple<Args...> args) const -> ActualVerifier const& {
        static_assert(count<ExpectedSize, Args...> > 0, "Error: size requires an expected size");
        static_assert(count<ExpectedSize, Args...> < 2, "Error: size allows no more than expected size");

        auto const expected = std::get<ExpectedSize>(args).value;

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

    template <typename... Args>
    auto insertImpl(std::tuple<Args...> args) -> ActualVerifier& {
        if constexpr (count<ProbeSequence, Args...> != 0) {
            static_assert(count<ProbeSequence, Args...> < 2, "Error: insert allows no more than one probe sequence");
            auto psv = ProbeSequenceVerifier<K, V, H>(table, std::get<ProbeSequence>(args).value);
            return insertImpl(psv, args);
        } else {
            auto npsv = NotProbeSequenceVerifier<K> {};
            return insertImpl(npsv, args);
        }
    }

    template <typename... Args>
    auto insertImpl(ProbeListener<K>& listener, std::tuple<Args...> args) -> ActualVerifier& {
        if constexpr (count<InsertResult, Args...> != 0) {
            static_assert(count<InsertResult, Args...> < 2, "Error: insert allows no more than one insert result");
            auto ec = EqualityChecker<bool> { std::get<InsertResult>(args).value };
            return insertImpl(listener, ec, args);
        } else {
            auto nec = NotEqualityChecker<bool> {};
            return insertImpl(listener, nec, args);
        }
    }

    template <typename... Args>
    auto insertImpl(ProbeListener<K>& listener, Consumer<bool> const& ec, std::tuple<Args...> args) -> ActualVerifier& {
        static_assert(count<Key, Args...> > 0, "Error: insert requires a key");
        static_assert(count<Key, Args...> < 2, "Error: insert allows no more than one key");
        static_assert(count<Value, Args...> > 0, "Error: insert requires a value");
        static_assert(count<Value, Args...> < 2, "Error: insert allows no more than one value");

        auto const key = std::get<Key>(args).value;
        auto const value = std::get<Value>(args).value;

        // NotifyingKey<K> notifyingKey { listener, key };
        auto notifyingKey = NotifyingKey<K> { listener, key };
        auto const result = table.insert(notifyingKey, value);
        listener.finalize();

        ec(result);

        return *this;
    }
};

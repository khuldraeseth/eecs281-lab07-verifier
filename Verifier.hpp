#pragma once


class Verifier {
public:
    template <typename Table>
    static auto numElements(Table&& table) -> decltype(auto) {
        return table.numElements;
    }

    template <typename Table>
    static auto numDeleted(Table&& table) -> decltype(auto) {
        return table.numDeleted;
    }

    template <typename Table>
    static auto buckets(Table&& table) -> decltype(auto) {
        return table.buckets;
    }
};

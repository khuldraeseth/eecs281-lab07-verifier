#pragma once


class Verifier {
public:
    template <typename Table>
    static auto numElements(Table&& table) -> auto& {
        return table.numElements;
    }

    template <typename Table>
    static auto numDeleted(Table&& table) -> auto& {
        return table.numDeleted;
    }

    template <typename Table>
    static auto buckets(Table&& table) -> auto& {
        return table.buckets;
    }
};

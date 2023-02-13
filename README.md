# EECS 281 Hash Table Verifier

Readme is still TODO.

Example usage:

```cpp
static constexpr auto expectedSize = typename ActualVerifier<int, int>::ExpectedSize::NamedParam {};
static constexpr auto expectedInsert = typename ActualVerifier<int, int>::ExpectedInsert::NamedParam {};
static constexpr auto key = typename ActualVerifier<int, int>::Key::NamedParam {};
static constexpr auto value = typename ActualVerifier<int, int>::Value::NamedParam {};

ActualVerifier<int, int> {}
    .size(expectedSize = 0)
    .insert(key = 0, value = 1)
    .size(expectedSize = 1)
    .insert(value = 4, key = 0, expectedInsert = false)
    .size(expectedSize = 1)
    // ...
```

You read that right. Keyword arguments in C++ :)

Implementation of optional arguments is still something I have to figure out. Allowing arbitrary reorderings of a fixed set, however, is easy.

I hope to allow for more complex verifications. The big one is an optional probe sequence for insert, delete, and subscript. That will mean wrapping each key in an object that can notify the verifier each time it's compared for equality with another key. Shouldn't be too difficult.

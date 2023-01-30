# EECS 281 Hash Table Verifier

Readme is still TODO.

Example usage:
```cpp
static constexpr auto expectedSize = typename ActualVerifier<int, int>::ExpectedSize::NamedParam {};
static constexpr auto key = typename ActualVerifier<int, int>::Key::NamedParam {};
static constexpr auto value = typename ActualVerifier<int, int>::Value::NamedParam {};

ActualVerifier<int, int> {}
    .size(expectedSize = 0)
    .insert(key = 0, value = 1)
    .size(expectedSize = 1);
```

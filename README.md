# EECS 281 Hash Table Verifier

A header-only C++ library for student-side testing of Lab 07 autograder solutions.

Example usage, where `BadHash{}(x) == 0` for all `x`:

```cpp
static constexpr auto key = typename ActualVerifier<int, int>::Key::NamedParam {};
static constexpr auto value = typename ActualVerifier<int, int>::Value::NamedParam {};
static constexpr auto size = typename ActualVerifier<int, int>::Size::NamedParam {};
static constexpr auto success = typename ActualVerifier<int, int>::Success::NamedParam {};
static constexpr auto probeSequence = typename ActualVerifier<int, int>::ProbeSequence::NamedParam {};

ActualVerifier<int, int, BadHash> {}
    // You can assert a size for the hash table:
    .size(size = 0)

    // You can insert a key-value pair:
    .insert(key = 0, value = 10)
    .size(size = 1)

    // You can insert a key-value pair and assert that it succeeds or fails:
    .insert(key = 0, value = 11, success = false)
    .size(size = 1)
    .insert(key = 1, value = 12, success = true)
    .size(size = 2)

    // You can assert that a specific probe sequence is followed:
    .insert(key = 2, value = 13, probeSequence = { 0, 1 })
    .size(size = 3)

    // You can assert that a key maps to a specific value under operator[]:
    .subscript(key = 0, value = 10)
    .subscript(key = 2, value = 13, probeSequence = { 0, 1, 2 })

    // You can erase a key and its associated value:
    .erase(key = 0, success = true)
    .size(size = 2)
    .erase(key = 0, success = false)
    .size(size = 2)
    .subscript(key = 0, value = 0)
    .size(size = 3)

    .erase(key = 1, success = true, probeSequence = { 0, 1 })
    .insert(key = 1, value = 14, probeSequence = { 0, 2 })
    // ...
```

You read that right. Keyword arguments in C++ :)

## How to use

1. Clone this repository in your Lab 07 autograder directory:

```bash
$ cd /path/to/the/code/
$ git clone git@github.com:khuldraeseth/eecs281-lab07-verifier.git verifier
```

2. Write a program using the verifier! You can edit `hash.cpp` to serve this purpose if you'd like. `#include "verifier/ActualVerifier.hpp"` to be able to use code like the example above.

## How it behaves

Some static assertions will inform you when you're using the verifier incorrectly. If your test does not compile, read the first error message.

A successful test will do nothing. A failed test will throw an exception. For now, I recommend using a debugger to see exactly what went wrong, since the exception message is not yet very helpful.

For a hash table with key type `K` and value type `V`:

### `size`

One required parameter `size` of type `std::size_t`. Throws an exception if the hash table does not have this size.

### `insert`

Two required parameters `key` and `value` of types `K` and `V`. Passes these along to the hash table's `insert` method.

Optional parameter `success` of type `bool`. If provided, asserts that the `insert` method returns this value and throws an exception if it does not.

Optional parameter `probeSequence` of type `std::vector<std::size_t>`. If provided, asserts that the `insert` method follows this probe sequence and throws an exception if it does not.

### `erase`

One required parameter `key`. Passes this along to the hash table's `erase` method.

Optional parameter `success` of type `bool`. If provided, asserts that the `erase` method returns this value and throws an exception if it does not.

Optional parameter `probeSequence` of type `std::vector<std::size_t>`. If provided, asserts that the `erase` method follows this probe sequence and throws an exception if it does not.

### `subscript`

One required parameter `key`. Passes this along to the hash table's `operator[]` method.

Optional parameter `value` of type `V`. If provided, asserts that the `operator[]` method returns this value and throws an exception if it does not.

Optional parameter `probeSequence` of type `std::vector<std::size_t>`. If provided, asserts that the `operator[]` method follows this probe sequence and throws an exception if it does not.

## Caveats and limitations

- This is C++17. If you want to run on CAEN, make sure you're using a somewhat recent version of `g++`. At the command line, `module unload gcc` and `module load gcc/11.3.0` should do the trick to select a suitable compiler.

- You need to know how your hash function behaves in order to know where to begin probe sequences.

- Mid-operation rehashing while a probe sequence is being checked is not supported. When you expect a rehash to occur, don't provide a probe sequence.

- `probeSequence = {}` does not work. Instead, use `probeSequence = std::vector<std::size_t>{}` to specify an empty probe sequence.

- The probe sequence is the sequence of indices that the hash table checks using `operator==` against the provded key. It does not include indices of ghosts. In the case of an insert, it does not include the index where the key-value pair is inserted.

- The example code above may or may not be a test that should succeed. It's intended as nothing more than an example.

- This library is imperfect and quite possibly buggy. It's in a very early stage right now. If you find what appears to be a bug, please let me know!

## How it works

There are two interesting and orthogonal components here.

### Probe sequence verification

Class template `NotifyingKey` wraps a key of an arbitrary type and notifies a `ProbeListener` each time it's compared for equality with another key. The `ProbeListener` makes sense of the sequence of probes it's told about. This library hijacks hash table operations by wrapping all keys in `NotifyingKey`, so it's able to observe comparisons that would ordinarily happen behind the scenes.

### Keyword arguments

The names of the keyword arguments are actually objects that with their assignment operators simply return the right-hand side of the assignment, more or less. The expression `size = 0` merely evaluates to `0`.

It's not quite that simple, actually. `size = 0` evaluates to `0` _in a [`newtype`](https://wiki.haskell.org/Newtype) wrapper_. This lets me use `std::get` to extract the value from a tuple of arguments even in the presence of multiple keyword arguments of the same type.

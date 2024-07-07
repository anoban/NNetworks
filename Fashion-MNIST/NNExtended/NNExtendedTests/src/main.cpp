#include <cstdio>
#include <cstdlib>

extern void TEST_ITERATORS() noexcept; // in iterator.cpp
extern void TEST_IDXIO() noexcept;     // in idxio.cpp

auto wmain() -> int {
    TEST_ITERATORS();
    TEST_IDXIO();
    ::_putws(L"All tests have passed!");
    return EXIT_SUCCESS;
}
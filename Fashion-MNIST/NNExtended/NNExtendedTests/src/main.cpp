// this project is solely for testing purpose
// made this into a separate project because having a all tests in single file within the main project felt really messy
// and opted to use MSVC++ instead of Intel's compiler :(

#include <cstdlib>

extern void TEST_ITERATORS() noexcept; // iterator.cpp
extern void TEST_IDXIO() noexcept;     // idxio.cpp
extern void TEST_MATRIX() noexcept;    // matrix.cpp

auto wmain() -> int {
    TEST_ITERATORS();
    TEST_IDXIO();
    TEST_MATRIX();

    return EXIT_SUCCESS;
}
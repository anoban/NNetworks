// opting to use MSVC++ for tests because address sanitizer is not supported in VS2022 with Intel C++ compiler :(

#include <cstdio>
#include <cstdlib>

// clang-format off
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_MEAN
#define _AMD64_
#include <windef.h>
#include <WinBase.h>    // ::GetCurrentDirectoryW()
// clang-format on

extern void TEST_ITERATORS() noexcept; // iterator.cpp
extern void TEST_IDXIO();              // idxio.cpp
extern void TEST_MATRIX() noexcept;    // matrix.cpp

auto wmain() -> int {
    wchar_t cwdirectory[MAX_PATH] {}; // NOLINT(modernize-avoid-c-arrays)
    ::GetCurrentDirectoryW(MAX_PATH, cwdirectory);
    ::_putws(cwdirectory);

    TEST_ITERATORS();
    TEST_IDXIO();
    TEST_MATRIX();

    return EXIT_SUCCESS;
}
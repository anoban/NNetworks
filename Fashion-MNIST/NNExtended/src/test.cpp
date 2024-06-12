#if defined(_DEBUG) && defined(__TEST__)

    #include <exception.hpp>
    #include <idxio.hpp>
    #include <iterator.hpp>
    #include <utilities.hpp>

auto wmain() -> int {
    #pragma region    __TEST_IDXIO__
    #pragma endregion __TEST_IDXIO__

    #pragma region    __TEST_NNEXT_EXCEPTION__
    #pragma endregion __TEST_NNEXT_EXCEPTION__

    #pragma region    __TEST_ITERATOR__
    #pragma endregion __TEST_ITERATOR__

    #pragma region    __TEST_UTILITIES__
    #pragma endregion __TEST_UTILITIES__

    return EXIT_SUCCESS;
}

#endif

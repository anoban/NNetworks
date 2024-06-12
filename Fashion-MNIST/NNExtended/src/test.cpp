#if defined(_DEBUG) && defined(__TEST__)

    #include <exception.hpp>
    #include <idxio.hpp>
    #include <iterator.hpp>
    #include <utilities.hpp>

auto wmain() -> int {
    #pragma region    __TEST_IDXIO__
    #pragma endregion __TEST_IDXIO__

    #pragma region __TEST_NNEXT_EXCEPTION__

    constexpr nnext_exception defexcpt {};
    assert(!defexcpt.what());

    nnext_exception excpt { "Something bad happened!" };
    nnext_exception wexcpt { L"Something bad happened!" };

    nnext_exception copy { excpt };
    assert(copy.what() == excpt.what());

    nnext_exception move { std::move(excpt) };
    assert(!excpt.what());

    try {
        throw nnext_exception { L"Catch this!\n" };
    } catch (const nnext_exception<wchar_t>& nwexcpt) { _putws(nwexcpt.what()); }

    try {
        throw nnext_exception { "Catch this!\n" };
    } catch (const nnext_exception<char>& nwexcpt) { puts(nwexcpt.what()); }

    #pragma endregion __TEST_NNEXT_EXCEPTION__

    #pragma region    __TEST_ITERATOR__
    #pragma endregion __TEST_ITERATOR__

    #pragma region    __TEST_UTILITIES__
    #pragma endregion __TEST_UTILITIES__

    return EXIT_SUCCESS;
}

#endif

#if defined(_DEBUG) && defined(__TEST__)

    #include <algorithm>
    #include <numeric>
    #include <random>
    #include <vector>

    #include <iterator.hpp>

auto wmain() -> int {
    #pragma region __TEST_ITERATOR__
    constexpr unsigned max { 10'000 };
    int                randoms[max] { 0 }; // NOLINT

    // test the default ctor
    constexpr random_access_iterator<const double> empty {};
    assert(!empty._rsrc);
    assert(!empty._length);
    assert(!empty._offset);

    random_access_iterator begin { randoms, __crt_countof(randoms) };
    random_access_iterator end { randoms, __crt_countof(randoms), __crt_countof(randoms) };

    // test the ctors
    assert(begin._rsrc == end._rsrc);
    assert(begin._length == end._length);
    assert(begin._offset + __crt_countof(randoms) == end._offset);

    // test the copy ctor
    auto begin_copy { begin };
    auto end_copy { end };

    assert(begin_copy._rsrc == randoms);
    assert(begin_copy._length == __crt_countof(randoms));
    assert(!begin_copy._offset);

    assert(end_copy._rsrc == randoms);
    assert(end_copy._length == __crt_countof(randoms));
    assert(end_copy._offset == __crt_countof(randoms));

    std::iota(begin, end, 11); // fill the array with values, start with 11

    assert(begin._Unwrapped() == randoms);
    assert(*begin == 11);
    assert(begin_copy._Unwrapped() == randoms);
    assert(*begin_copy == 11);

    assert(end._Unwrapped() == randoms);
    assert(end._Unwrapped() == randoms);

    random_access_iterator<const int> cbegin { randoms, __crt_countof(randoms) };
    random_access_iterator<const int> cend { randoms, __crt_countof(randoms), __crt_countof(randoms) };

    assert(cbegin._Unwrapped() == randoms);
    assert(*cbegin == 11);
    assert(cend._Unwrapped() == randoms);
    // assert(*cend == randoms[__crt_countof(randoms) - 1]);

    const auto sum    = std::accumulate(begin, end, 0LL);
    const auto csum   = std::accumulate(cbegin, cend, 0LL);
    const auto stdsum = std::accumulate(std::begin(randoms), std ::end(randoms), 0LL);
    assert(sum == stdsum);
    assert(csum == stdsum);

    for (auto it = std::cbegin(randoms), end = std::cend(randoms); it != end; ++it, ++begin_copy) assert(*it == *begin_copy);

    #pragma endregion __TEST_ITERATOR__

    return EXIT_SUCCESS;
}

#endif

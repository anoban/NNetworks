#if defined(_DEBUG) && defined(__TEST__)

    #include <algorithm>
    #include <numeric>
    #include <random>
    #include <vector>

    #include <iterator.hpp>

auto wmain() -> int {
    #pragma region __TEST_ITERATOR__
    constexpr unsigned max { 10'000 };
    int                randoms[max] { 0 }; // plain old c style array

    random_access_iterator begin { randoms, __crt_countof(randoms) };
    auto                   beginc { begin };
    random_access_iterator end { randoms, __crt_countof(randoms), __crt_countof(randoms) };

    std::random_device rdevice {};
    std::mt19937_64    rndeng { rdevice() };
    std::generate(begin, end, rndeng);

    random_access_iterator<const int> cbegin { randoms, __crt_countof(randoms) };
    random_access_iterator<const int> cend { randoms, __crt_countof(randoms), __crt_countof(randoms) };

    const auto sum    = std::accumulate(begin, end, 0LL);
    const auto csum   = std::accumulate(cbegin, cend, 0LL);
    const auto stdsum = std::accumulate(std::begin(randoms), std ::end(randoms), 0LL);
    assert(sum == stdsum);
    assert(csum == stdsum);

    for (auto it = std::cbegin(randoms), end = std::cend(randoms); it != end; ++it, ++beginc) assert(*it == *beginc);

    #pragma endregion __TEST_ITERATOR__

    return EXIT_SUCCESS;
}

#endif

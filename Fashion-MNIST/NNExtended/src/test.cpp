#if defined(_DEBUG) && defined(__TEST__)

// clang-format off
    #include <idxio.hpp>
// clang-format on

    #include <algorithm>
    #include <array>
    #include <chrono>
    #include <iostream>
    #include <numeric>
    #include <random>
    #include <ranges>
    #include <vector>

static constexpr unsigned max { 1000 };
static constexpr unsigned maxx { 784 };

auto wmain() -> int {
    #pragma region __TEST_ITERATOR__

    int   randoms[max] { 0 };      // NOLINT(modernize-avoid-c-arrays)
    float frandoms[maxx] { 0.00 }; // NOLINT(modernize-avoid-c-arrays)

    std::mt19937_64 rndengine { static_cast<unsigned long long>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) };
    std::uniform_real_distribution urealdistr { -0.5, 0.5 }; // min, max

    // test the default ctor for const iterator
    constexpr random_access_iterator<const double> empty {};
    assert(!empty._rsrc);
    assert(!empty._length);
    assert(!empty._offset);

    // test the default ctor for mutable iterator
    constexpr random_access_iterator<char> emp {};
    assert(!emp._rsrc);
    assert(!emp._length);
    assert(!emp._offset);

    random_access_iterator begin { randoms, __crt_countof(randoms) };
    random_access_iterator end { randoms, __crt_countof(randoms), __crt_countof(randoms) };

    random_access_iterator fbegin { frandoms, __crt_countof(frandoms) };
    random_access_iterator fend { frandoms, __crt_countof(frandoms), __crt_countof(frandoms) };

    // test the ctors
    assert(begin._rsrc == randoms);
    assert(begin._length == __crt_countof(randoms));
    assert(!begin._offset);

    assert(end._rsrc == randoms);
    assert(end._length == __crt_countof(randoms));
    assert(end._offset == __crt_countof(randoms));

    assert(fbegin._rsrc == frandoms);
    assert(fbegin._length == __crt_countof(frandoms));
    assert(!fbegin._offset);

    assert(fend._rsrc == frandoms);
    assert(fend._length == __crt_countof(frandoms));
    assert(fend._offset == __crt_countof(frandoms));

    // test the copy ctor
    auto begin_copy { begin };
    auto end_copy { end };

    assert(begin_copy._rsrc == randoms);
    assert(begin_copy._length == __crt_countof(randoms));
    assert(!begin_copy._offset);

    assert(end_copy._rsrc == randoms);
    assert(end_copy._length == __crt_countof(randoms));
    assert(end_copy._offset == __crt_countof(randoms));

    // test the move ctor
    auto mfbegin { std::move(fbegin) };
    auto mfend { std::move(fend) };

    assert(mfbegin._rsrc == frandoms);
    assert(mfbegin._length == __crt_countof(frandoms));
    assert(!mfbegin._offset);

    assert(mfend._rsrc == frandoms);
    assert(mfend._length == __crt_countof(frandoms));
    assert(mfend._offset == __crt_countof(frandoms));

    // test the moved from object
    assert(!fbegin._rsrc);
    assert(!fbegin._length);
    assert(!fbegin._offset);

    assert(!fend._rsrc);
    assert(!fend._length);
    assert(!fend._offset);

    // fill the array with values
    std::iota(begin + 10, end, 11); // start filing from 11th element, first 10 will be 0s.
    // first 200 will be between urealdistr.min() and urealdistr.max()
    std::for_each(mfbegin, mfbegin + 200, [&urealdistr, &rndengine](decltype(mfbegin)::value_type& _) constexpr noexcept -> void {
        _ = static_cast<decltype(mfbegin)::value_type>(urealdistr(rndengine));
    });
    assert(maxx >= 200);
    // the rest will be filled unrestricted by the random engine
    std::for_each(mfbegin + 200, mfend, [&rndengine](decltype(mfbegin)::value_type& _) constexpr noexcept -> void {
        _ = static_cast<float>(rndengine());
    });

    for (unsigned i = 0; i < 200; ++i) assert(frandoms[i] >= -0.5 && frandoms[i] <= 0.5);

    assert(begin._Unwrapped() == randoms);
    assert(*begin == 0);
    assert(*(begin + 10) == 11);
    assert(begin_copy._Unwrapped() == randoms);
    assert(*begin_copy == 0);
    assert(*(begin_copy + 10) == 11);

    assert(end._Unwrapped() == randoms);
    assert(end._Unwrapped() == randoms);

    random_access_iterator<const int> cbegin { randoms, __crt_countof(randoms) };
    random_access_iterator<const int> cend { randoms, __crt_countof(randoms), __crt_countof(randoms) };

    assert(cbegin._Unwrapped() == randoms);
    assert(*cbegin == 0);
    assert(*(cbegin + 10) == 11);
    assert(cend._Unwrapped() == randoms);

    assert(std::accumulate(cbegin, cbegin + 10, 0) == 0); // we did not fill the first 10 spaces!

    const auto sum    = std::accumulate(begin, end, 0LL);
    const auto csum   = std::accumulate(cbegin, cend, 0LL);
    const auto stdsum = std::accumulate(std::begin(randoms), std ::end(randoms), 0LL);
    assert(sum == stdsum);
    assert(csum == stdsum);

    for (auto it = std::cbegin(randoms), end = std::cend(randoms); it != end; ++it, ++begin_copy) assert(*it == *begin_copy);
    std::vector<decltype(mfbegin)::unqualified_value_type> frs { mfbegin, mfend };
    assert(frs.size() == maxx);
    std::vector<decltype(begin)::unqualified_value_type> rs { begin, end };
    assert(rs.size() == max);

    #pragma endregion __TEST_ITERATOR__

    #pragma region __TEST_IDX__

    const idxio::idx1 train_labels { L"../idx/train-labels-idx1-ubyte" };
    const idxio::idx1 test_labels { L"../idx/t10k-labels-idx1-ubyte" };

    std::wcout << train_labels;
    std::wcout << test_labels;

    assert(train_labels.count() == 60'000);
    assert(test_labels.count() == 10'000);
    assert(train_labels.magic() == 2049);
    assert(test_labels.magic() == 2049);

    const idxio::idx3 train_images { L"../idx/train-images-idx3-ubyte" };
    const idxio::idx3 test_images { L"../idx/t10k-images-idx3-ubyte" };

    std::wcout << train_images;
    std::wcout << test_images;

    std::array<unsigned, 10> frequencies {};

    for (const auto& l : train_labels) frequencies.at(l)++;
    assert(std::all_of(frequencies.cbegin(), frequencies.cend(), [](const int& count) constexpr noexcept -> bool { return count == 6'000; })
    );

    std::fill(frequencies.begin(), frequencies.end(), 0);
    for (const auto& l : test_labels) frequencies.at(l)++;
    assert(std::all_of(frequencies.cbegin(), frequencies.cend(), [](const int& count) constexpr noexcept -> bool { return count == 1'000; })
    );

    const auto trainltotal { std::reduce(train_labels.cbegin(), train_labels.cend(), 0LLU) };
    const auto strainlabs  = train_labels.labels_astype<short>();
    const auto ltrainlabs  = train_labels.labels_astype<long>();
    const auto lltrainlabs = train_labels.labels_astype<long long>();
    const auto ftrainlabs  = train_labels.labels_astype<float>();
    const auto dtrainlabs  = train_labels.labels_astype<double>();

    assert(trainltotal == std::accumulate(strainlabs.cbegin(), strainlabs.cend(), 0LLU));
    assert(trainltotal == std::accumulate(ltrainlabs.cbegin(), ltrainlabs.cend(), 0LLU));
    assert(trainltotal == std::accumulate(lltrainlabs.cbegin(), lltrainlabs.cend(), 0LLU));
    assert(trainltotal == std::accumulate(ftrainlabs.cbegin(), ftrainlabs.cend(), 0LLU));
    assert(trainltotal == std::accumulate(dtrainlabs.cbegin(), dtrainlabs.cend(), 0LLU));

    std::array<unsigned, std::numeric_limits<decltype(train_images)::value_type>::max() + 1> pixfreqs {};
    for (const auto& p : train_images) pixfreqs.at(p)++;
    std::wcout << L"Frequency of pixels in the training image dataset :: \n";
    for (unsigned i = 0; const auto& f : pixfreqs) std::wcout << i++ << L") " << f << L'\n';

    for (const auto& p : test_images) pixfreqs.at(p)++;
    std::wcout << L"Frequency of pixels in the test image dataset :: \n";
    for (unsigned i = 0; const auto& f : pixfreqs) std::wcout << i++ << L") " << f << L'\n';

    #pragma endregion __TEST_IDX__

    return EXIT_SUCCESS;
}

#endif

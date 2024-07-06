// clang .\src\test.cpp -I .\include\ -Wall -Wextra -O3 -pedantic -march=native -std=c++23 -D__TEST__ -D_DEBUG -DDEBUG -c -g0
// link.exe .\test.o kernel32.lib vcruntimed.lib msvcrtd.lib /DEBUG:NONE

#if defined(_DEBUG) && defined(__TEST__)
// clang-format off
    #include <iterator.hpp>
    #include <idxio.hpp>
    #include <matrix.hpp>
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
    assert(!fbegin._rsrc); // NOLINT(bugprone-use-after-move)
    assert(!fbegin._length);
    assert(!fbegin._offset);

    assert(!fend._rsrc); // NOLINT(bugprone-use-after-move)
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

    for (unsigned i = 0; i < 200; ++i)
        assert(frandoms[i] >= -0.5 && frandoms[i] <= 0.5); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)

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
    std::vector<decltype(mfbegin)::value_type> frs { mfbegin, mfend };
    assert(frs.size() == maxx);
    std::vector<decltype(begin)::value_type> rs { begin, end };
    assert(rs.size() == max);

    #pragma endregion

    #pragma region __TEST_IDX__

    idxio::idx1 train_labels { L"../idx/train-labels-idx1-ubyte" };
    idxio::idx1 test_labels { L"../idx/t10k-labels-idx1-ubyte" };

    std::wcout << train_labels;
    std::wcout << test_labels;

    assert(train_labels._idxmagic == 2049);
    assert(train_labels.magic() == 2049);
    assert(train_labels._nlabels == 60'000);
    assert(train_labels.count() == 60'000);
    assert(train_labels._raw_buffer);
    assert(train_labels._labels == train_labels._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    assert(test_labels._idxmagic == 2049);
    assert(test_labels.magic() == 2049);
    assert(test_labels._nlabels == 10'000);
    assert(test_labels.count() == 10'000);
    assert(test_labels._raw_buffer);
    assert(test_labels._labels == test_labels._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    idxio::idx3 train_images { L"../idx/train-images-idx3-ubyte" };
    idxio::idx3 test_images { L"../idx/t10k-images-idx3-ubyte" };

    assert(train_images._idxmagic == 2051);
    assert(train_images.magic() == 2051);
    assert(train_images._nimages == 60'000);
    assert(train_images.count() == 60'000);
    assert(train_images._raw_buffer);
    assert(train_images._pixels == train_images._raw_buffer + 16); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    assert(train_images._ncols == 28);
    assert(train_images._nrows == 28);
    auto [rows, cols] = train_images.dim();
    assert(rows == 28 && cols == 28);

    assert(test_images._idxmagic == 2051);
    assert(test_images.magic() == 2051);
    assert(test_images._nimages == 10'000);
    assert(test_images.count() == 10'000);
    assert(test_images._raw_buffer);
    assert(test_images._pixels == test_images._raw_buffer + 16); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    assert(test_images._ncols == 28);
    assert(test_images._nrows == 28);
    std::tie(rows, cols) = test_images.dim();
    assert(rows == 28 && cols == 28);

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
    assert(std::reduce(pixfreqs.cbegin(), pixfreqs.cend(), 0LLU) == 60'000 * 784);

    std::ranges::fill(pixfreqs, 0);
    for (const auto& p : test_images) pixfreqs.at(p)++;
    assert(std::reduce(pixfreqs.cbegin(), pixfreqs.cend(), 0LLU) == 10'000 * 784);

    // test the default ctors
    constexpr idxio::idx1 empty_idx1 {};
    constexpr idxio::idx3 empty_idx3 {};
    std::wcout << empty_idx1;
    std::wcout << empty_idx3;

    assert(!empty_idx1._idxmagic);
    assert(!empty_idx1._nlabels);
    assert(!empty_idx1._raw_buffer);
    assert(!empty_idx1._labels);

    assert(!empty_idx3._idxmagic);
    assert(!empty_idx3._nimages);
    assert(!empty_idx3._nrows);
    assert(!empty_idx3._ncols);
    assert(!empty_idx3._raw_buffer);
    assert(!empty_idx3._pixels);

    // test the copy ctors
    auto cpy_trlabs { train_labels };
    assert(cpy_trlabs._idxmagic == 2049);
    assert(cpy_trlabs.magic() == 2049);
    assert(cpy_trlabs._nlabels == 60'000);
    assert(cpy_trlabs.count() == 60'000);
    assert(train_labels._raw_buffer != cpy_trlabs._raw_buffer);
    assert(cpy_trlabs._raw_buffer);
    assert(cpy_trlabs._labels == cpy_trlabs._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    for (const auto& i : std::ranges::views::iota(0, 10))
        assert(std::count(train_labels.cbegin(), train_labels.cend(), i) == std::count(cpy_trlabs.cbegin(), cpy_trlabs.cend(), i));
    assert(std::reduce(train_labels.cbegin(), train_labels.cend(), 0LLU) == std::reduce(cpy_trlabs.cbegin(), cpy_trlabs.cend(), 0LLU));

    // test the move ctor
    const auto* const trainlbl = train_labels._raw_buffer;
    const auto* const testlbl  = test_labels._raw_buffer;

    auto       train_labelsmvd { std::move(train_labels) };
    const auto test_labelsmvd { std::move(test_labels) };

    assert(!train_labels._idxmagic);
    assert(!train_labels.magic());
    assert(!train_labels._nlabels);
    assert(!train_labels.count());
    assert(!train_labels._raw_buffer);
    assert(!train_labels._labels);
    assert(!train_labels._raw_buffer);

    assert(!test_labels._idxmagic);
    assert(!test_labels.magic());
    assert(!test_labels._nlabels);
    assert(!test_labels.count());
    assert(!test_labels._raw_buffer);
    assert(!test_labels._labels);
    assert(!test_labels._raw_buffer);

    assert(trainlbl == train_labelsmvd._raw_buffer);
    assert(testlbl == test_labelsmvd._raw_buffer);

    for (const auto& i : std::ranges::views::iota(0, 10))
        assert(std::count(train_labelsmvd.cbegin(), train_labelsmvd.cend(), i) == std::count(cpy_trlabs.cbegin(), cpy_trlabs.cend(), i));
    for (const auto& i : std::ranges::views::iota(0, 10)) {
        assert(std::count(train_labelsmvd.cbegin(), train_labelsmvd.cend(), i) == 6'000);
        assert(std::count(test_labelsmvd.cbegin(), test_labelsmvd.cend(), i) == 1'000);
    }
    assert(
        std::reduce(train_labelsmvd.cbegin(), train_labelsmvd.cend(), 0LLU) == std::reduce(cpy_trlabs.cbegin(), cpy_trlabs.cend(), 0LLU)
    );

    // test the copy assignment operator
    const auto* const ptr = train_labelsmvd._raw_buffer;
    train_labelsmvd       = test_labelsmvd;     // train labels object has a bigger buffer than the test labels object
    assert(train_labelsmvd._raw_buffer == ptr); // no reallocation should have happened!

    // even though train_labelsmvd now has lesser elements than what could fill its buffer, it sould repurpose its iterators accordingly
    for (const auto& i : std::ranges::views::iota(0, 10)) assert(std::count(train_labelsmvd.cbegin(), train_labelsmvd.cend(), i) == 1'000);

    std::wcout << train_labelsmvd;
    std::wcout << test_labelsmvd;

    #pragma endregion

    #pragma region __TEST_MATRIX__
    matrix fmat { 10, 10 };
    #pragma endregion

    return EXIT_SUCCESS;
}

#endif

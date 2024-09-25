#include <algorithm>
#include <array>
#include <numeric>
#include <ranges>

#include <CppUnitTest.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#include <idxio.hpp>

namespace NNExtUnitTest {
    TEST_CLASS(idx1) {
    public:
        TEST_METHOD(constructor) {
            idxio::idx1 train_labels { LR"(./../../Fashion-MNIST/train-labels-idx1-ubyte)" };
            idxio::idx1 test_labels { LR"(./../../Fashion-MNIST/t10k-labels-idx1-ubyte)" };

            Assert::AreEqual(train_labels._idxmagic, 2049U);
            Assert::AreEqual(train_labels.magic(), 2049U);
            Assert::AreEqual(train_labels._nlabels, 60'000U);
            Assert::AreEqual(train_labels.count(), 60'000U);
            Assert::AreEqual(train_labels._labels, train_labels._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            Assert::IsNotNull(train_labels._raw_buffer);

            Assert::AreEqual(test_labels._idxmagic, 2049U);
            Assert::AreEqual(test_labels.magic(), 2049U);
            Assert::AreEqual(test_labels._nlabels, 10'000U);
            Assert::AreEqual(test_labels.count(), 10'000U);
            Assert::IsNotNull(test_labels._raw_buffer);
            Assert::AreEqual(test_labels._labels, test_labels._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }

        std::wcout << train_labels;
        std::wcout << test_labels;

        std::wcout << train_images;
        std::wcout << test_images;

        std::array<unsigned, 10> frequencies {};

        for (const auto& l : train_labels) frequencies.at(l)++;
        assert(std::all_of(frequencies.cbegin(), frequencies.cend(), [](const int& count) constexpr noexcept -> bool {
            return count == 6'000;
        }));

        std::fill(frequencies.begin(), frequencies.end(), 0);
        for (const auto& l : test_labels) frequencies.at(l)++;
        assert(std::all_of(frequencies.cbegin(), frequencies.cend(), [](const int& count) constexpr noexcept -> bool {
            return count == 1'000;
        }));

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
            assert(
                std::count(train_labelsmvd.cbegin(), train_labelsmvd.cend(), i) == std::count(cpy_trlabs.cbegin(), cpy_trlabs.cend(), i)
            );
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
        for (const auto& i : std::ranges::views::iota(0, 10))
            assert(std::count(train_labelsmvd.cbegin(), train_labelsmvd.cend(), i) == 1'000);

        std::wcout << train_labelsmvd;
        std::wcout << test_labelsmvd;

        ::_putws(L"TEST_IDXIO passed :)");
    };

    TEST_CLASS(idx3) {
        TEST_METHOD(constructor) {
            idxio::idx3 train_images { LR"(./../../Fashion-MNIST/train-images-idx3-ubyte)" };
            idxio::idx3 test_images { LR"(./../../Fashion-MNIST/t10k-images-idx3-ubyte)" };

            Assert::AreEqual(train_images._idxmagic, 2051U);
            Assert::AreEqual(train_images.magic(), 2051U);
            Assert::AreEqual(train_images._nimages, 60'000U);
            Assert::AreEqual(train_images.count(), 60'000U);
            Assert::IsNotNull(train_images._raw_buffer);
            Assert::AreEqual(
                train_images._pixels, train_images._raw_buffer + 16
            ); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            Assert::AreEqual(train_images._ncols, 28U);
            Assert::AreEqual(train_images._nrows, 28U);
            auto [rows, cols] = train_images.dim();
            Assert::AreEqual(rows, 28U);
            Assert::AreEqual(cols, 28U);

            Assert::AreEqual(test_images._idxmagic, 2051U);
            Assert::AreEqual(test_images.magic(), 2051U);
            Assert::AreEqual(test_images._nimages, 10'000U);
            Assert::AreEqual(test_images.count(), 10'000U);
            Assert::IsNotNull(test_images._raw_buffer);
            Assert::AreEqual(test_images._pixels, test_images._raw_buffer + 16); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            Assert::AreEqual(test_images._ncols, 28U);
            Assert::AreEqual(test_images._nrows, 28U);
            std::tie(rows, cols) = test_images.dim();
            Assert::AreEqual(rows, 28U);
            Assert::AreEqual(cols, 28U);
        }
    }
} // namespace NNExtUnitTest

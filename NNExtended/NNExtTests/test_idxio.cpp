#include <algorithm>
#include <array>
#include <numeric>
#include <ranges>

#include <idxio.hpp>

// clang-format off
    #include <gtest/gtest.h>
// clang-format on

extern idxio::idx1 ltrain, ltest; // defined in main.cpp and initialized by main()
extern idxio::idx3 itrain, itest;

TEST(IDX1, DEFAULT_CONSTRUCTOR) {
    const idxio::idx1 idx1 {};
    EXPECT_FALSE(idx1._idxmagic);
    EXPECT_FALSE(idx1._nlabels);
    EXPECT_FALSE(idx1._raw_buffer);
    EXPECT_FALSE(idx1._labels);
}

TEST(IDX1, CONSTRUCTOR) {
    EXPECT_EQ(ltrain._idxmagic, 2049U);
    EXPECT_EQ(ltrain._nlabels, 60'000U);
    EXPECT_EQ(ltrain._labels, ltrain._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_TRUE(ltrain._raw_buffer);

    EXPECT_EQ(ltest._idxmagic, 2049U);
    EXPECT_EQ(ltest._nlabels, 10'000U);
    EXPECT_EQ(ltest._labels, ltest._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_TRUE(ltest._raw_buffer);
}

TEST(IDX1, COPY_CONSTRUCTOR) {
    const auto traincp { ltrain };
    const auto testcp { ltest };

    EXPECT_EQ(traincp._idxmagic, 2049U);
    EXPECT_EQ(traincp._nlabels, 60'000U);
    EXPECT_EQ(traincp._labels, traincp._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_TRUE(traincp._raw_buffer);

    EXPECT_EQ(testcp._idxmagic, 2049U);
    EXPECT_EQ(testcp._nlabels, 10'000U);
    EXPECT_EQ(testcp._labels, testcp._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_TRUE(testcp._raw_buffer);
}

TEST(IDX1, ITERATORS) {
    std::array<unsigned, 10> labfreqs {}; // frequencies of each unique labels

    for (idxio::idx1::const_iterator it = ltrain.cbegin(), end = ltrain.cend(); it != end; ++it) labfreqs.at(*it)++;
    EXPECT_TRUE(std::ranges::all_of(labfreqs, [](const auto& count) noexcept -> bool { return count == 6'000U; }));

    std::fill(labfreqs.begin(), labfreqs.end(), 0);
    for (idxio::idx1::const_iterator it = ltest.cbegin(), end = ltest.cend(); it != end; ++it) labfreqs.at(*it)++;
    EXPECT_TRUE(std::ranges::all_of(labfreqs, [](const auto& count) noexcept -> bool { return count == 1'000U; }));
}

TEST(IDX1, AS_TYPE) {
    const auto tr_total { std::reduce(ltrain.cbegin(), ltrain.cend(), 0.0L) };
    const auto tst_total { std::reduce(ltest.cbegin(), ltest.cend(), 0.0L) };

    auto s_labels { ltrain.labels_astype<short>() };
    auto l_labels { ltrain.labels_astype<long>() };
    auto ll_labels { ltrain.labels_astype<long long>() };
    auto f_labels { ltrain.labels_astype<float>() };
    auto d_labels { ltrain.labels_astype<double>() };

    EXPECT_EQ(tr_total, std::accumulate(s_labels.cbegin(), s_labels.cend(), 0.0L));
    EXPECT_EQ(tr_total, std::accumulate(l_labels.cbegin(), l_labels.cend(), 0.0L));
    EXPECT_EQ(tr_total, std::accumulate(ll_labels.cbegin(), ll_labels.cend(), 0.0L));
    EXPECT_EQ(tr_total, std::accumulate(f_labels.cbegin(), f_labels.cend(), 0.0L));
    EXPECT_EQ(tr_total, std::accumulate(d_labels.cbegin(), d_labels.cend(), 0.0L));

    s_labels  = ltest.labels_astype<short>();
    l_labels  = ltest.labels_astype<long>();
    ll_labels = ltest.labels_astype<long long>();
    f_labels  = ltest.labels_astype<float>();
    d_labels  = ltest.labels_astype<double>();

    EXPECT_EQ(tst_total, std::accumulate(s_labels.cbegin(), s_labels.cend(), 0.0L));
    EXPECT_EQ(tst_total, std::accumulate(l_labels.cbegin(), l_labels.cend(), 0.0L));
    EXPECT_EQ(tst_total, std::accumulate(ll_labels.cbegin(), ll_labels.cend(), 0.0L));
    EXPECT_EQ(tst_total, std::accumulate(f_labels.cbegin(), f_labels.cend(), 0.0L));
    EXPECT_EQ(tst_total, std::accumulate(d_labels.cbegin(), d_labels.cend(), 0.0L));
}

TEST(IDX1, MOVE_CONSTRUCTOR) { // testing move semantics at last otherwise the globals will become unusable for the other test cases
    const auto trainmv { std::move(ltrain) };
    const auto testmv { std::move(ltest) };

    EXPECT_EQ(trainmv._idxmagic, 2049U);
    EXPECT_EQ(trainmv._nlabels, 60'000U);
    EXPECT_EQ(trainmv._labels, trainmv._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_TRUE(trainmv._raw_buffer);

    EXPECT_EQ(testmv._idxmagic, 2049U);
    EXPECT_EQ(testmv._nlabels, 10'000U);
    EXPECT_EQ(testmv._labels, testmv._raw_buffer + 8); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_TRUE(testmv._raw_buffer);

    EXPECT_FALSE(ltrain._idxmagic);
    EXPECT_FALSE(ltrain._nlabels);
    EXPECT_FALSE(ltrain._raw_buffer);
    EXPECT_FALSE(ltrain._labels);

    EXPECT_FALSE(ltest._idxmagic);
    EXPECT_FALSE(ltest._nlabels);
    EXPECT_FALSE(ltest._raw_buffer);
    EXPECT_FALSE(ltest._labels);
}

/*
 std::array<unsigned, std::numeric_limits<decltype(train_images)::value_type>::max() + 1> pixfreqs {};
    for (const auto& p : train_images) pixfreqs.at(p)++;
    assert(std::reduce(pixfreqs.cbegin(), pixfreqs.cend(), 0LLU) == 60'000 * 784);

    std::ranges::fill(pixfreqs, 0);
    for (const auto& p : test_images) pixfreqs.at(p)++;
    assert(std::reduce(pixfreqs.cbegin(), pixfreqs.cend(), 0LLU) == 10'000 * 784);

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

for (const auto& i : std::ranges::views::iota(0, 10))
    assert(std::count(train_labelsmvd.cbegin(), train_labelsmvd.cend(), i) == std::count(cpy_trlabs.cbegin(), cpy_trlabs.cend(), i));
for (const auto& i : std::ranges::views::iota(0, 10)) {
    assert(std::count(train_labelsmvd.cbegin(), train_labelsmvd.cend(), i) == 6'000);
    assert(std::count(test_labelsmvd.cbegin(), test_labelsmvd.cend(), i) == 1'000);
}
assert(std::reduce(train_labelsmvd.cbegin(), train_labelsmvd.cend(), 0LLU) == std::reduce(cpy_trlabs.cbegin(), cpy_trlabs.cend(), 0LLU));

// test the copy assignment operator
const auto* const ptr = train_labelsmvd._raw_buffer;
train_labelsmvd       = test_labelsmvd;     // train labels object has a bigger buffer than the test labels object
assert(train_labelsmvd._raw_buffer == ptr); // no reallocation should have happened!

// even though train_labelsmvd now has lesser elements than what could fill its buffer, it sould repurpose its iterators accordingly
for (const auto& i : std::ranges::views::iota(0, 10)) assert(std::count(train_labelsmvd.cbegin(), train_labelsmvd.cend(), i) == 1'000);

std::wcout << train_labelsmvd;
std::wcout << test_labelsmvd;

*/

TEST(IDX3, DEFAULT_CONSTRUCTOR) {
    const idxio::idx3 idx3 {};

    EXPECT_FALSE(idx3._idxmagic);
    EXPECT_FALSE(idx3._nimages);
    EXPECT_FALSE(idx3._nrows);
    EXPECT_FALSE(idx3._ncols);
    EXPECT_FALSE(idx3._raw_buffer);
    EXPECT_FALSE(idx3._pixels);
}

TEST(IDX3, CONSTRUCTOR) {
    EXPECT_EQ(itrain._idxmagic, 2051U);
    EXPECT_EQ(itrain._nimages, 60'000U);
    EXPECT_TRUE(itrain._raw_buffer);
    EXPECT_EQ(itrain._pixels, itrain._raw_buffer + 16); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_EQ(itrain._ncols, 28U);
    EXPECT_EQ(itrain._nrows, 28U);
    auto [rows, cols] = itrain.dim();
    EXPECT_EQ(rows, 28U);
    EXPECT_EQ(cols, 28U);

    EXPECT_EQ(itest._idxmagic, 2051U);
    EXPECT_EQ(itest._nimages, 10'000U);
    EXPECT_TRUE(itest._raw_buffer);
    EXPECT_EQ(itest._pixels, itest._raw_buffer + 16); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_EQ(itest._ncols, 28U);
    EXPECT_EQ(itest._nrows, 28U);
    std::tie(rows, cols) = itest.dim();
    EXPECT_EQ(rows, 28U);
    EXPECT_EQ(cols, 28U);
}

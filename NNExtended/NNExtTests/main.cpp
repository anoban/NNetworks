#include <gtest/gtest.h> // manually including gtest (1.15.2) because VS2022 Google Test Adaptor uses an archaic release of gtest (1.8.1.7)
#include <idxio.hpp>

// using fixture classes for idx objects entail repeated file io operations to construct same idx objects
// so opting to use globals :)
idxio::idx1 ltrain, ltest;
idxio::idx3 itrain, itest;

auto main() -> int {
    ltrain = idxio::idx1 { LR"(./../../Fashion-MNIST/train-labels-idx1-ubyte)" };
    ltest  = idxio::idx1 { LR"(./../../Fashion-MNIST/t10k-labels-idx1-ubyte)" };

    itrain = idxio::idx3 { LR"(./../../Fashion-MNIST/train-images-idx3-ubyte)" };
    itest  = idxio::idx3 { LR"(./../../Fashion-MNIST/t10k-images-idx3-ubyte)" };

    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
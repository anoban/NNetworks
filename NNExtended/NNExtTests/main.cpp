#include <gtest/gtest.h>

// using manually included gtest sources because VS2022 Google Test Adaptor uses an archaic release of Google test
auto main() -> int {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
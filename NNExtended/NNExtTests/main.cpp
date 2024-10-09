#include <gtest/gtest.h>

// manually including gtest (1.15.2) because VS2022 Google Test Adaptor uses an archaic release of gtest (1.8.1.7)
auto main() -> int {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
#include <gtest/gtest.h>
#include <libmath/sum.hpp>

TEST(sumTest, testSum) {
    EXPECT_DOUBLE_EQ(math::sum(16, 8.55), 24.55);
    EXPECT_DOUBLE_EQ(math::sum(256.74, -6), 250.74);
}
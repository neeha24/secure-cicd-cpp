#include <stdexcept>

#include <gtest/gtest.h>

#include "stats.h"

TEST(StatsTest, BasicValues) {
    const auto s = computeStats({1.0, 2.0, 3.0, 4.0});
    EXPECT_EQ(s.count, 4u);
    EXPECT_DOUBLE_EQ(s.min, 1.0);
    EXPECT_DOUBLE_EQ(s.max, 4.0);
    EXPECT_DOUBLE_EQ(s.mean, 2.5);
}

TEST(StatsTest, SingleValue) {
    const auto s = computeStats({42.0});
    EXPECT_DOUBLE_EQ(s.min, 42.0);
    EXPECT_DOUBLE_EQ(s.max, 42.0);
    EXPECT_DOUBLE_EQ(s.mean, 42.0);
}

TEST(StatsTest, NegativeValues) {
    const auto s = computeStats({-5.0, 5.0});
    EXPECT_DOUBLE_EQ(s.min, -5.0);
    EXPECT_DOUBLE_EQ(s.max, 5.0);
    EXPECT_DOUBLE_EQ(s.mean, 0.0);
}

TEST(StatsTest, EmptyInputThrows) {
    EXPECT_THROW(computeStats({}), std::invalid_argument);
}

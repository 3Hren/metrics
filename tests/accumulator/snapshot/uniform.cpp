#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include <metrics/accumulator/snapshot/uniform.hpp>

namespace metrics {
namespace testing {

using accumulator::snapshot::uniform_t;

TEST(uniform_t, small_quantiles) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(1.0, snapshot.value(0.1), 1e-3);
}

TEST(uniform_t, large_quantiles) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(5.0, snapshot.value(1.0), 1e-3);
}

TEST(uniform_t, throws_on_nan_quantile) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_THROW(snapshot.value(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
}

TEST(uniform_t, throws_on_negative_quantile) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_THROW(snapshot.value(-0.5), std::invalid_argument);
}

TEST(uniform_t, throws_on_overflow_quantile) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_THROW(snapshot.value(1.5), std::invalid_argument);
}

TEST(uniform_t, median) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(3, snapshot.median(), 1e-3);
}

TEST(uniform_t, p75) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(4.5, snapshot.p75(), 1e-3);
}

TEST(uniform_t, p90) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(5.0, snapshot.p90(), 1e-3);
}

TEST(uniform_t, p95) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(5.0, snapshot.p95(), 1e-3);
}

TEST(uniform_t, p98) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(5.0, snapshot.p98(), 1e-3);
}

TEST(uniform_t, p99) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(5.0, snapshot.p90(), 1e-3);
}

TEST(uniform_t, values) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_EQ(std::vector<std::uint64_t>({1, 2, 3, 4, 5}), snapshot.values());
}

TEST(uniform_t, size) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_EQ(5, snapshot.size());
}

TEST(uniform_t, min) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_EQ(1, snapshot.min());
}

TEST(uniform_t, max) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_EQ(5, snapshot.max());
}

TEST(uniform_t, mean) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_EQ(3.0, snapshot.mean());
}

TEST(uniform_t, stddev) {
    uniform_t snapshot({5, 1, 3, 2, 4});

    EXPECT_NEAR(1.5811, snapshot.stddev(), 1e-4);
}

TEST(uniform_t, min_on_empty) {
    uniform_t snapshot({});

    EXPECT_EQ(0, snapshot.min());
}

TEST(uniform_t, max_on_empty) {
    uniform_t snapshot({});

    EXPECT_EQ(0, snapshot.max());
}

TEST(uniform_t, mean_on_empty) {
    uniform_t snapshot({});

    EXPECT_NEAR(0.0, snapshot.mean(), 1e-6);
}

TEST(uniform_t, stddev_on_empty) {
    uniform_t snapshot({});

    EXPECT_NEAR(0.0, snapshot.stddev(), 1e-6);
}

TEST(uniform_t, stddev_on_singleton) {
    uniform_t snapshot({1});

    EXPECT_NEAR(0.0, snapshot.stddev(), 1e-6);
}

}  // namespace testing
}  // namespace metrics

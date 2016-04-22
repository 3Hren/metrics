#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

#include <metrics/accumulator/snapshot/weighted.hpp>

namespace metrics {
namespace accumulator {
namespace snapshot {
namespace {

const weighted_t SNAPSHOT({{5, 1}, {1, 2}, {2, 3}, {3, 2}, {4, 2}});

TEST(weighted_t, SmallQuantiles) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(1.0, snapshot.value(0.0), 1e-3);
}

TEST(weighted_t, BigQuantiles) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(5.0, snapshot.value(1.0), 1e-3);
}

TEST(weighted_t, ThrowsOnNaNQuantile) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_THROW(snapshot.value(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
}

TEST(weighted_t, ThrowsOnNegativeQuantile) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_THROW(snapshot.value(-0.5), std::invalid_argument);
}

TEST(weighted_t, ThrowsOnOverflowQuantile) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_THROW(snapshot.value(1.5), std::invalid_argument);
}

TEST(weighted_t, Median) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(3.0, snapshot.median(), 1e-3);
}

TEST(weighted_t, p75) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(4, snapshot.p75(), 1e-3);
}

TEST(weighted_t, p95) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(5, snapshot.p95(), 1e-3);
}

TEST(weighted_t, p98) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(5, snapshot.p98(), 1e-3);
}

TEST(weighted_t, p99) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(5, snapshot.p99(), 1e-3);
}

TEST(weighted_t, values) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_EQ(std::vector<std::uint64_t>({1, 2, 3, 4, 5}), snapshot.values());
}

TEST(weighted_t, size) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_EQ(5, snapshot.size());
}

TEST(weighted_t, min) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_EQ(1, snapshot.min());
}

TEST(weighted_t, max) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_EQ(5, snapshot.max());
}

TEST(weighted_t, mean) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(2.7, snapshot.mean(), 1e-3);
}

TEST(weighted_t, stddev) {
    const weighted_t snapshot = SNAPSHOT;

    EXPECT_NEAR(1.2688, snapshot.stddev(), 1e-4);
}

TEST(weighted_t, MinOnEmpty) {
    const weighted_t snapshot({});

    EXPECT_EQ(0, snapshot.min());
}

TEST(weighted_t, MaxOnEmpty) {
    const weighted_t snapshot({});

    EXPECT_EQ(0, snapshot.max());
}

TEST(weighted_t, MeanOnEmpty) {
    const weighted_t snapshot({});

    EXPECT_NEAR(0.0, snapshot.mean(), 1e-6);
}

TEST(weighted_t, StddevOnEmpty) {
    const weighted_t snapshot({});

    EXPECT_NEAR(0.0, snapshot.stddev(), 1e-6);
}

TEST(weighted_t, StddevOnSingleton) {
    const weighted_t snapshot({{1, 1.0}});

    EXPECT_NEAR(0.0, snapshot.stddev(), 1e-6);
}

TEST(weighted_t, NoOverflowOnLowWeights) {
    const weighted_t snapshot({{1, 1e-10}, {2, 1e-10}, {3, 1e-10}});

    EXPECT_NEAR(2.0, snapshot.mean(), 1e-3);
}

}  // namespace
}  // namespace snapshot
}  // namespace accumulator
}  // namespace metrics

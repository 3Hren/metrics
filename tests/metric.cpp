#include <gtest/gtest.h>

#include <metrics/metric/counter.hpp>
#include <metrics/metric/gauge.hpp>
#include <metrics/metric/meter.hpp>
#include <metrics/metric/registry.hpp>

namespace metrics {
namespace testing {

using metric::registry_t;

TEST(metric, gauge) {
    registry_t registry;

    registry.listen<metric::gauge<std::uint64_t>>("metrics.testing.gauge", [&]() -> std::uint64_t {
        static std::uint64_t counter = 0;
        return ++counter;
    });

    auto gauge = registry.gauge<std::uint64_t>("metrics.testing.gauge");

    EXPECT_EQ(1, gauge.get());
    EXPECT_EQ(2, gauge.get());
}

TEST(metric, throws_if_gauge_not_registered) {
    registry_t registry;

    auto gauge = registry.gauge<std::uint64_t>("metrics.testing.gauge");

    EXPECT_THROW(gauge.get(), std::out_of_range);
}

TEST(metric, counter) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    // Default value is 0.
    EXPECT_EQ(0, counter.get());
}

TEST(metric, counter_inc) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter.inc();
    EXPECT_EQ(1, counter.get());

    EXPECT_EQ(1, counter.inc().get());
    EXPECT_EQ(2, counter.get());

    EXPECT_EQ(2, counter.inc().get());
    EXPECT_EQ(3, counter.get());
}

TEST(metric, counter_inc_val) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter.inc(42);
    EXPECT_EQ(42, counter.get());

    EXPECT_EQ(42, counter.inc(10).get());
    EXPECT_EQ(52, counter.get());

    EXPECT_EQ(52, counter.inc(8).get());
    EXPECT_EQ(60, counter.get());
}

TEST(metric, counter_shared) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter.inc();
    EXPECT_EQ(1, counter.get());

    EXPECT_EQ(1, counter.inc().get());
    EXPECT_EQ(2, counter.get());

    auto other = registry.counter<std::uint64_t>("metrics.testing.counter");

    EXPECT_EQ(2, other.inc().get());
    EXPECT_EQ(3, other.get());
}

TEST(metric, counter_dec) {
    registry_t registry;

    auto counter = registry.counter<std::int64_t>("metrics.testing.counter");

    counter.dec();
    EXPECT_EQ(-1, counter.get());

    EXPECT_EQ(-1, counter.dec().get());
    EXPECT_EQ(-2, counter.get());

    EXPECT_EQ(-2, counter.dec().get());
    EXPECT_EQ(-3, counter.get());
}

TEST(metric, counter_dec_val) {
    registry_t registry;

    auto counter = registry.counter<std::int64_t>("metrics.testing.counter");

    // Note, that we decrease by positive value, which results in substraction.
    counter.dec(42);
    EXPECT_EQ(-42, counter.get());

    EXPECT_EQ(-42, counter.dec(10).get());
    EXPECT_EQ(-52, counter.get());

    EXPECT_EQ(-52, counter.dec(8).get());
    EXPECT_EQ(-60, counter.get());
}

TEST(metric, meter) {
    registry_t registry;

    auto requests = registry.meter("metrics.testing.requests");

    EXPECT_EQ(0, requests.count());
}

TEST(metric, meter_mark) {
    registry_t registry;

    auto requests = registry.meter("metrics.testing.requests");

    requests.mark();
    EXPECT_EQ(1, requests.count());
}

}  // namespace testing
}  // namespace metrics

#include <gtest/gtest.h>

#include <stdexcept>

#include <metrics/gauge.hpp>
#include <metrics/registry.hpp>

namespace metrics {
namespace testing {

TEST(Gauge, Factory) {
    registry_t registry;

    registry.listen<gauge<std::uint64_t>>("metrics.testing.gauge", [&]() -> std::uint64_t {
        static std::uint64_t counter = 0;
        return ++counter;
    });

    auto gauge = registry.gauge<std::uint64_t>("metrics.testing.gauge");

    EXPECT_EQ(1, gauge.get());
    EXPECT_EQ(2, gauge.get());
}

TEST(Gauge, ThrowsIfNotRegistered) {
    registry_t registry;

    auto gauge = registry.gauge<std::uint64_t>("metrics.testing.gauge");

    EXPECT_THROW(gauge.get(), std::out_of_range);
}

TEST(Gauge, Tagged) {
    registry_t registry;

    registry.listen<gauge<std::uint64_t>>("queue.age", {{"service", "node"}}, [&]() -> std::uint64_t {
        static std::uint64_t counter = 0;
        return ++counter;
    });

    auto gauge = registry.gauge<std::uint64_t>("queue.age", {{"service", "node"}});

    EXPECT_EQ(1, gauge.get());
    EXPECT_EQ(2, gauge.get());
}

}  // namespace testing
}  // namespace metrics

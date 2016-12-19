#include <gtest/gtest.h>

#include <metrics/gauge.hpp>
#include <metrics/meter.hpp>
#include <metrics/registry.hpp>

namespace metrics {
namespace testing {

TEST(Metric, Factory) {
    registry_t registry;

    auto requests = registry.meter("metrics.testing.requests");

    EXPECT_EQ(0, requests->count());
}

TEST(Meter, Mark) {
    registry_t registry;

    auto requests = registry.meter("metrics.testing.requests");

    requests->mark();
    EXPECT_EQ(1, requests->count());
}

TEST(Meter, Tagged) {
    registry_t registry;

    auto requests1 = registry.meter("metrics.testing.requests", {
        {"tag", "#1"}
    });

    requests1->mark();

    auto requests2 = registry.meter("metrics.testing.requests");
    EXPECT_EQ(0, requests2->count());
}

}  // namespace testing
}  // namespace metrics

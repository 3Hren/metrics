#include <gtest/gtest.h>

#include <metrics/counter.hpp>
#include <metrics/gauge.hpp>
#include <metrics/meter.hpp>
#include <metrics/registry.hpp>

namespace metrics {
namespace testing {

TEST(Metric, Factory) {
    registry_t registry;

    auto requests = registry.meter("metrics.testing.requests");

    EXPECT_EQ(0, requests.count());
}

TEST(Meter, Mark) {
    registry_t registry;

    auto requests = registry.meter("metrics.testing.requests");

    requests.mark();
    EXPECT_EQ(1, requests.count());
}

}  // namespace testing
}  // namespace metrics

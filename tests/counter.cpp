#include <gtest/gtest.h>

#include <metrics/counter.hpp>
#include <metrics/registry.hpp>

namespace metrics {
namespace testing {

TEST(counter, CopyAssignable) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");
    counter = registry.counter<std::uint64_t>("metrics.testing.other_counter");

    // Default value is 0.
    EXPECT_EQ(0, counter.get());
}

}  // namespace testing
}  // namespace metrics

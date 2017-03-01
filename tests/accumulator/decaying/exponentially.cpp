#include <gtest/gtest.h>

#include <metrics/accumulator/decaying/exponentially.hpp>

namespace metrics {
namespace accumulator {
namespace decaying {
namespace {

TEST(exponentially_t, Accumulate100OutOf1000Elements) {
    exponentially_t accumulator(100, 0.99);

    for (int i = 0; i < 1000; ++i) {
        accumulator.update(i);
    }

    EXPECT_EQ(100, accumulator.size());

    const auto snapshot = accumulator.snapshot();
    EXPECT_EQ(100, snapshot.size());

    for (auto value : snapshot.values()) {
        ASSERT_TRUE(value <= 1000);
    }
}

}  // namespace
}  // namespace decaying
}  // namespace accumulator
}  // namespace metrics

#include <gtest/gtest.h>

#include <metrics/accumulator/sliding/window.hpp>
#include <metrics/registry.hpp>
#include <metrics/timer.hpp>

namespace metrics {
namespace testing {

TEST(timer_t, Obtain) {
    registry_t registry;

    registry.timer("metrics.testing.timer");
}

TEST(timer_t, TimerContextNoncopyableButMovable) {
    typedef timer<accumulator::sliding::window_t>::context_type context_type;

    EXPECT_TRUE(std::is_move_constructible<context_type>::value);
    EXPECT_FALSE(std::is_move_assignable<context_type>::value);
    EXPECT_FALSE(std::is_copy_constructible<context_type>::value);
    EXPECT_FALSE(std::is_copy_assignable<context_type>::value);
}

TEST(timer_t, Count) {
    registry_t registry;

    const auto timer = registry.timer("#");

    // Default value is 0.
    EXPECT_EQ(0, timer.count());
}

}  // namespace testing
}  // namespace metrics

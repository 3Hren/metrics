#include <gtest/gtest.h>

#include <metrics/accumulator/sliding/window.hpp>
#include <metrics/registry.hpp>
#include <metrics/timer.hpp>

#ifndef __GNUC_VERSION__
    #if defined(__GNUC__)
        #define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
    #endif
#endif

namespace metrics {
namespace testing {

TEST(timer_t, Obtain) {
    registry_t registry;

    registry.timer("metrics.testing.timer");
}

#if __GNUC_VERSION__ > 40600
TEST(timer_t, TimerContextNoncopyableButMovable) {
    typedef timer<accumulator::sliding::window_t>::context_type context_type;

    EXPECT_TRUE(std::is_move_constructible<context_type>::value);
    EXPECT_FALSE(std::is_move_assignable<context_type>::value);
    EXPECT_FALSE(std::is_copy_constructible<context_type>::value);
    EXPECT_FALSE(std::is_copy_assignable<context_type>::value);
}
#endif

TEST(timer_t, Count) {
    registry_t registry;

    const auto timer = registry.timer("#");

    // Default value is 0.
    EXPECT_EQ(0, timer.count());
}

TEST(timer_t, WithContext) {
    registry_t registry;

    auto timer = registry.timer("#");

    {
        const auto context = timer.context();
        // Do the work... bang!
    }

    EXPECT_EQ(1, timer.count());
}

TEST(timer_t, WithContextMoved) {
    registry_t registry;

    auto timer = registry.timer("#");

    {
        auto context1 = timer.context();
        auto context2 = std::move(context1);
    }

    EXPECT_EQ(1, timer.count());
}

TEST(timer_t, WithMeasure) {
    registry_t registry;

    auto timer = registry.timer("#");

    const auto actual = timer.measure([]() -> int {
        return 42;
    });

    EXPECT_EQ(1, timer.count());
    EXPECT_EQ(42, actual);
}

TEST(timer_t, Snapshot) {
    registry_t registry;

    const auto timer = registry.timer("#");

    EXPECT_EQ(0, timer.snapshot().size());
}

TEST(timer_t, MeterGetters) {
    registry_t registry;

    const auto timer = registry.timer("#");

    EXPECT_DOUBLE_EQ(0.0, timer.m01rate());
    EXPECT_DOUBLE_EQ(0.0, timer.m05rate());
    EXPECT_DOUBLE_EQ(0.0, timer.m15rate());
}

TEST(timer_t, Tagged) {
    registry_t registry;

    auto timer1 = registry.timer("#", {{"tag", "#1"}});

    {
        const auto context = timer1.context();
        // Do the work... bang!
    }

    auto timer2 = registry.timer("#");

    EXPECT_EQ(0, timer2.count());
}

}  // namespace testing
}  // namespace metrics

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

#include <metrics/accumulator/snapshot/uniform.hpp>

#include <metrics/detail/timer.hpp>

namespace metrics {
namespace testing {
namespace {
namespace mock {

struct clock_t {
    constexpr static bool is_steady = true;

    typedef std::chrono::high_resolution_clock::duration duration;
    typedef std::chrono::high_resolution_clock::time_point time_point;

    MOCK_CONST_METHOD0(now, time_point());
};

struct meter_t {
    MOCK_CONST_METHOD0(mark, void());
};

struct histogram_t {
    MOCK_CONST_METHOD0(count, std::uint64_t());
    MOCK_CONST_METHOD1(update, void(std::uint64_t));
};

} // namespace mock
} // namespace
} // namespace testing
} // namespace metrics

namespace metrics {
namespace testing {

using ::testing::Return;

typedef detail::timer<mock::clock_t, mock::meter_t, mock::histogram_t> timer_type;

TEST(Timer, Constructor) {
    timer_type timer;

    EXPECT_CALL(timer.histogram(), count())
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_EQ(0, timer.count());
}

TEST(Timer, UpdatesCountOnUpdate) {
    timer_type timer;

    EXPECT_CALL(timer.histogram(), update(1e9))
        .Times(1);

    EXPECT_CALL(timer.meter(), mark())
        .Times(1);

    timer.update(std::chrono::seconds(1));

    EXPECT_CALL(timer.histogram(), count())
        .Times(1)
        .WillOnce(Return(1));

    EXPECT_EQ(1, timer.count());
}

TEST(Timer, MeasureCallable) {
    timer_type timer;

    EXPECT_CALL(timer.clock(), now())
        .Times(2)
        .WillOnce(Return(mock::clock_t::time_point()))
        .WillRepeatedly(Return(mock::clock_t::time_point(std::chrono::milliseconds(50))));

    EXPECT_CALL(timer.histogram(), update(50000000))
        .Times(1);

    EXPECT_CALL(timer.meter(), mark())
        .Times(1);

    const auto actual = timer.measure([]() -> int {
        return 42;
    });

    EXPECT_EQ(42, actual);
}

TEST(Timer, MeasureCallableOnThrow) {
    timer_type timer;

    EXPECT_CALL(timer.clock(), now())
        .Times(2)
        .WillOnce(Return(mock::clock_t::time_point()))
        .WillRepeatedly(Return(mock::clock_t::time_point(std::chrono::milliseconds(50))));

    EXPECT_CALL(timer.histogram(), update(50000000))
        .Times(1);

    EXPECT_CALL(timer.meter(), mark())
        .Times(1);

    EXPECT_THROW(timer.measure([]() -> int {
        throw std::runtime_error("expected");
    }), std::runtime_error);
}

}  // namespace testing
}  // namespace metrics

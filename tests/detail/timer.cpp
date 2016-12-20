#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

#include <metrics/accumulator/snapshot/uniform.hpp>

#include <src/timer.hpp>

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

    MOCK_CONST_METHOD0(m01rate, double());
    MOCK_CONST_METHOD0(m05rate, double());
    MOCK_CONST_METHOD0(m15rate, double());
};

struct histogram_t {
    struct accumulator_t {
        typedef accumulator::snapshot::uniform_t snapshot_type;

        MOCK_CONST_METHOD0(snapshot, snapshot_type());
        MOCK_CONST_METHOD1(update, void(std::uint64_t));
    };

    typedef accumulator_t accumulator_type;
    typedef accumulator_type::snapshot_type snapshot_type;

    MOCK_CONST_METHOD0(count, std::uint64_t());
    MOCK_CONST_METHOD1(update, void(std::uint64_t));
    MOCK_CONST_METHOD0(snapshot, snapshot_type());
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

TEST(Timer, DelegatesRateCalls) {
    timer_type timer;

    EXPECT_CALL(timer.meter(), m01rate())
        .Times(1)
        .WillOnce(Return(1.0));

    EXPECT_DOUBLE_EQ(1.0, timer.m01rate());

    EXPECT_CALL(timer.meter(), m05rate())
        .Times(1)
        .WillOnce(Return(0.5));

    EXPECT_DOUBLE_EQ(0.5, timer.m05rate());

    EXPECT_CALL(timer.meter(), m15rate())
        .Times(1)
        .WillOnce(Return(0.1));

    EXPECT_DOUBLE_EQ(0.1, timer.m15rate());
}

}  // namespace testing
}  // namespace metrics

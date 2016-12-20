#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>

#include <src/meter.hpp>

namespace metrics {
namespace testing {
namespace {
namespace mock {

using ::testing::Return;

struct clock_t {
    typedef std::chrono::high_resolution_clock::duration duration;
    typedef std::chrono::high_resolution_clock::time_point time_point;

    MOCK_CONST_METHOD0(now, time_point());

    clock_t() {
        ON_CALL(*this, now())
            .WillByDefault(Return(time_point(std::chrono::seconds(0))));
    }
};

} // namespace mock
} // namespace
} // namespace testing
} // namespace metrics

namespace metrics {
namespace testing {

using ::testing::NiceMock;
using ::testing::Return;

typedef detail::meter<NiceMock<mock::clock_t>> meter_type;

/// Starts with no rates or counts by default.
TEST(meter, init) {
    meter_type meter;

    EXPECT_EQ(0, meter.count());

    EXPECT_FLOAT_EQ(0.0, meter.mean_rate());

    EXPECT_FLOAT_EQ(0.0, meter.m01rate());
    EXPECT_FLOAT_EQ(0.0, meter.m05rate());
    EXPECT_FLOAT_EQ(0.0, meter.m15rate());
}

TEST(meter, mark) {
    meter_type meter;

    EXPECT_CALL(meter.clock(), now())
        .Times(6)
        .WillOnce(Return(mock::clock_t::time_point()))
        .WillRepeatedly(Return(mock::clock_t::time_point(std::chrono::seconds(10))));

    meter.mark();
    meter.mark(2);

    EXPECT_EQ(3, meter.count());

    EXPECT_FLOAT_EQ(0.3, meter.mean_rate());

    EXPECT_NEAR(0.1840, meter.m01rate(), 1e-3);
    EXPECT_NEAR(0.1966, meter.m05rate(), 1e-3);
    EXPECT_NEAR(0.1988, meter.m15rate(), 1e-3);
}

}  // namespace testing
}  // namespace metrics

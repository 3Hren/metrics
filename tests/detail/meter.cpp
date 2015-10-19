#include <gtest/gtest.h>

#include <metrics/detail/meter.hpp>

namespace metrics {
namespace testing {
namespace {
namespace mock {

struct clock_t {
    typedef std::chrono::high_resolution_clock::time_point time_point;

    mutable int counter;

    clock_t(): counter(0) {}

    time_point now() const {
        // For first two calls (one for ctor, the second for tick).
        if (counter++ < 2) {
            return time_point(std::chrono::seconds(0));
        }

        return time_point(std::chrono::seconds(10));
    }
};

} // namespace mock
} // namespace
} // namespace testing
} // namespace metrics

namespace metrics {
namespace testing {

typedef detail::meter<mock::clock_t> meter_type;

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

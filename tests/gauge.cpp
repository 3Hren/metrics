#include <gtest/gtest.h>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/optional/optional.hpp>

#include <metrics/gauge.hpp>
#include <metrics/registry.hpp>

namespace metrics {
namespace testing {
namespace {

TEST(Gauge, Factory) {
    typedef boost::accumulators::accumulator_set<
        double,
        boost::accumulators::features<boost::accumulators::tag::rolling_mean>
    > rolling_mean_t;

    rolling_mean_t mean(boost::accumulators::rolling_window_size = 5);
    registry_t registry;
    registry.register_gauge<double>("metrics.testing.gauge", {}, [&]() -> double {
        return boost::accumulators::rolling_mean(mean);
    });

    auto gauge = registry.gauge<double>("metrics.testing.gauge");

    mean(0.0);
    mean(1.0);
    mean(1.0);
    mean(0.0);
    mean(1.0);
    EXPECT_FLOAT_EQ(0.6, (*gauge.get())());
    mean(2.0);
    mean(1.0);
    mean(0.0);
    EXPECT_FLOAT_EQ(0.8, (*gauge.get())());
}

}  // namespace
}  // namespace testing
}  // namespace metrics

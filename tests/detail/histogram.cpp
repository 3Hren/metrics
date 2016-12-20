#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <metrics/accumulator/snapshot/uniform.hpp>

#include <src/histogram.hpp>

namespace metrics {
namespace testing {

using ::testing::Return;

namespace mock {

struct accumulator_t {
    typedef accumulator::snapshot::uniform_t snapshot_type;

    MOCK_CONST_METHOD0(snapshot, snapshot_type());
    MOCK_CONST_METHOD1(update, void(std::uint64_t));
};

} // namespace mock

typedef detail::histogram<mock::accumulator_t> histogram_type;

TEST(Histogram, Constructor) {
    histogram_type histogram;

    EXPECT_EQ(0, histogram.count());
}

TEST(Histogram, UpdatesCountOnUpdates) {
    histogram_type histogram;

    EXPECT_CALL(histogram.accumulator(), update(1))
        .Times(1);

    histogram.update(1);

    EXPECT_EQ(1, histogram.count());
}

TEST(Histogram, DelegatesSnapshot) {
    histogram_type histogram;
    accumulator::snapshot::uniform_t snapshot({1, 2, 3});

    EXPECT_CALL(histogram.accumulator(), snapshot())
        .Times(1)
        .WillOnce(Return(snapshot));

    EXPECT_EQ(snapshot.values(), histogram.snapshot().values());
}

}  // namespace testing
}  // namespace metrics

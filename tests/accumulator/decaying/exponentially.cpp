#include <chrono>
#include <thread>
#include <random>

#include <cmath>

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

    constexpr auto EPSILON = double{150};

    EXPECT_NEAR(snapshot.mean(), 500, EPSILON);
    EXPECT_NEAR(snapshot.stddev(), 288, EPSILON);

    EXPECT_NEAR(snapshot.min(), 0, EPSILON);
    EXPECT_NEAR(snapshot.max(), 1000, EPSILON);
}

TEST(exponentially_t, rescale) {
    exponentially_t accumulator(100, 0.05, std::chrono::milliseconds(10));

    for (int i = 0; i < 1000; ++i) {
        if (i % 100 == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        accumulator.update(i);
    }

    constexpr auto EPSILON = double{150};

    const auto snapshot = accumulator.snapshot();

    EXPECT_NEAR(snapshot.mean(), 500, EPSILON);
    EXPECT_NEAR(snapshot.stddev(), 288, EPSILON);

    EXPECT_NEAR(snapshot.min(), 0, EPSILON);
    EXPECT_NEAR(snapshot.max(), 1000, EPSILON);
}

struct marginal_case_t {
    double size;
    double alpha;

    double mean;
    double stddev;

    double epsilon1;
};

class margins_test : public ::testing::TestWithParam<marginal_case_t> {};

TEST_P(margins_test, marginal_cases_test) {

    const auto &test = GetParam();

    std::random_device rd;
    std::mt19937 gen{rd()};
    std::normal_distribution<> norm{test.mean, test.stddev};

    exponentially_t accumulator(test.size, test.alpha, std::chrono::milliseconds{1});

    for(int i = 0; i < 1000; ++i) {
        accumulator(abs(norm(gen)) % 1000);
    }

    const auto snapshot = accumulator.snapshot();

    EXPECT_NEAR(snapshot.mean(), 500, test.epsilon1);
    EXPECT_NEAR(snapshot.stddev(), 288, test.epsilon1);

    EXPECT_NEAR(snapshot.min(), 0, test.epsilon1);
    EXPECT_NEAR(snapshot.max(), 1000, test.epsilon1);
}

INSTANTIATE_TEST_CASE_P(exponentially_t, margins_test,
    ::testing::Values(
        //              SIZE, ALPHA, AVG, SDEV, EPS1
        marginal_case_t{1000, 0.001, 500, 288, 150},
        marginal_case_t{1000, 13.10, 500, 288, 150},
        marginal_case_t{ 100, 0.001, 500, 288, 200},
        marginal_case_t{ 100, 13.10, 500, 288, 200},
        marginal_case_t{  10, 0.001, 500, 288, 700},
        marginal_case_t{  10, 13.10, 500, 288, 700}
    ));

}  // namespace
}  // namespace decaying
}  // namespace accumulator
}  // namespace metrics

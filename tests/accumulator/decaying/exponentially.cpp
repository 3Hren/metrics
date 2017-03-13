#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <stdexcept>

#include <cmath>

#include <gtest/gtest.h>

#include <metrics/accumulator/decaying/exponentially.hpp>

namespace metrics {
namespace accumulator {
namespace decaying {

constexpr auto RANDOM_SEED = 100500;
constexpr auto EPSILON = 150.0;

template<typename Snapshot>
void dump_snapshot(std::ostream &o, const Snapshot &s) {
    o << "mean: " << s.mean() << ' '
      << "std: " << s.stddev() << ' '
      << "min: " << s.min() << ' '
      << "max: " << s.max() << '\n';
}

TEST(exponentially_t, Accumulate100OutOf1000Elements) {
    exponentially_t accumulator(100, 0.99, std::chrono::milliseconds(10), RANDOM_SEED);

    for (int i = 0; i < 1000; ++i) {
        accumulator.update(i);
    }

    EXPECT_EQ(100, accumulator.size());

    const auto snapshot = accumulator.snapshot();
    EXPECT_EQ(100, snapshot.size());

    for (auto value : snapshot.values()) {
        ASSERT_TRUE(value <= 1000);
    }

    EXPECT_NEAR(snapshot.mean(), 500, EPSILON);
    EXPECT_NEAR(snapshot.stddev(), 288, EPSILON);

    EXPECT_NEAR(snapshot.min(), 0, EPSILON);
    EXPECT_NEAR(snapshot.max(), 1000, EPSILON);
}

TEST(exponentially_t, empty) {
    exponentially_t acc(1, 1e-10, std::chrono::milliseconds(1), RANDOM_SEED);

    const auto snapshot = acc.snapshot();

    EXPECT_FLOAT_EQ(snapshot.mean(), 0);
    EXPECT_FLOAT_EQ(snapshot.stddev(), 0);

    EXPECT_FLOAT_EQ(snapshot.min(), 0);
    EXPECT_FLOAT_EQ(snapshot.max(), 0);
}

TEST(exponentially, const_fill_by_10k_vals) {
    exponentially_t acc(10, 1e-10, std::chrono::milliseconds(100), RANDOM_SEED);

    constexpr int FILL_VALUE = 100500;

    for(int i = 0; i < 10 * 1000; ++i) {
        acc(FILL_VALUE);
    }

    const auto snapshot = acc.snapshot();

    EXPECT_FLOAT_EQ(snapshot.mean(), FILL_VALUE);
    EXPECT_FLOAT_EQ(snapshot.stddev(), 0);
    EXPECT_FLOAT_EQ(snapshot.min(), FILL_VALUE);
    EXPECT_FLOAT_EQ(snapshot.max(), FILL_VALUE);
}

TEST(exponentially_t, wrong_ctor_args) {
    EXPECT_THROW(exponentially_t(1, -1), std::invalid_argument);
    EXPECT_THROW(exponentially_t(1, 0), std::invalid_argument);
    EXPECT_THROW(exponentially_t(0, 1), std::invalid_argument);
    EXPECT_THROW(exponentially_t(13, 10, std::chrono::hours(0)), std::invalid_argument);

    EXPECT_NO_THROW(exponentially_t(13, 10, std::chrono::hours(1)));
}

TEST(exponentially_t, rescale) {
    exponentially_t accumulator(100, 0.05, std::chrono::milliseconds(10), RANDOM_SEED);

    for (int i = 0; i < 1000; ++i) {
        if (i % 100 == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        accumulator.update(i);
    }

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

    double expect_mean;
    double expect_std;

    double expect_min;
    double expect_max;

    double epsilon1;
};

class margins_test : public ::testing::TestWithParam<marginal_case_t> {};

TEST_P(margins_test, marginal_cases_test) {

    const auto &test = GetParam();

    std::mt19937 gen; // defaults to seed = 5489u
    std::normal_distribution<> norm{test.mean, test.stddev};

    exponentially_t accumulator(test.size, test.alpha, std::chrono::milliseconds{5}, RANDOM_SEED);

    for(int i = 0; i < 1000; ++i) {
        accumulator(abs(norm(gen)) % 1000);
    }

    const auto snapshot = accumulator.snapshot();

    EXPECT_NEAR(snapshot.mean(), test.expect_mean, test.epsilon1);
    EXPECT_NEAR(snapshot.stddev(), test.expect_std, test.epsilon1);

    EXPECT_NEAR(snapshot.min(), test.expect_min, test.epsilon1);
    EXPECT_NEAR(snapshot.max(), test.expect_max, test.epsilon1);
}

INSTANTIATE_TEST_CASE_P(exponentially_t, margins_test,
    ::testing::Values(
        //              SIZE,    ALPHA, AVG, STD, EXP_AVG, EXP_STD, MIN, MAX, EPS1
        marginal_case_t{1000,    0.001, 500, 288, 464.308, 248.085,   1,  994, 1e-3},
        marginal_case_t{1000,    13.10, 500, 288, 464.308, 248.085,   1,  994, 1e-3},

        marginal_case_t{ 100,   0.0001, 500, 288,  440.94, 237.772,   8,  942, 1e-3},
        marginal_case_t{ 100,    0.001, 500, 288,  440.94, 237.772,   8,  942, 1e-3},
        marginal_case_t{ 100,    13.10, 500, 288,  440.94, 237.772,   8,  942, 1e-3},
        marginal_case_t{ 100, 100500.0, 500, 288,  440.94, 237.772,   8,  942, 1e-3},

        marginal_case_t{  10,    0.001, 500, 288,   611.7, 255.597,  53,  942, 1e-3},
        marginal_case_t{  10,    13.10, 500, 288,   611.7, 255.597,  53,  942, 1e-3}
    ));

}  // namespace decaying
}  // namespace accumulator
}  // namespace metrics

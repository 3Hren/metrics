#include <gtest/gtest.h>

#include <metrics/accumulator/sliding/window.hpp>

namespace metrics {
namespace testing {

using metric::accumulator::sliding::window_t;

TEST(window_t, small) {
    window_t acc(3);

    acc(1);
    acc(2);
    EXPECT_EQ(std::vector<std::uint64_t>({1, 2}), acc.snapshot().values());

    acc(3);
    EXPECT_EQ(std::vector<std::uint64_t>({1, 2, 3}), acc.snapshot().values());
}

TEST(window_t, keep_most_recent) {
    window_t acc(3);

    acc(1);
    acc(2);
    acc(3);
    acc(4);
    EXPECT_EQ(std::vector<std::uint64_t>({2, 3, 4}), acc.snapshot().values());
}

}  // namespace testing
}  // namespace metrics

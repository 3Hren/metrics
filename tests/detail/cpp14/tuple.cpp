#include <gtest/gtest.h>

#include <src/cpp14/tuple.hpp>

namespace metrics {
namespace testing {

TEST(tuple, count) {
    constexpr auto v1 = cpp14::count<int>::value;
    EXPECT_EQ(0, v1);

    constexpr auto v2 = cpp14::count<int, int>::value;
    EXPECT_EQ(1, v2);

    constexpr auto v3 = cpp14::count<int, int, double>::value;
    EXPECT_EQ(1, v3);

    constexpr auto v4 = cpp14::count<int, int, double, int>::value;
    EXPECT_EQ(2, v4);

    constexpr auto v5 = cpp14::count<int, int, double, int, int>::value;
    EXPECT_EQ(3, v5);
}

TEST(tuple, IsUnique) {
    constexpr auto v1 = cpp14::is_unique<int, int>::value;
    EXPECT_TRUE(v1);

    constexpr auto v2 = cpp14::is_unique<int, int, double>::value;
    EXPECT_TRUE(v2);

    constexpr auto v3 = cpp14::is_unique<int, int, double, int>::value;
    EXPECT_FALSE(v3);

    constexpr auto v4 = cpp14::is_unique<int, int, double, int, int>::value;
    EXPECT_FALSE(v4);
}

}  // namespace testing
}  // namespace metrics

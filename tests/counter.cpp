#include <gtest/gtest.h>

#include <boost/optional/optional.hpp>

#include <metrics/registry.hpp>

namespace metrics {
namespace testing {

TEST(Counter, Factory) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    // Default value is 0.
    EXPECT_EQ(0, counter->load());
}

TEST(Counter, Inc) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter->fetch_add(1);
    EXPECT_EQ(1, counter->load());

    EXPECT_EQ(1, counter->fetch_add(1));
    EXPECT_EQ(2, counter->load());

    EXPECT_EQ(2, counter->fetch_add(1));
    EXPECT_EQ(3, counter->load());
}

TEST(Counter, IncVal) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter->fetch_add(42);
    EXPECT_EQ(42, counter->load());

    EXPECT_EQ(42, counter->fetch_add(10));
    EXPECT_EQ(52, counter->load());

    EXPECT_EQ(52, counter->fetch_add(8));
    EXPECT_EQ(60, counter->load());
}

TEST(Counter, Shared) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter->fetch_add(1);
    EXPECT_EQ(1, counter->load());

    EXPECT_EQ(1, counter->fetch_add(1));
    EXPECT_EQ(2, counter->load());

    auto other = registry.counter<std::uint64_t>("metrics.testing.counter");

    EXPECT_EQ(2, other->fetch_add(1));
    EXPECT_EQ(3, other->load());
}

TEST(Counter, Dec) {
    registry_t registry;

    auto counter = registry.counter<std::int64_t>("metrics.testing.counter");

    counter->fetch_add(-1);
    EXPECT_EQ(-1, counter->load());

    EXPECT_EQ(-1, counter->fetch_add(-1));
    EXPECT_EQ(-2, counter->load());

    EXPECT_EQ(-2, counter->fetch_add(-1));
    EXPECT_EQ(-3, counter->load());
}

TEST(metric, counter_dec_val) {
    registry_t registry;

    auto counter = registry.counter<std::int64_t>("metrics.testing.counter");

    // Note, that we decrease by positive value, which results in substraction.
    counter->fetch_add(-42);
    EXPECT_EQ(-42, counter->load());

    EXPECT_EQ(-42, counter->fetch_add(-10));
    EXPECT_EQ(-52, counter->load());

    EXPECT_EQ(-52, counter->fetch_add(-8));
    EXPECT_EQ(-60, counter->load());
}

TEST(Counter, CopyAssignable) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");
    counter = registry.counter<std::uint64_t>("metrics.testing.other_counter");

    // Default value is 0.
    EXPECT_EQ(0, counter->load());
}

TEST(Counter, Name) {
    const registry_t registry;
    const auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");
    EXPECT_EQ("metrics.testing.counter", counter.name());
}

TEST(Counter, TaggedName) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("connections.accepted", {
        {"service", "storage"},
        {"scope",   "testing"}
    });

    EXPECT_EQ("storage", *counter.tag("service"));
    EXPECT_EQ("testing", *counter.tag("scope"));

    EXPECT_FALSE(!!counter.tag("missing"));
}

TEST(Counter, DifferentByTagsInc) {
    registry_t registry;

    auto counter1 = registry.counter<std::uint64_t>("metrics.testing.counter", {
        {"tag", "#1"}
    });

    counter1->fetch_add(1);

    auto counter2 = registry.counter<std::uint64_t>("metrics.testing.counter", {
        {"tag", "#2"}
    });

    EXPECT_EQ(1, counter1->load());
    EXPECT_EQ(0, counter2->load());
}

TEST(Counter, DifferentByTagsDec) {
    registry_t registry;

    auto counter1 = registry.counter<std::int64_t>("metrics.testing.counter", {
        {"tag", "#1"}
    });

    counter1->fetch_add(-1);

    auto counter2 = registry.counter<std::int64_t>("metrics.testing.counter", {
        {"tag", "#2"}
    });

    EXPECT_EQ(-1, counter1->load());
    EXPECT_EQ(0, counter2->load());
}

}  // namespace testing
}  // namespace metrics

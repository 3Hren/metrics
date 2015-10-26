#include <gtest/gtest.h>

#include <boost/optional/optional.hpp>

#include <metrics/counter.hpp>
#include <metrics/registry.hpp>

namespace metrics {
namespace testing {

TEST(Counter, Factory) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    // Default value is 0.
    EXPECT_EQ(0, counter.get());
}

TEST(Counter, Inc) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter.inc();
    EXPECT_EQ(1, counter.get());

    EXPECT_EQ(1, counter.inc().get());
    EXPECT_EQ(2, counter.get());

    EXPECT_EQ(2, counter.inc().get());
    EXPECT_EQ(3, counter.get());
}

TEST(Counter, IncVal) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter.inc(42);
    EXPECT_EQ(42, counter.get());

    EXPECT_EQ(42, counter.inc(10).get());
    EXPECT_EQ(52, counter.get());

    EXPECT_EQ(52, counter.inc(8).get());
    EXPECT_EQ(60, counter.get());
}

TEST(Counter, Shared) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");

    counter.inc();
    EXPECT_EQ(1, counter.get());

    EXPECT_EQ(1, counter.inc().get());
    EXPECT_EQ(2, counter.get());

    auto other = registry.counter<std::uint64_t>("metrics.testing.counter");

    EXPECT_EQ(2, other.inc().get());
    EXPECT_EQ(3, other.get());
}

TEST(Counter, Dec) {
    registry_t registry;

    auto counter = registry.counter<std::int64_t>("metrics.testing.counter");

    counter.dec();
    EXPECT_EQ(-1, counter.get());

    EXPECT_EQ(-1, counter.dec().get());
    EXPECT_EQ(-2, counter.get());

    EXPECT_EQ(-2, counter.dec().get());
    EXPECT_EQ(-3, counter.get());
}

TEST(metric, counter_dec_val) {
    registry_t registry;

    auto counter = registry.counter<std::int64_t>("metrics.testing.counter");

    // Note, that we decrease by positive value, which results in substraction.
    counter.dec(42);
    EXPECT_EQ(-42, counter.get());

    EXPECT_EQ(-42, counter.dec(10).get());
    EXPECT_EQ(-52, counter.get());

    EXPECT_EQ(-52, counter.dec(8).get());
    EXPECT_EQ(-60, counter.get());
}

TEST(Counter, CopyAssignable) {
    registry_t registry;

    auto counter = registry.counter<std::uint64_t>("metrics.testing.counter");
    counter = registry.counter<std::uint64_t>("metrics.testing.other_counter");

    // Default value is 0.
    EXPECT_EQ(0, counter.get());
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

    counter1.inc();

    auto counter2 = registry.counter<std::uint64_t>("metrics.testing.counter", {
        {"tag", "#2"}
    });

    EXPECT_EQ(1, counter1.get());
    EXPECT_EQ(0, counter2.get());
}

TEST(Counter, DifferentByTagsDec) {
    registry_t registry;

    auto counter1 = registry.counter<std::int64_t>("metrics.testing.counter", {
        {"tag", "#1"}
    });

    counter1.dec();

    auto counter2 = registry.counter<std::int64_t>("metrics.testing.counter", {
        {"tag", "#2"}
    });

    EXPECT_EQ(-1, counter1.get());
    EXPECT_EQ(0, counter2.get());
}

}  // namespace testing
}  // namespace metrics

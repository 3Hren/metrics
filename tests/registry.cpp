#include <boost/optional/optional.hpp>

#include <gtest/gtest.h>

#include <metrics/registry.hpp>
#include <metrics/tags.hpp>

namespace metrics {
namespace {

TEST(resistry_t, Query) {
    registry_t registry;
    auto c1 = registry.counter<std::int64_t>("hostname.testing.accepted", {{"source", "node"}});
    auto c2 = registry.counter<std::int64_t>("hostname.testing.rejected", {{"source", "node"}});
    auto c3 = registry.counter<std::int64_t>("hostname.testing.rejected", {{"source", "non-node"}});

    auto r1 = registry.select([](const tagged_t& metric) -> bool {
        return metric.tag("source") && *metric.tag("source") == "node";
    });
    EXPECT_EQ(2, r1.size());

    auto r2 = registry.select([](const tagged_t& metric) -> bool {
        return metric.tag("source") && *metric.tag("source") == "non-node";
    });
    EXPECT_EQ(1, r2.size());
}

TEST(resistry_t, QueryAll) {
    registry_t registry;
    auto c1 = registry.counter<std::int64_t>("hostname.testing.accepted", {{"source", "node"}});
    auto c2 = registry.counter<std::int64_t>("hostname.testing.rejected", {{"source", "node"}});
    auto c3 = registry.counter<std::int64_t>("hostname.testing.rejected", {{"source", "non-node"}});

    auto r1 = registry.select();
    EXPECT_EQ(3, r1.size());
}

TEST(resistry_t, SameGaugesExactSize) {
    const auto name = "<test>";

    registry_t registry;
    auto c1 = registry.register_gauge<std::int64_t>(name, {}, [] {
        return 42;
    });
    EXPECT_EQ(1, registry.gauges<std::int64_t>().size());

    // This one overrides.
    auto r1 = registry.register_gauge<std::int64_t>(name, {}, [] {
        return 100500;
    });
    EXPECT_EQ(1, registry.gauges<std::int64_t>().size());
}

TEST(resistry_t, RemoveGauge) {
    const auto name = "<test>";

    registry_t registry;
    auto c1 = registry.register_gauge<std::int64_t>(name, {}, [] {
        return 42;
    });

    EXPECT_TRUE(registry.remove<gauge<std::int64_t>>(name, {}));
    EXPECT_EQ(0, registry.gauges<std::int64_t>().size());

    EXPECT_FALSE(registry.remove<gauge<std::int64_t>>(name, {}));
}

TEST(resistry_t, SameCounterExactSize) {
    const auto name = "<test>";

    registry_t registry;
    auto c1 = registry.counter<std::int64_t>(name);
    EXPECT_EQ(1, registry.counters<std::int64_t>().size());

    auto r1 = registry.counter<std::int64_t>(name);
    EXPECT_EQ(1, registry.counters<std::int64_t>().size());
}

TEST(resistry_t, RemoveCounter) {
    const auto name = "<test>";

    registry_t registry;
    auto c1 = registry.counter<std::int64_t>(name);

    EXPECT_TRUE(registry.remove<std::atomic<std::int64_t>>(name, {}));
    EXPECT_EQ(0, registry.counters<std::int64_t>().size());

    EXPECT_FALSE(registry.remove<std::atomic<std::int64_t>>(name, {}));
}

TEST(resistry_t, SameMeterExactSize) {
    const auto name = "<test>";

    registry_t registry;
    auto c1 = registry.meter(name);
    EXPECT_EQ(1, registry.meters().size());

    auto r1 = registry.meter(name);
    EXPECT_EQ(1, registry.meters().size());
}

TEST(resistry_t, RemoveMeter) {
    const auto name = "<test>";

    registry_t registry;
    auto c1 = registry.meter(name);

    EXPECT_TRUE(registry.remove<meter_t>(name, {}));
    EXPECT_EQ(0, registry.meters().size());

    EXPECT_FALSE(registry.remove<meter_t>(name, {}));
}

TEST(resistry_t, SameTimerExactSize) {
    const auto name = "<test>";

    registry_t registry;
    auto c1 = registry.timer(name);
    EXPECT_EQ(1, registry.timers().size());

    auto r1 = registry.timer(name);
    EXPECT_EQ(1, registry.timers().size());
}

TEST(resistry_t, RemoveTimer) {
    const auto name = "<test>";

    registry_t registry;
    auto c1 = registry.timer(name);

    EXPECT_TRUE(registry.remove<timer<accumulator::sliding::window_t>>(name, {}));
    EXPECT_EQ(0, registry.timers().size());

    EXPECT_FALSE(registry.remove<timer<accumulator::sliding::window_t>>(name, {}));
}

} // namespace
} // namespace metrics

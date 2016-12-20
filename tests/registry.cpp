#include <boost/optional/optional.hpp>

#include <gtest/gtest.h>

#include <metrics/registry.hpp>
#include <metrics/tags.hpp>

namespace metrics {
namespace {

TEST(resistry_t, Query) {
    registry_t registry;
    auto c1 = registry.counter<std::int64_t>("hostname.testing.accepted", {
        {"source", "node"}
    });
    auto c2 = registry.counter<std::int64_t>("hostname.testing.rejected", {
        {"source", "node"}
    });
    auto c3 = registry.counter<std::int64_t>("hostname.testing.rejected", {
        {"source", "non-node"}
    });

    auto r1 = registry.select([](const tags_t& tags) -> bool {
        return tags.tag("source") && *tags.tag("source") == "node";
    });
    EXPECT_EQ(2, r1.size());

    auto r2 = registry.select([](const tags_t& tags) -> bool {
        return tags.tag("source") && *tags.tag("source") == "non-node";
    });
    EXPECT_EQ(1, r2.size());
}

TEST(resistry_t, QueryAll) {
    registry_t registry;
    auto c1 = registry.counter<std::int64_t>("hostname.testing.accepted", {
        {"source", "node"}
    });
    auto c2 = registry.counter<std::int64_t>("hostname.testing.rejected", {
        {"source", "node"}
    });
    auto c3 = registry.counter<std::int64_t>("hostname.testing.rejected", {
        {"source", "non-node"}
    });

    auto r1 = registry.select();
    EXPECT_EQ(3, r1.size());
}

}  // namespace
}  // namespace metrics

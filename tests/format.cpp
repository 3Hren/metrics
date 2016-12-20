#include <boost/optional/optional.hpp>

#include <rapidjson/document.h>

#include <gtest/gtest.h>

#include <metrics/format/json.hpp>
#include <metrics/meter.hpp>
#include <metrics/registry.hpp>
#include <metrics/tags.hpp>

namespace metrics {
namespace {

TEST(json, counter) {
    registry_t registry;
    auto c1 = registry.counter<std::int64_t>("hostname.accepted", {
        {"source", "node"}
    });
    c1->store(1);
    auto c2 = registry.counter<std::int64_t>("hostname.rejected", {
        {"source", "node"}
    });

    rapidjson::Document doc;
    doc.Parse<0>(format::as_json(registry.select()).c_str());
    ASSERT_TRUE(doc.IsObject());
    ASSERT_TRUE(doc.HasMember("hostname"));
    ASSERT_TRUE(doc["hostname"].HasMember("accepted"));
    ASSERT_TRUE(doc["hostname"].HasMember("rejected"));
    ASSERT_TRUE(doc["hostname"]["accepted"].IsInt());
    ASSERT_TRUE(doc["hostname"]["rejected"].IsInt());

    EXPECT_EQ(1, doc["hostname"]["accepted"].GetInt());
    EXPECT_EQ(0, doc["hostname"]["rejected"].GetInt());
}

TEST(json, meter) {
    registry_t registry;
    auto m = registry.meter("hostname.requests", {
        {"source", "node"}
    });
    m->mark(5);

    rapidjson::Document doc;
    doc.Parse<0>(format::as_json(registry.select()).c_str());
    ASSERT_TRUE(doc.IsObject());
    ASSERT_TRUE(doc.HasMember("hostname"));
    ASSERT_TRUE(doc["hostname"].HasMember("requests"));
    ASSERT_TRUE(doc["hostname"]["requests"].HasMember("count"));
    ASSERT_TRUE(doc["hostname"]["requests"].HasMember("m01rate"));
    ASSERT_TRUE(doc["hostname"]["requests"].HasMember("m05rate"));
    ASSERT_TRUE(doc["hostname"]["requests"].HasMember("m15rate"));

    ASSERT_TRUE(doc["hostname"]["requests"]["count"].IsInt());
    EXPECT_EQ(5, doc["hostname"]["requests"]["count"].GetInt());
}

}  // namespace
}  // namespace metrics

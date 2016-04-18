#include <gtest/gtest.h>

#include <boost/optional/optional.hpp>

#include <metrics/tags.hpp>

namespace metrics {
namespace testing {

TEST(tags_t, Constructor) {
    tags_t tags("name");

    EXPECT_EQ("name", tags.name());
}

TEST(tags_t, Tag) {
    tags_t tags("name", {{"tag", "value"}});

    EXPECT_EQ("value", *tags.tag("tag"));
}

TEST(tags_t, Eq) {
    tags_t tags1("name", {{"tag", "value"}});
    tags_t tags2("name", {{"tag", "value"}});
    tags_t tags3("name", {{"tag", "ooops"}});

    EXPECT_TRUE(tags1 == tags2);
    EXPECT_TRUE(tags2 == tags1);
    EXPECT_TRUE(tags1 != tags3);
    EXPECT_TRUE(tags2 != tags3);
    EXPECT_TRUE(tags3 != tags1);
    EXPECT_TRUE(tags3 != tags2);
}

TEST(tags_t, Hash) {
    tags_t tags("name", {{"tag", "value"}});

    EXPECT_TRUE(std::hash<tags_t>()(tags) != 0);
}

TEST(tags_t, Less) {
    tags_t tags1("name", {{"tag", "value"}});
    tags_t tags2("name", {{"tag", "value"}});
    tags_t tags3("name", {{"tag", "ooops"}});

    EXPECT_FALSE(tags1 < tags2);
    EXPECT_FALSE(tags2 < tags1);
    EXPECT_FALSE(tags1 < tags3);
    EXPECT_FALSE(tags2 < tags3);
    EXPECT_TRUE (tags3 < tags1);
    EXPECT_TRUE (tags3 < tags2);
}

}  // namespace testing
}  // namespace metrics

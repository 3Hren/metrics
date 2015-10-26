#include <gtest/gtest.h>

#include <boost/optional/optional.hpp>

#include <metrics/tagged.hpp>

namespace metrics {
namespace testing {

TEST(Tagged, Constructor) {
    tagged_t tagged("name");

    EXPECT_EQ("name", tagged.name());
}

TEST(Tagged, Tag) {
    tagged_t tagged("name", {{"tag", "value"}});

    EXPECT_EQ("value", *tagged.tag("tag"));
}

TEST(Tagged, Eq) {
    tagged_t tagged1("name", {{"tag", "value"}});
    tagged_t tagged2("name", {{"tag", "value"}});
    tagged_t tagged3("name", {{"tag", "ooops"}});

    EXPECT_TRUE(tagged1 == tagged2);
    EXPECT_TRUE(tagged2 == tagged1);
    EXPECT_TRUE(tagged1 != tagged3);
    EXPECT_TRUE(tagged2 != tagged3);
    EXPECT_TRUE(tagged3 != tagged1);
    EXPECT_TRUE(tagged3 != tagged2);
}

TEST(Tagged, Hash) {
    tagged_t tagged("name", {{"tag", "value"}});

    EXPECT_TRUE(std::hash<tagged_t>()(tagged) != 0);
}

}  // namespace testing
}  // namespace metrics

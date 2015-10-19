#include <gtest/gtest.h>

#include <metrics/detail/ewma.hpp>

namespace metrics {
namespace testing {

using detail::ewma_t;

void
elapse_minute(ewma_t* ewma) {
    for (int i = 0; i < 12; i++) {
        ewma->tick();
    }
}

TEST(ewma, m01rate) {
    auto ewma = ewma_t::m01rate();
    ewma.update(3);
    ewma.tick();

    EXPECT_FLOAT_EQ(0.6, ewma.rate<std::chrono::seconds>());

    for (auto expected : {0.22072766, 0.08120117, 0.02987224, 0.01098938, 0.00404277}) {
        elapse_minute(&ewma);
        EXPECT_FLOAT_EQ(expected, ewma.rate<std::chrono::seconds>());
    }
}

TEST(ewma, m05rate) {
    auto ewma = ewma_t::m05rate();
    ewma.update(3);
    ewma.tick();

    EXPECT_FLOAT_EQ(0.6, ewma.rate<std::chrono::seconds>());

    for (auto expected : {0.49123845, 0.40219203, 0.32928698, 0.26959738, 0.22072766}) {
        elapse_minute(&ewma);
        EXPECT_FLOAT_EQ(expected, ewma.rate<std::chrono::seconds>());
    }
}

TEST(ewma, m15rate) {
    auto ewma = ewma_t::m15rate();
    ewma.update(3);
    ewma.tick();

    EXPECT_FLOAT_EQ(0.6, ewma.rate<std::chrono::seconds>());

    for (auto expected : {0.56130419, 0.52510399, 0.49123845, 0.45955700, 0.42991879}) {
        elapse_minute(&ewma);
        EXPECT_FLOAT_EQ(expected, ewma.rate<std::chrono::seconds>());
    }
}

}  // namespace testing
}  // namespace metrics

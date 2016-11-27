#include <array>

#include <gtest/gtest.h>

#include <metrics/usts/ewma.hpp>

#include <deque>

namespace metrics {
namespace usts {
namespace {

class ineffective_ewma_t {
    const double τ;
    std::deque<std::tuple<std::chrono::high_resolution_clock::time_point, double>> values;
    double average;

public:
    explicit ineffective_ewma_t(std::chrono::high_resolution_clock::duration interval) :
        τ(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count())
    {}

    auto add(std::chrono::high_resolution_clock::time_point time, double value) -> void {
        values.push_back({time, value});

        double sum1 = 0.0;
        double sum2 = 0.0;
        const auto weight = [&](double ẟ) {
            return std::exp(-ẟ / τ);
        };
        for (auto v : values) {
            const auto ẟ = std::chrono::duration_cast<std::chrono::nanoseconds>(time - std::get<0>(v)).count();
            sum1 += weight(ẟ) * std::get<1>(v);
            sum2 += weight(ẟ);
        }
        average = sum1 / sum2;
    }

    auto get() const -> double {
        return average;
    };
};

TEST(usts, USTS) {
    struct point_t {
        std::chrono::high_resolution_clock::time_point t;
        std::int64_t y;
    };

    const std::chrono::high_resolution_clock::time_point birthstamp;
    const std::vector<point_t> points{
        {birthstamp + std::chrono::milliseconds(1000),  1},
        {birthstamp + std::chrono::milliseconds(1100),  2},
        {birthstamp + std::chrono::milliseconds(1200),  3},
        {birthstamp + std::chrono::milliseconds(1300),  2},
        {birthstamp + std::chrono::milliseconds(1400),  1},
        {birthstamp + std::chrono::milliseconds(1500),  0},
        {birthstamp + std::chrono::milliseconds(2000),  1},
        {birthstamp + std::chrono::milliseconds(2100),  2},
        {birthstamp + std::chrono::milliseconds(3000),  3},
        {birthstamp + std::chrono::milliseconds(4000),  4},
        {birthstamp + std::chrono::milliseconds(5000),  5},
        {birthstamp + std::chrono::milliseconds(6000),  6},
        {birthstamp + std::chrono::milliseconds(6100),  7},
        {birthstamp + std::chrono::milliseconds(6200),  8},
        {birthstamp + std::chrono::milliseconds(6300),  9},
        {birthstamp + std::chrono::milliseconds(6400),  10},
        {birthstamp + std::chrono::milliseconds(6500),  12},
        {birthstamp + std::chrono::milliseconds(6600),  13},
        {birthstamp + std::chrono::milliseconds(6700),  14},
        {birthstamp + std::chrono::milliseconds(6800),  15},
        {birthstamp + std::chrono::milliseconds(6900),  16},
        {birthstamp + std::chrono::milliseconds(7000),  15},
        {birthstamp + std::chrono::milliseconds(100000), 14},
        {birthstamp + std::chrono::milliseconds(102000), 13},
        {birthstamp + std::chrono::milliseconds(103000), 12},
        {birthstamp + std::chrono::milliseconds(103100), 11},
        {birthstamp + std::chrono::milliseconds(103200), 10},
        {birthstamp + std::chrono::milliseconds(103300), 9},
        {birthstamp + std::chrono::milliseconds(103400), 8},
        {birthstamp + std::chrono::milliseconds(103500), 7},
        {birthstamp + std::chrono::milliseconds(103600), 6}
    };

    ewma_t average1(std::chrono::milliseconds(1000));
    ineffective_ewma_t average2(std::chrono::milliseconds(1000));

    for (auto point : points) {
        average1.add(point.t, point.y);
        average2.add(point.t, point.y);
    }
}

}  // namespace
}  // namespace usts
}  // namespace metrics

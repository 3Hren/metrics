#include "metrics/counter.hpp"

#include <future>

#include "metrics/detail/processor.hpp"
#include "metrics/detail/utility.hpp"

namespace metrics {
namespace accumulator {
namespace snapshot {

uniform_t::uniform_t(std::vector<value_type> values) {
    std::sort(values.begin(), values.end());

    d.values = std::move(values);
}

const std::vector<uniform_t::value_type>&
uniform_t::values() const {
   return d.values;
}

std::size_t
uniform_t::size() const {
    return d.values.size();
}

std::uint64_t
uniform_t::min() const {
    if (size() == 0) {
        return 0;
    }

    return d.values.front();
}

std::uint64_t
uniform_t::max() const {
    if (size() == 0) {
        return 0;
    }

    return d.values.back();
}

double
uniform_t::mean() const {
    const auto size = this->size();

    if (size == 0) {
        return 0;
    }

    return boost::accumulate(d.values, 0.0) / size;
}

double
uniform_t::stddev() const {
    const auto size = this->size();

    if (size <= 1) {
        return 0;
    }

    const auto mean = this->mean();
    const auto sum = boost::accumulate(d.values, 0.0, [&](double accumulator, double value) -> double {
        const auto diff = value - mean;
        return accumulator + diff * diff;
    });

    const auto variance = sum / (size - 1);

    return std::sqrt(variance);
}

double
uniform_t::value(double quantile) const {
    if (quantile < 0.0 || quantile > 1.0 || std::isnan(quantile)) {
        throw std::invalid_argument("quantile must be in [0; 1] range");
    }

    if (d.values.empty()) {
        return 0.0;
    }

    const auto pos = quantile * (d.values.size() + 1);
    const auto id = static_cast<std::size_t>(pos);

    if (id < 1) {
        return d.values.front();
    }

    if (id >= d.values.size()) {
        return d.values.back();
    }

    const auto lower = d.values[id - 1];
    const auto upper = d.values[id];

    return lower + (pos - std::floor(pos)) * (upper - lower);
}

}  // namespace snapshot
}  // namespace accumulator
}  // namespace metrics

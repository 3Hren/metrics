#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include <boost/range/numeric.hpp>

namespace metrics {
namespace metric {
namespace accumulator {
namespace snapshot {

class uniform_t {
public:
    typedef std::uint64_t value_type;

private:
    struct {
        std::vector<value_type> values;
    } d;

public:
    /// Create a new uniform snapshot with the given values.
    ///
    /// \param `values` an unordered set of values in the accumulator.
    explicit
    uniform_t(std::vector<value_type> values) {
        std::sort(values.begin(), values.end());

        d.values = std::move(values);
    }

    const std::vector<value_type>&
    values() const {
        return d.values;
    }

    std::size_t
    size() const {
        return d.values.size();
    }

    std::uint64_t
    min() const {
        if (size() == 0) {
            return 0;
        }

        return d.values.front();
    }

    std::uint64_t
    max() const {
        if (size() == 0) {
            return 0;
        }

        return d.values.back();
    }

    double
    mean() const {
        const auto size = this->size();

        if (size == 0) {
            return 0;
        }

        return boost::accumulate(d.values, 0.0) / size;
    }

    double
    stddev() const {
        const auto size = this->size();

        if (size <= 1) {
            return 0;
        }

        const auto mean = this->mean();
        const auto sum = boost::accumulate(d.values, 0.0, [&](double accumulator, double value) {
            const auto diff = value - mean;
            return accumulator + diff * diff;
        });

        const auto variance = sum / (size - 1);

        return std::sqrt(variance);
    }

    // TODO: Think about adapters.
    double
    median() const {
        return value(0.5);
    }

    double
    p75() const {
        return value(0.75);
    }

    double
    p90() const {
        return value(0.90);
    }

    double
    p95() const {
        return value(0.95);
    }

    double
    p98() const {
        return value(0.98);
    }

    double
    p99() const {
        return value(0.99);
    }

    double
    value(double quantile) const {
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
};

}  // namespace snapshot
}  // namespace accumulator
}  // namespace metric
}  // namespace metrics

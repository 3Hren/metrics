#include "metrics/accumulator/snapshot/weighted.hpp"

#include <cmath>
#include <functional>
#include <system_error>

#include <boost/range/numeric.hpp>

namespace metrics {
namespace accumulator {
namespace snapshot {

weighted_t::weighted_t(std::vector<sample_t> samples) {
    data.reserve(samples.size());
    weights.reserve(samples.size());
    quantiles.reserve(samples.size());

    std::sort(samples.begin(), samples.end(), [&](const sample_t& pivot, const sample_t& other) -> bool {
        return pivot.value < other.value;
    });

    const auto sum = boost::accumulate(samples, 0.0, [&](double accumulator, const sample_t& sample) -> double {
        return accumulator + sample.weight;
    });

    for (const auto& sample : samples) {
        data.push_back(sample.value);
        weights.push_back(sample.weight / sum);
    }

    boost::partial_sum(weights, std::back_inserter(quantiles), std::plus<double>());
}

auto weighted_t::size() const noexcept -> std::size_t {
    return values().size();
}

auto weighted_t::values() const noexcept -> const std::vector<std::uint64_t>& {
    return data;
}

auto weighted_t::value(double quantile) const -> double {
    if (quantile < 0.0 || quantile > 1.0 || std::isnan(quantile)) {
        throw std::invalid_argument("quantile must be in [0; 1] range");
    }

    if (data.empty()) {
        return 0.0;
    }

    const auto it = std::upper_bound(quantiles.begin(), quantiles.end(), quantile);

    if (it == std::end(quantiles)) {
        return data.back();
    }

    return data[std::distance(quantiles.begin(), it)];
}

auto weighted_t::min() const -> std::uint64_t {
    if (size() == 0) {
        return 0;
    }

    return data.front();
}

auto weighted_t::max() const -> std::uint64_t {
    if (size() == 0) {
        return 0;
    }

    return data.back();
}

auto weighted_t::mean() const -> double {
    if (size() == 0) {
        return 0;
    }

    double sum = 0.0;
    for (std::size_t i = 0; i < size(); ++i) {
        sum += data[i] * weights[i];
    }

    return sum;
}

auto weighted_t::stddev() const -> double {
    const auto size = this->size();

    if (size <= 1) {
        return 0;
    }

    double variance = 0.0;
    const auto mean = this->mean();
    for (std::size_t i = 0; i < data.size(); ++i) {
        const auto diff = data[i] - mean;
        variance += weights[i] * diff * diff;
    }

    return std::sqrt(variance);
}

}  // namespace snapshot
}  // namespace accumulator
}  // namespace metrics

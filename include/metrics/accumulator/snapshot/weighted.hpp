#pragma once

#include <cstdint>
#include <vector>

namespace metrics {
namespace accumulator {
namespace snapshot {

/// A statistical snapshot of a type T.
template<typename T>
class snapshot {
public:
    /// Returns the median value in the distribution.
    ///
    /// \return the median value.
    auto median() const -> double {
        return self().value(0.5);
    }

    /// Returns the value at the 75th percentile in the distribution.
    ///
    /// \return the value at the 75th percentile.
    auto p75() const -> double {
        return self().value(0.75);
    }

    /// Returns the value at the 90th percentile in the distribution.
    ///
    /// \return the value at the 90th percentile.
    auto p90() const -> double {
        return self().value(0.90);
    }

    /// Returns the value at the 95th percentile in the distribution.
    ///
    /// \return the value at the 95th percentile.
    auto p95() const -> double {
        return self().value(0.95);
    }

    /// Returns the value at the 98th percentile in the distribution.
    ///
    /// \return the value at the 98th percentile.
    auto p98() const -> double {
        return self().value(0.98);
    }

    /// Returns the value at the 99th percentile in the distribution.
    ///
    /// \return the value at the 99th percentile.
    auto p99() const -> double {
        return self().value(0.99);
    }

private:
    auto self() const -> const T& {
        return static_cast<const T&>(*this);
    }
};

class weighted_t : public snapshot<weighted_t> {
public:
    struct sample_t {
        std::uint64_t value;
        double weight;
    };

private:
    std::vector<std::uint64_t> data;
    std::vector<double> weights;
    std::vector<double> quantiles;

public:
    /// Creates a new snapshot with the given values.
    ///
    /// \param an unordered set of values in the accumulator.
    explicit weighted_t(std::vector<sample_t> values);

    /// Returns the number of values in the snapshot.
    auto size() const noexcept -> std::size_t;

    /// Returns a reference for the entire set of values in the snapshot.
    auto values() const noexcept -> const std::vector<std::uint64_t>&;

    /// Returns the value at the given quantile.
    ///
    /// \param quantile a given quantile, in [0..1].
    /// \return the value in the distribution at quantile.
    auto value(double quantile) const -> double;

    /// Reverse of this->value(quantile)
    ///
    /// \param value value to found a quantile for.
    /// \return the phi (weighted proportion) for value in [0..1].
    auto phi(double value) const -> double;

    /// Returns the lowest value in the snapshot.
    auto min() const -> std::uint64_t;

    /// Returns the highest value in the snapshot.
    auto max() const -> std::uint64_t;

    /// Returns the weighted arithmetic mean of the values in the snapshot.
    auto mean() const -> double;

    /// Returns the weighted standard deviation of the values in the snapshot.
    auto stddev() const -> double;
};

}  // namespace snapshot
}  // namespace accumulator
}  // namespace metrics

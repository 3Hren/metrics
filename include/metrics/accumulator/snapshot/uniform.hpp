#pragma once

#include <cstdint>
#include <vector>

namespace metrics {
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
    uniform_t(std::vector<value_type> values);

    /// Returns the number of values in the snapshot.
    std::size_t
    size() const noexcept;

    /// Returns a reference for the entire set of values in the snapshot.
    const std::vector<value_type>&
    values() const noexcept;

    /// Returns the lowest value in the snapshot.
    std::uint64_t
    min() const;

    /// Returns the highest value in the snapshot.
    std::uint64_t
    max() const;

    /// Returns the arithmetic mean of the values in the snapshot.
    double
    mean() const;

    /// Returns the standard deviation of the values in the snapshot.
    double
    stddev() const;

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

    /// Returns the value at the given quantile.
    ///
    /// \param quantile a given quantile, in [0; 1] range.
    double
    value(double quantile) const;
};

}  // namespace snapshot
}  // namespace accumulator
}  // namespace metrics

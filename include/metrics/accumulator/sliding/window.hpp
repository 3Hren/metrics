#pragma once

#include <algorithm>

#include "metrics/accumulator/snapshot/uniform.hpp"

namespace metrics {
namespace accumulator {
namespace sliding {

/// An accumulator implementation backed by a sliding window that stores the last N measurements.
class window_t {
public:
    typedef std::uint64_t value_type;
    typedef snapshot::uniform_t snapshot_type;

    std::vector<std::uint64_t> measurements;
    std::size_t count;

public:
    /// Creates a new sliding window accumulator which stores the last 1024 measurements.
    window_t():
        measurements(1024),
        count(0)
    {}

    /// Creates a new sliding window accumulator which stores the last `size` measurements.
    ///
    /// \param `size` the number of measurements to store.
    explicit
    window_t(std::size_t size):
        measurements(size),
        count(0)
    {}

    snapshot_type
    snapshot() const {
        const auto size = this->size();

        std::vector<std::uint64_t> result;
        result.reserve(size);

        std::copy_n(measurements.begin(), size, std::back_inserter(result));

        return snapshot_type(result);
    }

    std::size_t
    size() const noexcept {
        return std::min(count, measurements.size());
    }

    void
    update(value_type value) noexcept {
        measurements[count++ % measurements.size()] = value;
    }

    void
    operator()(value_type value) noexcept {
        update(value);
    }
};

}  // namespace sliding
}  // namespace accumulator
}  // namespace metrics

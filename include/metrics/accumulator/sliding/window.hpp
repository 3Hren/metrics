#pragma once

#include <atomic>
#include <mutex>

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
    std::atomic<std::size_t> count;

    mutable std::mutex mutex;

public:
    /// Creates a new sliding window accumulator which stores the last 1024 measurements.
    window_t();

    /// Creates a new sliding window accumulator which stores the last `size` measurements.
    ///
    /// \param `size` the number of measurements to store.
    explicit window_t(std::size_t size);

    auto snapshot() const -> snapshot_type;

    auto size() const noexcept -> std::size_t;

    auto update(value_type value) noexcept -> void;
    auto operator()(value_type value) noexcept -> void;
};

}  // namespace sliding
}  // namespace accumulator
}  // namespace metrics

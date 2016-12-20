#pragma once

#include "metrics/metric.hpp"

namespace metrics {
inline namespace v2 {

/// A `meter` measures the rate at which a set of events occur.
///
/// Just like the Unix load averages visible in `top`.
class meter_t {
public:
    virtual ~meter_t() = 0;

    /// Returns the number of events which have been marked.
    virtual auto count() const -> std::uint64_t = 0;

    /// Returns the mean rate at which events have occurred since the meter was created.
    virtual auto mean_rate() const -> double = 0;

    /// Returns the one-minute exponentially-weighted moving average rate at which events have
    /// occurred since the meter was created.
    virtual auto m01rate() const -> double = 0;

    /// Returns the five-minute exponentially-weighted moving average rate at which events have
    /// occurred since the meter was created.
    virtual auto m05rate() const -> double = 0;

    /// Returns the fifteen-minute exponentially-weighted moving average rate at which events have
    /// occurred since the meter was created.
    virtual auto m15rate() const -> double = 0;

    /// Mark the occurrence of an event.
    virtual auto mark() -> void = 0;

    /// Mark the occurrence of a given number of events.
    virtual auto mark(std::uint64_t value) -> void = 0;
};

} // namespace v2
} // namespace metrics

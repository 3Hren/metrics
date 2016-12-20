#pragma once

#include <array>
#include <chrono>

#include "metrics/timer.hpp"

namespace metrics {
namespace detail {

/// A timer metric which aggregates timing durations and provides duration statistics, plus
/// throughput statistics via `meter`.
///
/// \tparam `Clock` must meet `TrivialClock` requirements and must be steady.
template<class Clock, class Meter, class Histogram>
class timer : public metrics::timer<typename Histogram::accumulator_type> {
public:
    typedef Clock clock_type;
    typedef typename clock_type::time_point time_point;
    typedef typename clock_type::duration duration_type;

    typedef Meter meter_type;
    typedef Histogram histogram_type;
    typedef typename histogram_type::snapshot_type snapshot_type;

    // TODO: Suppress this check for GCC and pray, because `steady_clock` isn't available until 4.9.
#ifdef __clang__
    static_assert(clock_type::is_steady, "clock type must be steady");
#endif

private:
    struct data_t {
        clock_type clock;
        mutable meter_type meter;
        histogram_type histogram;

        template<typename... Args>
        data_t(Args&&... args):
            histogram(std::forward<Args>(args)...)
        {}
    } d;

public:
    template<typename... Args>
    timer(Args&&... args):
        d(std::forward<Args>(args)...)
    {}

    /// Dependency observers.

    /// Returns a const reference to the clock implementation.
    auto clock() const noexcept -> const clock_type& {
        return d.clock;
    }

    /// Returns a const reference to the histogram implementation.
    auto histogram() const noexcept -> const histogram_type& {
        return d.histogram;
    }

    auto meter() const noexcept -> const meter_type& {
        return d.meter;
    }

    /// Lookup.

    auto now() const -> time_point {
        return clock().now();
    }

    auto count() const noexcept -> std::uint64_t {
        return histogram().count();
    }

    /// Returns the one-minute exponentially-weighted moving average rate at which events have
    /// occurred since the timer was created.
    auto m01rate() const -> double {
        return d.meter.m01rate();
    }

    /// Returns the five-minute exponentially-weighted moving average rate at which events have
    /// occurred since the timer was created.
    auto m05rate() const -> double {
        return d.meter.m05rate();
    }

    /// Returns the fifteen-minute exponentially-weighted moving average rate at which events have
    /// occurred since the timer was created.
    auto m15rate() const -> double {
        return d.meter.m15rate();
    }

    auto snapshot() const -> snapshot_type {
        return histogram().snapshot();
    }

    /// Modifiers.

    auto update(duration_type duration) -> void {
        d.histogram.update(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
        d.meter.mark();
    }
};

}  // namespace detail
}  // namespace metrics

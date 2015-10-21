#pragma once

#include <array>
#include <chrono>

namespace metrics {
namespace detail {

/// A timer metric which aggregates timing durations and provides duration statistics, plus
/// throughput statistics via `meter`.
///
/// \type `Clock` must meet `TrivialClock` requirements and must be steady.
template<class Clock, class Histogram, class = void>
class timer {
public:
    typedef Clock clock_type;
    typedef typename clock_type::time_point time_point;
    typedef typename clock_type::duration duration_type;

    typedef Histogram histogram_type;

    static_assert(clock_type::is_steady, "clock type must be steady");

private:
    struct data_t {
        clock_type clock;
        histogram_type histogram;

        template<typename... Args>
        data_t(Args&&... args):
            histogram(std::forward<Args>(args)...)
        {}
    } d;

    struct context_t {
        timer* parent;
        const time_point timestamp;

        context_t(timer* parent):
            parent(parent),
            timestamp(parent->clock().now())
        {}

        context_t(const context_t& other) = delete;
        context_t(context_t&&) = default;

        ~context_t() {
            const auto now = parent->clock().now();
            const auto elapsed = now - timestamp;

            try {
                parent->update(elapsed);
            } catch (...) {
                // Ignore this, because of destructor exception safety reasons. Actually update
                // operation should never throw.
            }
        }
    };

public:
    template<typename... Args>
    timer(Args&&... args):
        d(std::forward<Args>(args)...)
    {}

    /// Dependency observers.

    /// Returns a const reference to the clock implementation.
    const clock_type&
    clock() const noexcept {
        return d.clock;
    }

    /// Returns a const reference to the histogram implementation.
    const histogram_type&
    histogram() const noexcept {
        return d.histogram;
    }

    /// Lookup.

    std::uint64_t
    count() const noexcept {
        return histogram().count();
    }

    /// Modifiers.

    /// Adds a recorded duration.
    void
    update(duration_type duration) {
        histogram().update(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
        // meter.mark();
    }

    template<typename F>
    auto
    measure(F fn) -> decltype(fn()) {
        context_t context(this);
        return fn();
    }
};

}  // namespace detail
}  // namespace metrics

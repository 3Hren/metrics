#pragma once

#include <future>

#include "metrics/metric.hpp"

namespace metrics {

/// Represents a timer metric which aggregates timing durations and provides duration statistics,
/// plus throughput statistics via meter.
///
/// \tparam `Accumulate` must meet `Accumulate` requirements.
template<class Accumulate>
class timer {
public:
    typedef std::chrono::high_resolution_clock clock_type;
    typedef clock_type::time_point time_point;
    typedef clock_type::duration duration_type;

    typedef Accumulate accumulator_type;
    typedef typename accumulator_type::snapshot_type snapshot_type;

    /// Represents a measured RAII context. Notifies the parent timer on destruction by updating its
    /// statistics.
    class context_t {
        struct empty_deleter {
            auto operator()(timer*) -> void {}
        };

        /// Pointer to the parent timer. Note that it doesn't allocate memory on heap.
        std::unique_ptr<timer, empty_deleter> parent;
        const time_point timestamp;

    public:
        explicit context_t(timer* parent) :
            parent(parent, empty_deleter()),
            timestamp(parent->now())
        {}

        context_t(const context_t& other) = delete;
        context_t(context_t&& other) = default;

        ~context_t() {
            const auto now = parent->now();
            const auto elapsed = now - timestamp;

            try {
                parent->update(elapsed);
            } catch (...) {
                // Ignore this, because of destructor exception safety reasons. Actually update
                // operation should never throw.
            }
        }

        auto operator=(const context_t& other) -> context_t& = delete;
        auto operator=(context_t&& other) -> context_t& = delete;
    };

    typedef context_t context_type;

public:
    virtual ~timer() {}

    /// Returns a time point representing the current point in time.
    virtual auto now() const -> time_point = 0;

    /// Returns the number of events which have occurred.
    virtual auto count() const -> std::uint64_t = 0;

    /// Returns the one-minute exponentially-weighted moving average rate at which events have
    /// occurred since the timer was created.
    virtual auto m01rate() const -> double = 0 ;

    /// Returns the five-minute exponentially-weighted moving average rate at which events have
    /// occurred since the timer was created.
    virtual auto m05rate() const -> double = 0;

    /// Returns the fifteen-minute exponentially-weighted moving average rate at which events have
    /// occurred since the timer was created.
    virtual auto m15rate() const -> double = 0;

    /// Returns the full statistics snapshot.
    virtual auto snapshot() const -> snapshot_type = 0;

    /// Creates new measure context and returns it.
    auto context() -> context_type {
        return context_type(this);
    }

    /// Measures the time consumption of the given
    template<typename F>
    auto measure(F fn) -> decltype(fn()) {
        context_t context(this);
        return fn();
    }

private:
    /// Adds a recorded duration.
    virtual auto update(duration_type value) -> void = 0;
};

}  // namespace metrics

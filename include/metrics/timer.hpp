#pragma once

#include "metrics/metric.hpp"

namespace metrics {

/// Represents timer interface.
class timer_t {
public:
    typedef std::chrono::high_resolution_clock clock_type;
    typedef clock_type::time_point time_point;
    typedef clock_type::duration duration_type;

    /// Represents a measured RAII context. Notifies the parent timer on destruction by updating its
    /// statistics.
    ///
    /// Cannot be copied, but it's okay to move objects of this class.
    class context_t {
        struct empty_deleter {
            auto operator()(timer_t*) -> void {}
        };

        /// Pointer to the parent timer. Note that it doesn't allocate memory on the heap.
        std::unique_ptr<timer_t, empty_deleter> parent;
        const time_point timestamp;

    public:
        context_t(timer_t* parent);
        context_t(const context_t& other) = delete;
        context_t(context_t&& other) = default;

        ~context_t();

        auto operator=(const context_t& other) -> context_t& = delete;
        auto operator=(context_t&& other) -> context_t& = default;

        /// Discards the current context, forcing it not to update the assosiated timer on
        /// destruction.
        ///
        /// Can be safely called multiple times.
        auto discard() -> void;
    };

public:
    virtual ~timer_t() = 0;

    /// Returns a time point representing the current point in time.
    virtual auto now() const -> time_point = 0;

    /// Creates new measure context and returns it.
    ///
    /// The timer will be updated automatically on context destruction.
    ///
    /// \note it's the user responsibility to keep timer instance alive until all detached contexts
    ///     be destroyed.
    auto context() -> context_t;

    /// Measures the time consumption of the given callable.
    template<typename F>
    auto measure(F fn) -> decltype(fn()) {
        context_t context(this);
        return fn();
    }

    /// Adds a manually recorded duration.
    virtual auto update(duration_type duration) -> void = 0;
};

/// Represents a timer metric which aggregates timing durations and provides duration statistics,
/// plus throughput statistics via meter.
///
/// \tparam `Accumulate` must meet `Accumulate` requirements.
template<class Accumulate>
class timer : public timer_t {
public:
    typedef timer_t::clock_type clock_type;
    typedef timer_t::time_point time_point;
    typedef timer_t::duration_type duration_type;

    typedef Accumulate accumulator_type;
    typedef typename accumulator_type::snapshot_type snapshot_type;

public:
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
};

}  // namespace metrics

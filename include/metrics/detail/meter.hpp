#pragma once

#include <array>
#include <chrono>
#include <cmath>

#include "metrics/ewma.hpp"

namespace metrics {
namespace detail {

template<class Clock>
class meter {
    typedef Clock clock_type;
    typedef typename clock_type::time_point time_point;

    struct {
        clock_type clock;
        std::uint64_t count;
    } d;

    time_point birthstamp;
    time_point prev;

    std::array<ewma_t, 3> rates;

public:
    /// Creates a new `meter`.
    meter() :
        birthstamp(d.clock.now()),
        prev(birthstamp),
        rates{{
            {-std::expm1(-5.0 / 60 / std::chrono::minutes(1).count()), std::chrono::seconds(5)},
            {-std::expm1(-5.0 / 60 / std::chrono::minutes(5).count()), std::chrono::seconds(5)},
            {-std::expm1(-5.0 / 60 / std::chrono::minutes(15).count()), std::chrono::seconds(5)}
        }}
    {
        d.count = 0;
    }

    /// Dependency observers.

    /// Returns a const reference to the clock implementation.
    const clock_type&
    clock() const noexcept {
        return d.clock;
    }

    /// Returns the number of events which have been marked.
    std::uint64_t
    count() const {
        return d.count;
    }

    /// Returns the mean rate at which events have occurred since the meter was created.
    double
    mean_rate() const {
        const auto count = this->count();

        if (count == 0) {
            return 0.0;
        }

        const auto elapsed = std::chrono::duration_cast<
            std::chrono::seconds
        >(clock().now() - birthstamp).count();

        return static_cast<double>(count) / elapsed;
    }

    /// Returns the one-minute exponentially-weighted moving average rate at which events have
    /// occurred since the meter was created.
    double
    m01rate() {
        return mxxrate<0>();
    }

    /// Returns the five-minute exponentially-weighted moving average rate at which events have
    /// occurred since the meter was created.
    double
    m05rate() {
        return mxxrate<1>();
    }

    /// Returns the fifteen-minute exponentially-weighted moving average rate at which events have
    /// occurred since the meter was created.
    double
    m15rate() {
        return mxxrate<2>();
    }

    /// Mark the occurrence of a given number of events.
    void
    mark(std::uint64_t value = 1) {
        tick_maybe();

        d.count += value;

        for (auto& rate : rates) {
            rate.update(value);
        }
    }

private:
    template<std::size_t N>
    typename std::enable_if<N >= 0 && N < 3, double>::type
    mxxrate() {
        tick_maybe();
        return rates[N].template rate<std::chrono::seconds>();
    }

    void
    tick_maybe() {
        const auto now = clock().now();
        const auto elapsed = now - prev;

        if (elapsed > std::chrono::seconds(5)) {
            prev = now;

            const auto ticks = elapsed / std::chrono::seconds(5);

            for (auto i = 0; i < ticks; ++i) {
                for (auto& rate : rates) {
                    rate.tick();
                }
            }
        }
    }
};

typedef meter<std::chrono::high_resolution_clock> meter_t;

}  // namespace detail
}  // namespace metrics

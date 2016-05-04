#pragma once

#include <array>
#include <chrono>
#include <cmath>

#include "metrics/meter.hpp"

#include "metrics/detail/ewma.hpp"

namespace metrics {
namespace detail {

template<class Clock>
class meter : public metrics::meter_t {
    typedef Clock clock_type;
    typedef typename clock_type::time_point time_point;

    struct {
        clock_type clock;
        std::atomic<std::uint64_t> count;
    } d;

    time_point birthstamp;
    std::atomic<std::int64_t> prev;

    ewma_t m01r;
    ewma_t m05r;
    ewma_t m15r;

public:
    /// Creates a new `meter`.
    meter() :
        birthstamp(d.clock.now()),
        prev(std::chrono::duration_cast<std::chrono::seconds>(birthstamp.time_since_epoch()).count()),
        m01r(ewma_t::m01rate()),
        m05r(ewma_t::m05rate()),
        m15r(ewma_t::m15rate())
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
    auto count() const -> std::uint64_t {
        return d.count;
    }

    /// Returns the mean rate at which events have occurred since the meter was created.
    auto mean_rate() const -> double {
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
    auto m01rate() -> double {
        tick_maybe();
        return m01r.template rate<std::chrono::seconds>();
    }

    /// Returns the five-minute exponentially-weighted moving average rate at which events have
    /// occurred since the meter was created.
    auto m05rate() -> double {
        tick_maybe();
        return m05r.template rate<std::chrono::seconds>();
    }

    /// Returns the fifteen-minute exponentially-weighted moving average rate at which events have
    /// occurred since the meter was created.
    auto m15rate() -> double {
        tick_maybe();
        return m15r.template rate<std::chrono::seconds>();
    }

    auto mark() -> void {
        mark(1);
    }

    /// Mark the occurrence of a given number of events.
    auto mark(std::uint64_t value) -> void {
        tick_maybe();

        d.count += value;

        m01r.update(value);
        m05r.update(value);
        m15r.update(value);
        // for (auto& rate : rates) {
        //     rate.update(value);
        // }
    }

private:
    auto tick_maybe() -> void {
        const auto now = std::chrono::duration_cast<
            std::chrono::seconds
        >(clock().now().time_since_epoch()).count();
        auto prev = this->prev.load();
        const auto elapsed = now - prev;

        if (elapsed > 5) {
            // Clock values should monotonically increase, so no ABA problem here is possible.
            if (this->prev.compare_exchange_strong(prev, now - elapsed % 5)) {
                const auto ticks = elapsed / 5;

                for (auto i = 0; i < ticks; ++i) {
                    m01r.tick();
                    m05r.tick();
                    m15r.tick();
                    // for (auto& rate : rates) {
                    //     rate.tick();
                    // }
                }
            }
        }
    }
};

typedef meter<std::chrono::high_resolution_clock> meter_t;

}  // namespace detail
}  // namespace metrics

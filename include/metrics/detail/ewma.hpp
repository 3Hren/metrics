#pragma once

#include <chrono>

namespace metrics {
namespace detail {

/// An exponentially-weighted moving average.
///
/// \see http://www.teamquest.com/pdfs/whitepaper/ldavg1.pdf UNIX Load Average Part 1: How It Works
/// \see http://www.teamquest.com/pdfs/whitepaper/ldavg2.pdf UNIX Load Average Part 2
/// \see http://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average EMA
class ewma_t {
public:
    typedef std::chrono::high_resolution_clock clock_type;

private:
    bool initialized;

    struct {
        double rate;
    } d;

    std::uint64_t uncounted;

    double alpha;
    double interval;

public:
    /// Creates a new EWMA with a specific smoothing constant.
    ewma_t(double alpha, clock_type::duration interval);

    /// Creates a new EWMA which is equivalent to the UNIX one minute load average and which
    /// expects to be ticked every 5 seconds.
    static ewma_t m01rate();

    /// Creates a new EWMA which is equivalent to the UNIX five minute load average and which
    /// expects to be ticked every 5 seconds.
    static ewma_t m05rate();

    /// Creates a new EWMA which is equivalent to the UNIX fifteen minute load average and which
    /// expects to be ticked every 5 seconds.
    static ewma_t m15rate();

    /// Update the moving average with a new value.
    void
    update(std::uint64_t value);

    /// Mark the passage of time and decay the current rate accordingly.
    void
    tick();

    /// Returns the rate in the given units of time.
    ///
    /// \type `T` the unit of time, must be `std::chrono::duration`.
    template<typename T>
    double
    rate() const {
        return d.rate * std::chrono::duration_cast<std::chrono::nanoseconds>(T(1)).count();
    }
};

}  // namespace detail
}  // namespace metrics

#pragma once

#include <atomic>
#include <chrono>
#include <cmath>
#include <mutex>

#include <boost/assert.hpp>

namespace metrics {
namespace usts {

/// Exponentially weighted moving average for an unequally spaced time series.
///
/// The moving average (MA) is a useful tool to summarize the past behavior of a time series at
/// any time point.
///
/// The exponentially weighted moving average (EWMA) involves using exponential weighting function
/// which declines exponentially with the time distance of the past observations from now.
///
/// \tparam Clock Clock type, must be monotonic.
///
/// \note Thread-safety: reentrant.
/// \warning unstable. Use at your own risk.
///
/// \sa http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.324.6082&rep=rep1&type=pdf.
template<typename Clock>
class ewma {
public:
    typedef Clock clock_type;
    typedef typename clock_type::duration duration;
    typedef typename clock_type::time_point time_point;

private:
    // Window size in nanoseconds.
    const double tau;

    // Previous time point value.
    time_point prev;
    // Result.
    double average;

    time_point birthstamp;
    std::atomic_flag initialized;

public:
    explicit ewma(duration interval);

    /// Checks whether the moving average was warmed up and its value may be considered valid.
    ///
    /// By initializing moving average with its first value added we introduce an error, which
    /// declines respectively for a discrete series. After the build-up time, when we start using
    /// the EWMA, we expect it to be (almost) free of initialization errors.
    auto warmed_up() const -> bool;

    /// Adds a point using current time, updating the moving average.
    ///
    /// \param value Observed value.
    auto add(double value) -> void;

    /// Adds a point updating the moving average.
    ///
    /// \overload
    /// \param time Time point. Must be monotonically increased comparing with previous values.
    /// \param value Observed value.
    auto add(time_point time, double value) -> void;

    /// Returns the moving average result.
    auto get() const -> double;
};

using ewma_t = ewma<std::chrono::high_resolution_clock>;

} // namespace usts
} // namespace metrics

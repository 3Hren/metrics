#pragma once

#include <cstddef>

namespace metrics {
namespace accumulator {
namespace decaying {

class exponentially_t {
public:
    typedef std::chrono::high_resolution_clock clock_type;
    typedef clock_type::time_point time_point;
    typedef clock_type::duration duration_type;

public:
    /// Creates a new exponentially decaying accumulator.
    ///
    /// \param size the number of samples to keep in the sampling accumulator.
    /// \param alpha the exponential decay factor; the higher this is, the more biased the
    ///     accumulator will be towards newer values.
    exponentially_t(std::size_t size, double alpha) {
        // this(size, alpha, Clock.defaultClock());
    }

    void update(std::uint64_t value, time_point timestamp) {
        update(value, currentTimeInSeconds());
    }
};

}  // namespace decaying
}  // namespace accumulator
}  // namespace metrics

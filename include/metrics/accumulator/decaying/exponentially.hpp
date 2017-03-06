#pragma once

#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
#include <random>

#include <cstddef>

#include <boost/optional.hpp>

#include "metrics/accumulator/snapshot/weighted.hpp"

namespace metrics {
namespace accumulator {
namespace decaying {

class exponentially_t {
public:
    typedef std::chrono::high_resolution_clock clock_type;
    typedef clock_type::time_point time_point;
    typedef clock_type::duration duration_type;

    typedef std::chrono::seconds seconds_type;
    typedef std::chrono::microseconds us_type;
    typedef us_type::rep us_int_type;

    // sample_type = struct{ .value, .weight }
    typedef snapshot::weighted_t snapshot_type;
    typedef snapshot_type::sample_t sample_type;

    typedef std::map<double, sample_type> samples_mapping_type;
private:
    size_t sample_size;
    double alpha;

    // Forward decay starting point (aka L in `literature`)
    // Note: all time difference computations are done within seconds resolution,
    //       but `rescale_time` stored and adjusted in microseconds resolution
    //       in order to be compatible with tiny rescale periods.
    time_point start_time;
    const duration_type rescale_threshold;
    std::atomic<us_int_type> rescale_time;

    samples_mapping_type samples;

    std::mt19937 gen;
    std::uniform_real_distribution<> uniform_dist;
    typedef std::uniform_real_distribution<>::result_type distr_real_type;

    // should be shared_mutex or probably atomic based logic someday
    mutable std::mutex samples_mut;
public:
    /// Creates a new exponentially decaying accumulator.
    ///
    /// \param size the number of samples to keep in the sampling accumulator.
    /// \param alpha the exponential decay factor; the higher this is, the more biased the
    ///     accumulator will be towards newer values.
    /// \param rescale_period upon that time interval rescaling should be done,
    ///     note that rescaling is done on value update, but before value sampling
    ///     into reservoir.
    /// \param seed to random device (std::random_device is used in case of boost::none),
    ///     provided mostly for debug and testing.
    exponentially_t(std::size_t size, double alpha,
        duration_type rescale_period = std::chrono::hours(1),
        boost::optional<std::mt19937::result_type> seed = boost::none);

    auto update(std::uint64_t value, time_point timestamp = clock_type::now()) -> void;
    auto operator()(std::uint64_t value, time_point timestamp = clock_type::now()) -> void;

    auto snapshot() const -> snapshot_type;

    auto size() const noexcept -> size_t;
private:
    auto rescale(time_point current, us_int_type next) -> void;
};

}  // namespace decaying
}  // namespace accumulator
}  // namespace metrics

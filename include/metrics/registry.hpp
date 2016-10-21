#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>

#include "metrics/forwards.hpp"
#include "metrics/gauge.hpp"
#include "metrics/metric.hpp"
#include "metrics/tags.hpp"

namespace metrics {

/// TODO: Docs.
class registry_t {
    class inner_t;
    std::unique_ptr<inner_t> inner;

public:
    /// Constructs a new metric registry.
    registry_t();

    /// Destroys the current metric registry, freeing all its allocated resources and joining the
    /// processor thread.
    ~registry_t();

    /// Registers a gauge shared metric that is mapped to a given tags.
    ///
    /// \param name gauge name.
    /// \param tags additional tags.
    /// \param fn function that is called by gauge every time the value requested. Must be thread
    ///     safe if planned to use in multi-threaded environment.
    template<typename R>
    auto register_gauge(std::string name, tags_t::container_type tags, std::function<R()> fn) ->
        void;

    /// Returns a gauge shared metric that is mapped to a given tags.
    ///
    /// \throws std::out_of_range if a gauge with either given name or type T does not exist.
    ///
    /// \param name gauge name.
    /// \param tags optional additional tags.
    /// \tparam `T` must be either std::uint64_t, std::int64_t or double.
    template<typename T>
    auto gauge(std::string name, tags_t::container_type tags = tags_t::container_type()) const
        -> shared_metric<gauge<T>>;

    /// Returns a counter shared metric that is mapped to a given tags, performing a creation with
    /// registering if such metric does not already exist.
    ///
    /// \param name counter name.
    /// \param tags optional additional tags.
    /// \tparam `T` must be either std::uint64_t or std::int64_t.
    template<typename T>
    auto counter(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        shared_metric<std::atomic<T>>;

    template<typename T>
    auto counters() const -> std::map<tags_t, shared_metric<std::atomic<T>>>;

    /// Returns a meter shared metric that is mapped to a given tags, performing a creation with
    /// registering if such metric does not already exist.
    ///
    /// \param name meter name.
    /// \param tags optional additional tags.
    auto meter(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        shared_metric<meter_t>;

    auto meters() const -> std::map<tags_t, shared_metric<meter_t>>;

    /// Returns a timer shared metric that is mapped to a given tags, performing a creation with
    /// registering if such metric does not already exist.
    ///
    /// \todo change default accumulator to `exponentially_decaying_t`.
    /// \param name timer name.
    /// \param tags optional additional tags.
    /// \tparam Accumulate must meet Accumulate requirements.
    template<class Accumulate = accumulator::sliding::window_t>
    auto timer(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        shared_metric<timer<Accumulate>>;

    template<class Accumulate = accumulator::sliding::window_t>
    auto timers() const -> std::map<tags_t, shared_metric<metrics::timer<Accumulate>>>;
};

}  // namespace metrics

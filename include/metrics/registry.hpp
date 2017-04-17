#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "fwd.hpp"
#include "gauge.hpp"
#include "metric.hpp"
#include "tags.hpp"

namespace metrics {

/// A registry of metric instances.
class registry_t {
    class inner_t;
    std::unique_ptr<inner_t> inner;

public:
    template<typename T>
    using metric_set = std::map<tags_t, shared_metric<T>>;

    /// Constructs a new metric registry.
    registry_t();

    /// Destroys the current metric registry, freeing all its allocated resources.
    ~registry_t();

    /// Registers a gauge shared metric that is mapped to a given tags.
    ///
    /// \param name gauge name.
    /// \param tags additional tags.
    /// \param fn function that is called by gauge every time the value requested. Must be thread
    ///     safe if planned to use in multi-threaded environment.
    template<typename R>
    auto
    register_gauge(std::string name, tags_t::container_type tags, std::function<R()> fn) ->
        shared_metric<metrics::gauge<R>>;

    /// Returns a gauge shared metric that is mapped to a given tags.
    ///
    /// \throws std::out_of_range if a gauge with either given name or type T does not exist.
    ///
    /// \param name gauge name.
    /// \param tags optional additional tags.
    /// \tparam `T` must be either std::uint64_t, std::int64_t or double.
    template<typename T>
    auto
    gauge(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        shared_metric<gauge<T>>;

    template<typename T>
    auto
    gauges() const -> metric_set<metrics::gauge<T>>;

    template<typename T>
    auto
    gauges(const query_t& query) const -> metric_set<metrics::gauge<T>>;

    /// Returns the сounter registered under this name and tags; or create and register a new
    /// counter if none is registered.
    ///
    /// \param name Counter name.
    /// \param tags Optional additional tags.
    /// \tparam `T` must be either std::int64_t or std::uint64_t.
    template<typename T>
    auto
    counter(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        shared_metric<std::atomic<T>>;

    template<typename T>
    auto
    counters() const -> metric_set<std::atomic<T>>;

    template<typename T>
    auto
    counters(const query_t& query) const -> metric_set<std::atomic<T>>;

    /// Returns a meter shared metric that is mapped to a given tags, performing a creation with
    /// registering if such metric does not already exist.
    ///
    /// \param name meter name.
    /// \param tags optional additional tags.
    auto
    meter(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        shared_metric<meter_t>;

    auto
    meters() const -> metric_set<meter_t>;

    auto
    meters(const query_t& query) const -> metric_set<meter_t>;

    /// Returns a timer shared metric that is mapped to a given tags, performing a creation with
    /// registering if such metric does not already exist.
    ///
    /// \param name timer name.
    /// \param tags optional additional tags.
    /// \tparam Accumulate must meet Accumulate requirements.
    /// \todo change default accumulator to `exponentially_decaying_t`.
    template<class Accumulate = accumulator::sliding::window_t>
    auto
    timer(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        shared_metric<timer<Accumulate>>;

    template<class Accumulate = accumulator::sliding::window_t>
    auto
    timers() const -> metric_set<metrics::timer<Accumulate>>;

    template<class Accumulate = accumulator::sliding::window_t>
    auto
    timers(const query_t& query) const -> metric_set<metrics::timer<Accumulate>>;

    auto
    select() const ->
        std::vector<std::shared_ptr<tagged_t>>;

    auto
    select(const query_t& query) const ->
        std::vector<std::shared_ptr<tagged_t>>;

    /// Removes the metric with the given name.
    ///
    /// \param name The name of the metric.
    /// \param tags Additional tags.
    ///
    /// \return Whether or not the metric was removed.
    template<typename T>
    auto
    remove(const std::string& name, const tags_t::container_type& tags) -> bool;
};

extern template auto registry_t::remove<gauge<std::int64_t>>(const std::string& name, const tags_t::container_type& tags) -> bool;
extern template auto registry_t::remove<gauge<std::uint64_t>>(const std::string& name, const tags_t::container_type& tags) -> bool;
extern template auto registry_t::remove<gauge<std::double_t>>(const std::string& name, const tags_t::container_type& tags) -> bool;
extern template auto registry_t::remove<std::atomic<std::int64_t>>(const std::string& name, const tags_t::container_type& tags) -> bool;
extern template auto registry_t::remove<std::atomic<std::uint64_t>>(const std::string& name, const tags_t::container_type& tags) -> bool;
extern template auto registry_t::remove<meter_t>(const std::string& name, const tags_t::container_type& tags) -> bool;
extern template auto registry_t::remove<timer<accumulator::sliding::window_t>>(const std::string& name, const tags_t::container_type& tags) -> bool;
extern template auto registry_t::remove<timer<accumulator::decaying::exponentially_t>>(const std::string& name, const tags_t::container_type& tags) -> bool;

}  // namespace metrics

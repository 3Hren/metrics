#include "metrics/registry.hpp"

#include <future>

#include "metrics/meter.hpp"
#include "metrics/metric.hpp"
// #include "metrics/timer.hpp"

#include "metrics/detail/registry.hpp"

namespace metrics {

registry_t::registry_t():
    inner(new inner_t)
{}

registry_t::~registry_t() = default;

template<typename T>
auto registry_t::counter(std::string name, tags_t::container_type other) const ->
    shared_metric<std::atomic<T>>
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->counters.mutex);
    auto& instances = inner->counters.template get<T>();

    std::shared_ptr<std::atomic<T>> instance;
    if((instance = instances[tags].lock()) == nullptr) {
        instance = std::make_shared<std::atomic<T>>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

auto registry_t::meter(std::string name, tags_t::container_type other) const ->
    shared_metric<meter_t>
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->meters.mutex);
    auto& instances = inner->meters.template get<detail::meter_t>();

    std::shared_ptr<detail::meter_t> instance;
    if((instance = instances[tags].lock()) == nullptr) {
        instance = std::make_shared<detail::meter_t>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

template<class Accumulate>
auto registry_t::timer(std::string name, tags_t::container_type other) const ->
    shared_metric<metrics::timer<Accumulate>>
{
    typedef std::chrono::high_resolution_clock clock_type;
    typedef detail::timer<
        clock_type,
        detail::meter<clock_type>,
        detail::histogram<Accumulate>
    > result_type;

    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->timers.mutex);
    auto& instances = inner->timers.template get<Accumulate>();

    std::shared_ptr<result_type> instance;
    if((instance = instances[tags].lock()) == nullptr) {
        instance = std::make_shared<result_type>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

/// Instantiations.

template
auto registry_t::counter<std::int64_t>(std::string, tags_t::container_type) const ->
    shared_metric<std::atomic<std::int64_t>>;

template
auto registry_t::counter<std::uint64_t>(std::string, tags_t::container_type) const ->
    shared_metric<std::atomic<std::uint64_t>>;

template
auto registry_t::timer<accumulator::sliding::window_t>(std::string, tags_t::container_type tags) const ->
    shared_metric<metrics::timer<accumulator::sliding::window_t>>;

}  // namespace metrics

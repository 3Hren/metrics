#include "metrics/registry.hpp"

#include <future>

#include "metrics/gauge.hpp"
#include "metrics/meter.hpp"
#include "metrics/metric.hpp"
// #include "metrics/timer.hpp"

#include "registry.hpp"

namespace metrics {

registry_t::registry_t():
    inner(new inner_t)
{}

registry_t::~registry_t() = default;

template<typename R>
auto registry_t::register_gauge(std::string name, tags_t::container_type other, std::function<R()> fn) ->
    void
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->gauges.mutex);
    auto& instances = inner->gauges.template get<R>();

    std::shared_ptr<metrics::gauge<R>> instance;
    if((instance = instances[tags]) == nullptr) {
        instance = std::make_shared<metrics::gauge<R>>(std::move(fn));
        instances[tags] = instance;
    }
}

template<typename T>
auto registry_t::gauge(std::string name, tags_t::container_type other) const
    -> shared_metric<metrics::gauge<T>>
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->gauges.mutex);
    const auto& instances = inner->gauges.template get<T>();
    return {tags, instances.at(tags)};
}

template<typename T>
auto registry_t::gauges() const -> std::map<tags_t, shared_metric<metrics::gauge<T>>> {
    std::map<tags_t, shared_metric<metrics::gauge<T>>> result;

    std::lock_guard<std::mutex> lock(inner->gauges.mutex);
    const auto& instances = inner->gauges.template get<T>();

    for (const auto& item : instances) {
        const auto& tags = std::get<0>(item);
        if (auto instance = std::get<1>(item)) {
            result.insert(std::make_pair(tags, shared_metric<metrics::gauge<T>>(tags, instance)));
        }
    }

    return result;
}

template<typename T>
auto registry_t::counter(std::string name, tags_t::container_type other) const ->
    shared_metric<std::atomic<T>>
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->counters.mutex);
    auto& instances = inner->counters.template get<T>();

    std::shared_ptr<std::atomic<T>> instance;
    if((instance = instances[tags]) == nullptr) {
        instance = std::make_shared<std::atomic<T>>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

template<typename T>
auto registry_t::counters() const -> std::map<tags_t, shared_metric<std::atomic<T>>> {
    std::map<tags_t, shared_metric<std::atomic<T>>> result;

    std::lock_guard<std::mutex> lock(inner->counters.mutex);
    const auto& instances = inner->counters.template get<T>();

    for (const auto& item : instances) {
        const auto& tags = std::get<0>(item);
        if (auto instance = std::get<1>(item)) {
            result.insert(std::make_pair(tags, shared_metric<std::atomic<T>>(tags, instance)));
        }
    }

    return result;
}

auto registry_t::meter(std::string name, tags_t::container_type other) const ->
    shared_metric<meter_t>
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->meters.mutex);
    auto& instances = inner->meters.get<detail::meter_t>();

    std::shared_ptr<detail::meter_t> instance;
    if((instance = instances[tags]) == nullptr) {
        instance = std::make_shared<detail::meter_t>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

auto registry_t::meters() const -> std::map<tags_t, shared_metric<meter_t>> {
    std::map<tags_t, shared_metric<meter_t>> result;

    std::lock_guard<std::mutex> lock(inner->meters.mutex);
    const auto& instances = inner->meters.get<detail::meter_t>();

    for (const auto& item : instances) {
        const auto& tags = std::get<0>(item);
        if (auto instance = std::get<1>(item)) {
            result.insert(std::make_pair(tags, shared_metric<meter_t>(tags, instance)));
        }
    }

    return result;
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
    if((instance = instances[tags]) == nullptr) {
        instance = std::make_shared<result_type>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

template<class Accumulate>
auto registry_t::timers() const -> std::map<tags_t, shared_metric<metrics::timer<Accumulate>>> {
    std::map<tags_t, shared_metric<metrics::timer<Accumulate>>> result;

    std::lock_guard<std::mutex> lock(inner->timers.mutex);
    const auto& instances = inner->timers.template get<Accumulate>();

    for (const auto& item : instances) {
        const auto& tags = std::get<0>(item);
        if (auto instance = std::get<1>(item)) {
            result.insert(std::make_pair(tags, shared_metric<metrics::timer<Accumulate>>(tags, instance)));
        }
    }

    return result;
}

/// Instantiations.

template
auto registry_t::register_gauge<std::int64_t>(std::string, tags_t::container_type other, std::function<std::int64_t()> fn) ->
    void;

template
auto registry_t::register_gauge<std::uint64_t>(std::string, tags_t::container_type other, std::function<std::uint64_t()> fn) ->
    void;

template
auto registry_t::register_gauge<double>(std::string, tags_t::container_type other, std::function<double()> fn) ->
    void;

template
auto registry_t::gauge<std::int64_t>(std::string, tags_t::container_type) const ->
    shared_metric<metrics::gauge<std::int64_t>>;

template
auto registry_t::gauge<std::uint64_t>(std::string, tags_t::container_type) const ->
    shared_metric<metrics::gauge<std::uint64_t>>;

template
auto registry_t::gauge<double>(std::string, tags_t::container_type) const ->
    shared_metric<metrics::gauge<double>>;

template
auto registry_t::gauges<std::int64_t>() const ->
    std::map<tags_t, shared_metric<metrics::gauge<std::int64_t>>>;

template
auto registry_t::gauges<std::uint64_t>() const ->
    std::map<tags_t, shared_metric<metrics::gauge<std::uint64_t>>>;

template
auto registry_t::gauges<double>() const ->
    std::map<tags_t, shared_metric<metrics::gauge<double>>>;

template
auto registry_t::counter<std::int64_t>(std::string, tags_t::container_type) const ->
    shared_metric<std::atomic<std::int64_t>>;

template
auto registry_t::counter<std::uint64_t>(std::string, tags_t::container_type) const ->
    shared_metric<std::atomic<std::uint64_t>>;

template
auto registry_t::counters<std::int64_t>() const ->
    std::map<tags_t, shared_metric<std::atomic<std::int64_t>>>;

template
auto registry_t::timer<accumulator::sliding::window_t>(std::string, tags_t::container_type tags) const ->
    shared_metric<metrics::timer<accumulator::sliding::window_t>>;

template
auto registry_t::timers<accumulator::sliding::window_t>() const ->
    std::map<tags_t, shared_metric<metrics::timer<accumulator::sliding::window_t>>>;

}  // namespace metrics

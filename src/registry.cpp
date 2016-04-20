#include "metrics/registry.hpp"

#include <future>

#include "metrics/meter.hpp"
#include "metrics/metric.hpp"
// #include "metrics/timer.hpp"

#include "metrics/detail/processor.hpp"

namespace metrics {

registry_t::registry_t():
    processor(new processor_t)
{}

registry_t::~registry_t() = default;

template<typename T>
auto registry_t::counter(std::string name, tags_t::container_type other) const ->
    shared_metric<std::atomic<T>>
{
    tags_t tags(std::move(name), std::move(other));

    auto counter = processor->post([&]() -> std::shared_ptr<std::atomic<T>> {
        return processor->counter<T>(tags);
    }).get();

    return {std::move(tags), std::move(counter)};
}

auto registry_t::meter(std::string name, tags_t::container_type other) const ->
    shared_metric<meter_t>
{
    tags_t tags(std::move(name), std::move(other));

    auto meter = processor->post([&]() -> std::shared_ptr<meter_t> {
        return processor->meter(tags);
    }).get();

    return {std::move(tags), std::move(meter)};
}

template<class Accumulate>
auto registry_t::timer(std::string name, tags_t::container_type other) const ->
    shared_metric<metrics::timer<Accumulate>>
{
    tags_t tags(std::move(name), std::move(other));

    auto meter = processor->post([&]() -> std::shared_ptr<metrics::timer<Accumulate>> {
        return processor->timer<Accumulate>(tags);
    }).get();

    return {std::move(tags), std::move(meter)};
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

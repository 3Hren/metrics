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
    metric<std::atomic<T>>
{
    tags_t tags(name, other);

    auto meter = processor->post([&]() -> std::shared_ptr<std::atomic<T>> {
        return processor->counter<T>(tags);
    }).get();

    return {std::move(tags), std::move(meter)};
}

auto registry_t::meter(std::string name, tags_t::container_type tags_) const -> metric<meter_t> {
    tags_t tags(name, tags_);
    auto meter = processor->meter(tags);

    return {std::move(tags), std::move(meter)};
}

// template<class Accumulate>
// timer<Accumulate>
// registry_t::timer(std::string name, tags_t::container_type tags) const {
//     return metrics::timer<Accumulate>({std::move(name), std::move(tags)}, *processor);
// }

/// Instantiations.

template
auto registry_t::counter<std::int64_t>(std::string, tags_t::container_type) const ->
    metric<std::atomic<std::int64_t>>;

template
auto registry_t::counter<std::uint64_t>(std::string, tags_t::container_type) const ->
    metric<std::atomic<std::uint64_t>>;

// template
// timer<accumulator::sliding::window_t>
// registry_t::timer<accumulator::sliding::window_t>(std::string, tags_t::container_type tags) const;

}  // namespace metrics

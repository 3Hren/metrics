#include "metrics/registry.hpp"

#include "metrics/counter.hpp"
#include "metrics/gauge.hpp"
#include "metrics/meter.hpp"
#include "metrics/timer.hpp"

#include "metrics/detail/processor.hpp"

namespace metrics {

template<class M>
struct metric_traits;

template<typename T>
struct metric_traits<gauge<T>> {
    static void
    apply(processor_t& processor, std::string name, tagged_t::container_type tags, std::function<T()> metric) {
        processor.post([&, name, tags, metric] {
            processor.gauges<T>().insert({{std::move(name), std::move(tags)}, std::move(metric)});
        });
    }
};

registry_t::registry_t():
    processor(new processor_t)
{}

registry_t::~registry_t() {}

template<typename M>
void
registry_t::listen(std::string name, typename M::function_type metric) {
    metric_traits<M>::apply(*processor, std::move(name), {}, std::move(metric));
}

template<typename M>
void
registry_t::listen(std::string name, tagged_t::container_type tags, typename M::function_type metric) {
    metric_traits<M>::apply(*processor, std::move(name), std::move(tags), std::move(metric));
}

template<typename T>
gauge<T>
registry_t::gauge(std::string name, tagged_t::container_type tags) const {
    return metrics::gauge<T>({std::move(name), std::move(tags)}, *processor);
}

template<typename T>
counter<T>
registry_t::counter(const std::string& name) const {
    return metrics::counter<T>({name}, *processor);
}

template<typename T>
counter<T>
registry_t::counter(std::string name, tagged_t::container_type tags) const {
    return metrics::counter<T>({std::move(name), std::move(tags)}, *processor);
}

meter_t
registry_t::meter(std::string name, tagged_t::container_type tags) const {
    return meter_t({std::move(name), std::move(tags)}, *processor);
}

template<class Accumulate>
timer<Accumulate>
registry_t::timer(const std::string& name) const {
    return metrics::timer<Accumulate>({name}, *processor);
}

/// Instantiations.
template
void
registry_t::listen<gauge<std::uint64_t>>(std::string, std::function<std::uint64_t()>);

template
void
registry_t::listen<gauge<std::uint64_t>>(std::string, tagged_t::container_type, std::function<std::uint64_t()>);

template
gauge<std::uint64_t>
registry_t::gauge<std::uint64_t>(std::string, tagged_t::container_type) const;

template
counter<std::int64_t>
registry_t::counter<std::int64_t>(const std::string&) const;

template
counter<std::uint64_t>
registry_t::counter<std::uint64_t>(const std::string&) const;

template
counter<std::int64_t>
registry_t::counter<std::int64_t>(std::string, tagged_t::container_type) const;

template
counter<std::uint64_t>
registry_t::counter<std::uint64_t>(std::string, tagged_t::container_type) const;

template
timer<accumulator::sliding::window_t>
registry_t::timer<accumulator::sliding::window_t>(const std::string&) const;

}  // namespace metrics

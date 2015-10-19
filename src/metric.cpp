#include "metrics/metric.hpp"

namespace metrics {

metric_t::metric_t(std::string name, processor_t& processor):
    processor(processor)
{
    d.name = std::move(name);
}

const std::string&
metric_t::name() const {
    return d.name;
}

}  // namespace metrics

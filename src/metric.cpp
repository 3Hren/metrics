#include "metrics/metric.hpp"

#include <memory>

#include <boost/optional/optional.hpp>

namespace metrics {

metric_t::data_t::data_t(tagged_t tagged):
    tagged(std::move(tagged))
{}

metric_t::metric_t(tagged_t tagged, processor_t& processor):
    d(std::move(tagged)),
    processor(std::addressof(processor))
{}

const std::string&
metric_t::name() const {
    return d.tagged.name();
}

boost::optional<std::string>
metric_t::tag(const std::string& key) const {
    return d.tagged.tag(key);
}

}  // namespace metrics

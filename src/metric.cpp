#include "metrics/metric.hpp"

#include <memory>

#include <boost/optional/optional.hpp>

namespace metrics {

metric_t::data_t::data_t(tags_t tags):
    tags(std::move(tags))
{}

metric_t::metric_t(tags_t tags, processor_t& processor):
    d(std::move(tags)),
    processor(std::addressof(processor))
{}

const std::string&
metric_t::name() const noexcept {
    return d.tags.name();
}

const tags_t&
metric_t::tags() const noexcept {
    return d.tags;
}

boost::optional<std::string>
metric_t::tag(const std::string& key) const {
    return d.tags.tag(key);
}

}  // namespace metrics

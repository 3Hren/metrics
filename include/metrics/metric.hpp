#pragma once

#include <string>

#include "metrics/tags.hpp"

namespace metrics {
namespace detail {

class processor_t;

}  // namespace detail
}  // namespace metrics

namespace metrics {

using detail::processor_t;

class metric_t {
    struct data_t {
        tags_t tags;

        data_t(tags_t tags);
    } d;

protected:
    processor_t* processor;

public:
    metric_t(tags_t tags, processor_t& processor);

    const std::string&
    name() const noexcept;

    const tags_t&
    tags() const noexcept;

    boost::optional<std::string>
    tag(const std::string& key) const;
};

}  // namespace metrics

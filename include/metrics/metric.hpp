#pragma once

#include <string>

#include "metrics/tagged.hpp"

namespace metrics {
namespace detail {

class processor_t;

}  // namespace detail
}  // namespace metrics

namespace metrics {

using detail::processor_t;

class metric_t {
    struct data_t {
        tagged_t tagged;

        data_t(tagged_t tagged);
    } d;

protected:
    processor_t* processor;

public:
    metric_t(tagged_t tagged, processor_t& processor);

    const std::string&
    name() const;

    boost::optional<std::string>
    tag(const std::string& key) const;
};

}  // namespace metrics

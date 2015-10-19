#pragma once

#include <string>

namespace metrics {
namespace detail {
namespace metric {

class processor_t;

}  // namespace metric
}  // namespace detail
}  // namespace metrics

namespace metrics {
namespace metric {

using detail::metric::processor_t;

class metric_t {
    struct {
        std::string name;
    } d;

protected:
    processor_t& processor;

public:
    metric_t(std::string name, processor_t& processor);

    const std::string&
    name() const;
};

}  // namespace metric
}  // namespace metrics

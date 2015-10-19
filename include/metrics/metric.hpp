#pragma once

#include <string>

namespace metrics {
namespace detail {

class processor_t;

}  // namespace detail
}  // namespace metrics

namespace metrics {

using detail::processor_t;

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

}  // namespace metrics

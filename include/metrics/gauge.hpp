#pragma once

#include <functional>

#include "metrics/metric.hpp"

namespace metrics {

/// A gauge is the simplest metric type. It just returns a value.
///
/// If, for example, your application has a value which is maintained by a third-party library, you
/// can easily expose it by registering a `gauge` instance which returns that value.
template<typename T>
class gauge : public metric_t {
public:
    typedef T value_type;
    typedef std::function<value_type()> function_type;

public:
    gauge(tags_t tags, processor_t& processor);

    value_type
    get() const;
};

}  // namespace metrics

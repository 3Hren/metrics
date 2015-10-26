#pragma once

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

public:
    gauge(tagged_t tagged, processor_t& processor);

    value_type
    get() const;
};

}  // namespace metrics

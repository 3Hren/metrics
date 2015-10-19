#pragma once

#include <future>

#include "metrics/metric.hpp"

namespace metrics {

/// An incrementing and decrementing counter metric.
template<typename T>
class counter : public metric_t {
public:
    typedef T value_type;

public:
    counter(std::string name, processor_t& processor);

    /// Returns the counter's current value.
    value_type
    get() const;

    /// Increment the counter by `value`.
    ///
    /// Returns a future with the previous value which will be ready right after incrementing
    /// operation completes.
    ///
    /// \param `value` - the amount by which the counter will be increased.
    std::future<value_type>
    inc(value_type value = 1);

    /// Decrement the counter by `value`.
    ///
    /// Returns a future with the previous value which will be ready right after decrementing
    /// operation completes.
    ///
    /// \param `value` - the amount by which the counter will be decreased.
    std::future<value_type>
    dec(value_type value = 1);
};

}  // namespace metrics

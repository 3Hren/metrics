#pragma once

#include <future>

#include "metrics/metric.hpp"

namespace metrics {

/// An incrementing and decrementing counter metric.
template<typename T>
class counter : public metric_t {
public:
    typedef T value_type;

    std::shared_ptr<std::atomic<value_type>> value;

public:
    counter(tagged_t tagged, processor_t& processor);

    /// Returns the counter's current value.
    auto get() const -> value_type;

    /// Increment the counter by `value`.
    ///
    /// Returns the previous value which will be ready right after incrementing operation completes.
    ///
    /// \param `value` - the amount by which the counter will be increased.
    auto inc(value_type value = 1) -> value_type;

    /// Decrement the counter by `value`.
    ///
    /// Returns the previous value which will be ready right after decrementing operation completes.
    ///
    /// \param `value` - the amount by which the counter will be decreased.
    auto dec(value_type value = 1) -> value_type;
};

}  // namespace metrics

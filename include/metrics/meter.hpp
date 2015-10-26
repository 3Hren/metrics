#pragma once

#include "metrics/metric.hpp"

namespace metrics {

/// A `meter` measures the rate at which a set of events occur.
///
/// Just like the Unix load averages visible in `top`.
class meter_t : public metric_t {
public:
    meter_t(tagged_t tagged, processor_t& processor);

    std::uint64_t
    count() const;

    /// Mark the occurrence of an event.
    void
    mark();
};

}  // namespace metrics

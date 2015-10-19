#pragma once

#include "metrics/metric/metric.hpp"

namespace metrics {
namespace metric {

/// A `meter` measures the rate at which a set of events occur.
///
/// Just like the Unix load averages visible in `top`.
class meter_t : public metric_t {
public:
    meter_t(std::string name, processor_t& processor);

    std::uint64_t
    count() const;

    /// Mark the occurrence of an event.
    void
    mark();
};

}  // namespace metric
}  // namespace metrics

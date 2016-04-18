#pragma once

#include <memory>

namespace metrics {

class meter_t;

/// Represents a factory for standalone non-tagged isolated metrics.
class factory_t {
public:
    /// Creates default meter implementation.
    auto meter() const -> std::unique_ptr<meter_t>;
};

}  // namespace metrics

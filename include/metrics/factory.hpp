#pragma once

#include <memory>

#include "forwards.hpp"

namespace metrics {

/// Represents a factory for standalone non-tagged isolated metrics.
class factory_t {
public:
    /// Creates default meter implementation.
    auto meter() const -> std::unique_ptr<meter_t>;

    /// Creates default timer implementation using the specified accumulator type.
    ///
    /// \todo not implemented yet.
    template<typename Accumulate>
    auto timer() const -> std::unique_ptr<timer<Accumulate>>;
};

}  // namespace metrics

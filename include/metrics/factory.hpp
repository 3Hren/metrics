#pragma once

#include <memory>

namespace metrics {

class meter_t;

class factory_t {
public:
    /// Creates default meter implementation.
    auto meter() const -> std::unique_ptr<meter_t>;
};

}  // namespace metrics

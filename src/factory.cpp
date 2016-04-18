#include "metrics/factory.hpp"

#include "metrics/detail/meter.hpp"

namespace metrics {

auto factory_t::meter() const -> std::unique_ptr<meter_t> {
    return std::unique_ptr<meter_t>(new detail::meter_t());
}

}  // namespace metrics

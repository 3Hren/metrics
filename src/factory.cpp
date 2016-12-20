#include "metrics/factory.hpp"

#include "metrics/accumulator/sliding/window.hpp"

#include "histogram.hpp"
#include "meter.hpp"
#include "timer.hpp"

namespace metrics {

auto factory_t::meter() const -> std::unique_ptr<meter_t> {
    return std::unique_ptr<meter_t>(new detail::meter_t);
}

template<typename Accumulate>
auto factory_t::timer() const -> std::unique_ptr<metrics::timer<Accumulate>> {
    typedef std::chrono::high_resolution_clock clock_type;
    typedef detail::meter<clock_type> meter_type;
    typedef detail::histogram<Accumulate> histogram_type;

    return std::unique_ptr<metrics::timer<Accumulate>>(
        new detail::timer<clock_type, meter_type, histogram_type>()
    );
}

/// Instantiations.
template
auto factory_t::timer<accumulator::sliding::window_t>() const ->
    std::unique_ptr<metrics::timer<accumulator::sliding::window_t>>;

}  // namespace metrics

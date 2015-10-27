#include "metrics/meter.hpp"

#include <future>

#include "metrics/detail/processor.hpp"

namespace metrics {

meter_t::meter_t(tagged_t tagged, processor_t& processor) :
    metric_t(std::move(tagged), processor)
{
    // Initialize the meter.
    processor.post([&] {
        processor.meter(this->tagged());
    });
}

std::uint64_t
meter_t::count() const {
    auto rx = processor->post([&]() -> std::uint64_t {
        return processor->meter(tagged()).count();
    });

    return rx.get();
}

void
meter_t::mark() {
    processor->post([&] {
        processor->meter(tagged()).mark();
    });
}

}  // namespace metrics

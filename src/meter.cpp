#include "metrics/meter.hpp"

#include <future>

#include "metrics/detail/processor.hpp"

namespace metrics {

meter_t::meter_t(std::string name, processor_t& processor) :
    metric_t(std::move(name), processor)
{
    processor.post([&] {
        processor.meter(this->name());
    });
}

std::uint64_t
meter_t::count() const {
    std::promise<std::uint64_t> tx;
    auto rx = tx.get_future();

    processor.post([&] {
        tx.set_value(processor.meter(name()).count());
    });

    return rx.get();
}

void
meter_t::mark() {
    processor.post([&] {
        processor.meter(name()).mark();
    });
}

}  // namespace metrics

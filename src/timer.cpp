#include "metrics/timer.hpp"

#include "metrics/accumulator/sliding/window.hpp"

#include "metrics/detail/processor.hpp"

namespace metrics {

template<class Accumulate>
timer<Accumulate>::context_t::context_t(timer* parent) :
    parent(parent),
    timestamp(clock_type::now())
{}

template<class Accumulate>
timer<Accumulate>::context_t::~context_t() {
    // TODO: Check pointer invalidation.
    const auto now = clock_type::now();
    const auto elapsed = now - timestamp;

    parent->processor->post([=] {
        parent->processor->timer<Accumulate>(parent->name()).update(elapsed);
    });
}

template<class Accumulate>
timer<Accumulate>::timer(std::string name, processor_t& processor) :
    metric_t(std::move(name), processor)
{}

template<class Accumulate>
std::uint64_t
timer<Accumulate>::count() const {
    std::promise<std::uint64_t> tx;
    auto rx = tx.get_future();

    processor->post([&] {
        tx.set_value(processor->timer<Accumulate>(name()).count());
    });

    return rx.get();
}

template<class Accumulate>
typename timer<Accumulate>::context_type
timer<Accumulate>::context() {
    return context_t(this);
}

/// Instantiations.
template class timer<accumulator::sliding::window_t>;

}  // namespace metrics

#include "metrics/timer.hpp"

#include "metrics/accumulator/sliding/window.hpp"
#include "metrics/accumulator/decaying/exponentially.hpp"

namespace metrics {

timer_t::context_t::context_t(timer_t* parent) :
    parent(parent, empty_deleter()),
    timestamp(parent->now())
{}

timer_t::context_t::~context_t() {
    if (parent == nullptr) {
        return;
    }

    const auto now = parent->now();
    const auto elapsed = now - timestamp;

    try {
        parent->update(elapsed);
    } catch (...) {
        // Ignore this, because of destructor exception safety reasons. Actually update
        // operation should never throw.
    }
}

auto timer_t::context_t::discard() -> void {
    parent.reset();
}

auto timer_t::context() -> context_t {
    return context_t(this);
}

timer_t::~timer_t() = default;

/// Instantiations.
template class timer<accumulator::sliding::window_t>;
template class timer<accumulator::decaying::exponentially_t>;

}  // namespace metrics

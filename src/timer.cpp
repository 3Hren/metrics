#include "metrics/timer.hpp"

#include "metrics/accumulator/sliding/window.hpp"

#include "metrics/detail/processor.hpp"

namespace metrics {

namespace {

// Workaround for GCC 4.6, which can't compile the similar lambda with the error: invalid use of
// ‘class metrics::timer<Accumulate>’.
template<typename Duration, typename Accumulate>
void
update(processor_t* processor, tags_t tags, Duration elapsed) {
    processor->timer<Accumulate>(std::move(tags)).update(elapsed);
}

}  // namespace

template<class Accumulate>
timer<Accumulate>::context_t::context_t(timer* parent) :
    parent(parent, [](timer*) {}),
    timestamp(clock_type::now())
{}

template<class Accumulate>
timer<Accumulate>::context_t::~context_t() {
    if (parent == nullptr) {
        return;
    }

    const auto now = clock_type::now();
    const auto elapsed = now - timestamp;

    const auto tags = parent->tags();
    const auto processor = parent->processor;

    processor->post(
        std::bind(&update<clock_type::duration, Accumulate>, processor, std::move(tags), elapsed)
    );
}

template<class Accumulate>
timer<Accumulate>::timer(tags_t tags, processor_t& processor) :
    metric_t(std::move(tags), processor)
{}

template<class Accumulate>
std::uint64_t
timer<Accumulate>::count() const {
    auto rx = processor->post([&]() -> std::uint64_t {
        return processor->timer<Accumulate>(tags()).count();
    });

    return rx.get();
}

template<class Accumulate>
double
timer<Accumulate>::m01rate() const {
    auto rx = processor->post([&]() -> double {
        return processor->timer<Accumulate>(tags()).m01rate();
    });

    return rx.get();
}

template<class Accumulate>
double
timer<Accumulate>::m05rate() const {
    auto rx = processor->post([&]() -> double {
        return processor->timer<Accumulate>(tags()).m05rate();
    });

    return rx.get();
}

template<class Accumulate>
double
timer<Accumulate>::m15rate() const {
    auto rx = processor->post([&]() -> double {
        return processor->timer<Accumulate>(tags()).m15rate();
    });

    return rx.get();
}

template<class Accumulate>
typename timer<Accumulate>::snapshot_type
timer<Accumulate>::snapshot() const {
    auto rx = processor->post([&]() -> typename timer<Accumulate>::snapshot_type {
        return processor->timer<Accumulate>(tags()).histogram().snapshot();
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

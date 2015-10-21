#include "metrics/timer.hpp"

#include "metrics/accumulator/sliding/window.hpp"

#include "metrics/detail/processor.hpp"

namespace metrics {

template<class Accumulate>
timer<Accumulate>::timer(std::string name, processor_t& processor) :
    metric_t(std::move(name), processor)
{}

template<class Accumulate>
std::uint64_t
timer<Accumulate>::count() const {
    return 0;
}

/// Instantiations.
template class timer<accumulator::sliding::window_t>;

}  // namespace metrics

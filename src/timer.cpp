#include "metrics/timer.hpp"

#include "metrics/accumulator/sliding/window.hpp"

namespace metrics {

/// Instantiations.
template class timer<accumulator::sliding::window_t>;

}  // namespace metrics

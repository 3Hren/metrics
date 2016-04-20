#pragma once

namespace metrics {

/// Standalone metrics.

template<typename T>
class gauge;

class meter_t;

template<class Accumulate>
class timer;

/// Accumulators and shapshots.

namespace accumulator {
namespace sliding {

class window_t;

}  // namespace sliding
}  // namespace accumulator

/// Metric wrappers.

template<typename T>
class tagged;

template<typename T>
class shared_metric;

namespace detail {

class processor_t;

}  // namespace detail
}  // namespace metrics

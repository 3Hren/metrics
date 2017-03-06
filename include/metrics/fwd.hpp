#pragma once

#include <functional>

namespace metrics {

/// Standalone metrics.

inline namespace v2 {

template <typename T>
using gauge = std::function<T()>;

class meter_t;
class visitor_t;

} // namespace v2

template <class Accumulate>
class timer;

/// Accumulators and shapshots.

namespace accumulator {
namespace sliding {

class window_t;

} // namespace sliding
namespace decaying {

class exponentially_t;

} // namespace decaying
} // namespace accumulator

/// Metric wrappers.

class tags_t;
class tagged_t;

template <typename T>
class shared_metric;

using query_t = std::function<bool(const tagged_t& metric)>;

} // namespace metrics

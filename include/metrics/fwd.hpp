#pragma once

#include <functional>

namespace metrics {

/// Standalone metrics.

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

class tags_t;
class tagged_t;

template<typename T>
class shared_metric;

using query_t = std::function<bool(const tags_t& tags)>;

inline namespace v2 {

class visitor_t;

}  // namespace v2
}  // namespace metrics

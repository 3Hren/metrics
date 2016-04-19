#pragma once

namespace metrics {

template<typename T>
class gauge;

class meter_t;

template<class Accumulate>
class timer;

namespace accumulator {
namespace sliding {

class window_t;

}  // namespace sliding
}  // namespace accumulator

template<typename T>
class metric;

namespace detail {

class processor_t;

}  // namespace detail
}  // namespace metrics

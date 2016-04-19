#pragma once

#include <functional>
#include <memory>
#include <string>

#include "metrics/metric.hpp"
#include "metrics/tags.hpp"

namespace metrics {
namespace detail {

class processor_t;

}  // namespace detail
}  // namespace metrics

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

using detail::processor_t;

class registry_t {
    std::unique_ptr<processor_t> processor;

public:
    registry_t();
    ~registry_t();

    /// Given a `metric` gauge, registers it under the given name.
    template<typename M>
    void
    listen(std::string name, typename M::function_type metric);

    /// Given a `metric` gauge, registers it under the given name with the given tags.
    template<typename M>
    void
    listen(std::string name, tags_t::container_type tags, typename M::function_type metric);

    /// \where `T` must be either `std::uint64_t`, `std::int64_t` or `long double`.
    template<typename T>
    metrics::gauge<T>
    gauge(std::string name, tags_t::container_type tags = tags_t::container_type()) const;

    /// Obtains by either creating or getting an existing object of a counter with the given name
    /// and returns it.
    ///
    /// \tparam `T` must be either `std::uint64_t` or `std::int64_t`.
    /// \todo wording.
    template<typename T>
    auto counter(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        metric<std::atomic<T>>;

    auto meter(std::string name, tags_t::container_type tags = tags_t::container_type()) const ->
        metric<meter_t>;

    // TODO: Change default accumulator to `exponentially_decaying_t`.
    template<class Accumulate = accumulator::sliding::window_t>
    timer<Accumulate>
    timer(std::string name, tags_t::container_type tags = tags_t::container_type()) const;
};

}  // namespace metrics

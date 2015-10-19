#pragma once

#include <functional>
#include <memory>
#include <string>

#include "metrics/metric/meter.hpp"

namespace metrics {
namespace detail {
namespace metric {

class processor_t;

}  // namespace metric
}  // namespace detail
}  // namespace metrics

namespace metrics {
namespace metric {

template<typename T>
class gauge;

template<typename T>
class counter;

using detail::metric::processor_t;

class registry_t {
    std::unique_ptr<processor_t> processor;

public:
    registry_t();
    ~registry_t();

    /// Given a `metric` gauge, registers it under the given name.
    template<typename M>
    void
    listen(std::string name, std::function<typename M::value_type()> metric);

    ///
    /// \where `T` must be either `std::uint64_t`, `std::int64_t` or `long double`.
    template<typename T>
    gauge<T>
    gauge(const std::string& name) const;

    /// \where `T` must be either `std::uint64_t` or `std::int64_t`.
    template<typename T>
    counter<T>
    counter(const std::string& name) const;

    meter_t
    meter(const std::string& name) const;
};

}  // namespace meter
}  // namespace metrics

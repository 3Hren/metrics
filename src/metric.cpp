#include "metrics/metric.hpp"

#include <atomic>

#include <boost/optional/optional.hpp>

#include <metrics/meter.hpp>
#include <metrics/timer.hpp>

namespace metrics {

template<typename T>
tagged<T>::tagged(tags_t value) :
    value(std::move(value))
{}

template<typename T>
auto tagged<T>::tags() const noexcept -> const tags_t& {
    return value;
}

template<typename T>
auto tagged<T>::name() const noexcept -> const std::string& {
    return tags().name();
}

template<typename T>
auto tagged<T>::tag(const std::string& key) const -> boost::optional<std::string> {
    return tags().tag(key);
}

template<typename T>
shared_metric<T>::shared_metric(tags_t tags, std::shared_ptr<T> inner) :
    tagged<T>(std::move(tags)),
    inner(std::move(inner))
{}

template<typename T>
auto shared_metric<T>::operator->() const -> T* {
    return inner.get();
}

template<typename T>
auto shared_metric<T>::get() const -> std::shared_ptr<T> {
    return inner;
}

/// Instantiations.

template class tagged<std::atomic<std::int64_t>>;
template class tagged<std::atomic<std::uint64_t>>;
template class tagged<meter_t>;
template class tagged<timer<accumulator::sliding::window_t>>;

template class shared_metric<std::atomic<std::int64_t>>;
template class shared_metric<std::atomic<std::uint64_t>>;
template class shared_metric<meter_t>;
template class shared_metric<timer<accumulator::sliding::window_t>>;

}  // namespace metrics

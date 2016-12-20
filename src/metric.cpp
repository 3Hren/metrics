#include "metrics/metric.hpp"

#include <atomic>

#include <boost/optional/optional.hpp>

#include "metrics/accumulator/sliding/window.hpp"
#include "metrics/gauge.hpp"
#include "metrics/meter.hpp"
#include "metrics/timer.hpp"
#include "metrics/visitor.hpp"

namespace metrics {

tagged_t::tagged_t(tags_t value) :
    value(std::move(value))
{}

auto tagged_t::tags() const noexcept -> const tags_t& {
    return value;
}

auto tagged_t::name() const -> std::string {
    return tags().name();
}

auto tagged_t::tag(const std::string& key) const -> boost::optional<const std::string&> {
    return tags().tag(key);
}

template<typename T>
shared_metric<T>::shared_metric(tags_t tags, std::shared_ptr<T> inner) :
    tagged_t(std::move(tags)),
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

template<typename T>
auto shared_metric<T>::apply(visitor_t& visitor) const -> void {
    visitor.visit(*inner);
}

/// Instantiations.

template class shared_metric<gauge<std::int64_t>>;
template class shared_metric<gauge<std::uint64_t>>;
template class shared_metric<gauge<std::double_t>>;
template class shared_metric<std::atomic<std::int64_t>>;
template class shared_metric<std::atomic<std::uint64_t>>;
template class shared_metric<meter_t>;
template class shared_metric<timer<accumulator::sliding::window_t>>;

}  // namespace metrics

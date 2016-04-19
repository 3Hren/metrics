#include "metrics/metric.hpp"

#include <boost/optional/optional.hpp>

#include <metrics/meter.hpp>

namespace metrics {

template<typename T>
metric<T>::data_t::data_t(tags_t tags, std::shared_ptr<T> inner) :
    tags(std::move(tags)),
    inner(std::move(inner))
{}

template<typename T>
metric<T>::metric(tags_t tags, std::shared_ptr<T> inner) :
    d(std::move(tags), std::move(inner))
{}

template<typename T>
auto metric<T>::tags() const noexcept -> const tags_t& {
    return d.tags;
}

template<typename T>
auto metric<T>::name() const noexcept -> const std::string& {
    return d.tags.name();
}

template<typename T>
auto metric<T>::tag(const std::string& key) const -> boost::optional<std::string> {
    return d.tags.tag(key);
}

template<typename T>
auto metric<T>::operator->() const -> T* {
    return d.inner.get();
}

template<typename T>
auto metric<T>::get() const -> std::shared_ptr<T> {
    return d.inner;
}

template class metric<std::atomic<std::int64_t>>;
template class metric<std::atomic<std::uint64_t>>;
template class metric<meter_t>;

}  // namespace metrics

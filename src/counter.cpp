#include "metrics/counter.hpp"

#include <future>

#include "metrics/detail/cpp14/utility.hpp"
#include "metrics/detail/processor.hpp"

namespace metrics {

template<typename T>
counter<T>::counter(tags_t tags, processor_t& processor) :
    metric_t(std::move(tags), processor)
{
    auto rx = processor.post([&]() -> std::shared_ptr<std::atomic<T>> {
        return processor.counter<T>(this->tags());
    });

    value = rx.get();
}

template<typename T>
auto counter<T>::get() const -> T {
    return value->load();
}

template<typename T>
auto counter<T>::inc(value_type v) -> T {
    return value->fetch_add(v);
}

template<typename T>
auto counter<T>::dec(value_type value) -> T {
    return inc(-value);
}

/// Instantiations.
template class counter<std::int64_t>;
template class counter<std::uint64_t>;

}  // namespace metrics

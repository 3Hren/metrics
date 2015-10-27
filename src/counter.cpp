#include "metrics/counter.hpp"

#include <future>

#include "metrics/detail/cpp14/utility.hpp"
#include "metrics/detail/processor.hpp"

namespace metrics {

template<typename T>
counter<T>::counter(tagged_t tagged, processor_t& processor) :
    metric_t(std::move(tagged), processor)
{}

template<typename T>
T
counter<T>::get() const {
    auto rx = processor->post([&]() -> T {
        return processor->counter<T>(tagged());
    });

    return rx.get();
}

template<typename T>
std::future<T>
counter<T>::inc(value_type value) {
    auto rx = processor->post([=]() -> T {
        auto& counter = processor->counter<T>(tagged());
        return std::exchange(counter, counter + value);
    });

    return rx;
}

template<typename T>
std::future<T>
counter<T>::dec(value_type value) {
    auto rx = processor->post([=]() -> T {
        auto& counter = processor->counter<T>(tagged());
        return std::exchange(counter, counter - value);
    });

    return rx;
}

/// Instantiations.
template class counter<std::int64_t>;
template class counter<std::uint64_t>;

}  // namespace metrics

#include "metrics/gauge.hpp"

#include <future>

#include "metrics/detail/processor.hpp"

namespace metrics {

template<typename T>
gauge<T>::gauge(tags_t tags, processor_t& processor) :
    metric_t(std::move(tags), processor)
{}

template<typename T>
T
gauge<T>::get() const {
    auto rx = processor->post([&]() -> T {
        if (auto fn = processor->gauge<T>(tags())) {
            return (*fn)();
        } else {
            throw std::out_of_range(name());
        }
    });

    return rx.get();
}

/// Instantiations.
template class gauge<std::uint64_t>;

}  // namespace metrics

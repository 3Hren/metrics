#include "metrics/gauge.hpp"

#include <future>

#include "metrics/detail/processor.hpp"

namespace metrics {

template<typename T>
gauge<T>::gauge(std::string name, processor_t& processor) :
    metric_t(std::move(name), processor)
{}

template<typename T>
T
gauge<T>::get() const {
    std::promise<T> tx;
    auto rx = tx.get_future();

    processor->post([&] {
        if (auto fn = processor->gauge<T>(name())) {
            tx.set_value((*fn)());
        } else {
            tx.set_exception(std::make_exception_ptr(std::out_of_range(name())));
        }
    });

    return rx.get();
}

/// Instantiations.
template class gauge<std::uint64_t>;

}  // namespace metrics

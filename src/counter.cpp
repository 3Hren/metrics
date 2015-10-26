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
    std::promise<T> tx;
    auto rx = tx.get_future();

    processor->post([&] {
        tx.set_value(processor->counter<T>(name()));
    });

    return rx.get();
}

template<typename T>
std::future<T>
counter<T>::inc(value_type value) {
    auto tx = std::make_shared<std::promise<T>>();
    auto rx = tx->get_future();

    processor->post([=] {
        auto& counter = processor->counter<T>(name());
        tx->set_value(std::exchange(counter, counter + value));
    });

    return rx;
}

template<typename T>
std::future<T>
counter<T>::dec(value_type value) {
    // TODO: Refactor.
    auto tx = std::make_shared<std::promise<T>>();
    auto rx = tx->get_future();

    processor->post([=] {
        auto& counter = processor->counter<T>(name());
        tx->set_value(std::exchange(counter, counter - value));
    });

    return rx;
}

/// Instantiations.
template class counter<std::int64_t>;
template class counter<std::uint64_t>;

}  // namespace metrics

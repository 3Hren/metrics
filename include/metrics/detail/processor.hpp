#pragma once

#include <cmath>
#include <map>
#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/optional/optional.hpp>

#include "metrics/accumulator/sliding/window.hpp"

#include "metrics/detail/cpp14/tuple.hpp"
#include "metrics/detail/ewma.hpp"
#include "metrics/detail/histogram.hpp"
#include "metrics/detail/meter.hpp"
#include "metrics/detail/timer.hpp"

namespace metrics {
namespace detail {

typedef tagged_t tagged_type;
typedef std::chrono::high_resolution_clock clock_type;

template<typename T>
struct map_gauge {
    typedef std::function<T()> type;
};

template<typename T>
struct map_counter {
    typedef T type;
};

template<typename T>
struct map_timer {
    typedef timer<clock_type, meter<clock_type>, histogram<T>> type;
};

/// Represents a tagged collection of metrics with various specializations.
template<template<typename> class Map, typename... U>
struct tagged_collection {
    std::tuple<std::map<tagged_type, typename Map<U>::type>...> containers;

    template<typename T>
    const std::map<tagged_type, typename Map<T>::type>&
    get() const noexcept {
        return cpp14::get<std::map<tagged_type, typename Map<T>::type>>(containers);
    }

    template<typename T>
    std::map<tagged_type, typename Map<T>::type>&
    get() noexcept {
        return cpp14::get<std::map<tagged_type, typename Map<T>::type>>(containers);
    }
};

/// \internal
class processor_t {
    boost::asio::io_service loop;
    boost::optional<boost::asio::io_service::work> work;

    std::thread thread;

    /// Data.
    struct {
        tagged_collection<map_gauge, std::uint64_t> gauges;
        tagged_collection<map_counter, std::int64_t, std::uint64_t> counters;
        std::map<tagged_type, meter_t> meters;
        tagged_collection<map_timer, accumulator::sliding::window_t> timers;
    } data;

public:
    processor_t():
        work(boost::optional<boost::asio::io_service::work>(boost::asio::io_service::work(loop))),
        thread([&] { loop.run(); })
    {}

    ~processor_t() {
        work.reset();
        thread.join();
    }

    /// Request the processor to invoke the given handler and return immediately.
    ///
    /// Returns a `std::future` associated with the promised result.
    ///
    /// \type `F` must meet `CopyConstructible` requirements.
    /// \note this method involves heap allocation to match I/O service requirements for completion
    ///       handler object, which must be `CopyConstructible`.
    template<class F>
    auto
    post(F fn) -> std::future<decltype(fn())> {
        typedef decltype(fn()) result_type;
        typedef std::packaged_task<result_type()> task_type;

        auto tx = std::make_shared<task_type>([=]() -> result_type {
            return fn();
        });

        loop.post(std::bind(&task_type::operator(), tx));

        return tx->get_future();
    }

    /// \warning must be called from event loop's thread.
    template<typename T>
    const std::map<tagged_type, std::function<T()>>&
    gauges() const {
        return data.gauges.get<T>();
    }

    /// \overload
    /// \warning must be called from event loop's thread.
    template<typename T>
    std::map<tagged_type, std::function<T()>>&
    gauges() {
        return data.gauges.get<T>();
    }

    /// \warning must be called from event loop's thread.
    template<typename T>
    const std::map<tagged_type, T>&
    counters() const {
        return data.counters.get<T>();
    }

    /// \overload
    /// \warning must be called from event loop's thread.
    template<typename T>
    std::map<tagged_type, T>&
    counters() {
        return data.counters.get<T>();
    }

    /// \warning must be called from event loop's thread.
    template<class Accumulate>
    const std::map<tagged_type, timer<clock_type, meter<clock_type>, histogram<Accumulate>>>&
    timers() const {
        return data.timers.get<Accumulate>();
    }

    /// \overload
    /// \warning must be called from event loop's thread.
    template<class Accumulate>
    std::map<tagged_type, timer<clock_type, meter<clock_type>, histogram<Accumulate>>>&
    timers() {
        return data.timers.get<Accumulate>();
    }

    /// \warning must be called from event loop's thread.
    template<typename T>
    boost::optional<std::function<T()>>
    gauge(const tagged_type& tagged) const {
        const auto& gauges = this->gauges<T>();
        const auto it = gauges.find(tagged);

        if (it != gauges.end()) {
            return boost::make_optional(it->second);
        } else {
            return boost::none;
        }
    }

    /// \warning must be called from event loop's thread.
    template<typename T>
    T&
    counter(const tagged_type& tagged) {
        return counters<T>()[tagged];
    }

    /// \warning must be called from event loop's thread.
    meter_t&
    meter(const tagged_type& tagged) {
        return data.meters[tagged];
    }

    /// \warning must be called from event loop's thread.
    template<typename Accumulate>
    timer<clock_type, metrics::detail::meter<clock_type>, histogram<Accumulate>>&
    timer(const tagged_type& tagged) {
        return timers<Accumulate>()[tagged];
    }
};

}  // namespace detail
}  // namespace metrics

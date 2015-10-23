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

typedef std::chrono::high_resolution_clock clock_type;

template<typename... U>
struct gauges_t {
    std::tuple<std::map<std::string, std::function<U()>>...> containers;

    template<typename T>
    const std::map<std::string, std::function<T()>>&
    get() const noexcept {
        return cpp14::get<std::map<std::string, std::function<T()>>>(containers);
    }

    template<typename T>
    std::map<std::string, std::function<T()>>&
    get() noexcept {
        return cpp14::get<std::map<std::string, std::function<T()>>>(containers);
    }
};

template<typename... U>
struct counters_t {
    std::tuple<std::map<std::string, U>...> containers;

    template<typename T>
    std::map<std::string, T>&
    get() noexcept {
        return cpp14::get<std::map<std::string, T>>(containers);
    }
};

template<typename... U>
struct timers_t {
    std::tuple<std::map<std::string, timer<clock_type, meter<clock_type>, histogram<U>>>...> containers;

    template<typename T>
    std::map<std::string, timer<clock_type, meter<clock_type>, histogram<T>>>&
    get() noexcept {
        return cpp14::get<std::map<std::string, timer<clock_type, meter<clock_type>, histogram<T>>>>(containers);
    }
};

/// \internal
class processor_t {
    boost::asio::io_service loop;
    boost::optional<boost::asio::io_service::work> work;

    std::thread thread;

    /// Data.
    struct {
        gauges_t<std::uint64_t> gauges;
        counters_t<std::int64_t, std::uint64_t> counters;
        std::map<std::string, meter_t> meters;
        timers_t<accumulator::sliding::window_t> timers;
    } data;

public:
    processor_t()
        : work(boost::optional<boost::asio::io_service::work>(boost::asio::io_service::work(loop))),
          thread([&] { loop.run(); }) {}

    ~processor_t() {
        work.reset();
        thread.join();
    }

    template<class F>
    void
    post(F fn) {
        loop.post(std::move(fn));
    }

    /// \warning must be called from event loop's thread.
    template<typename T>
    const std::map<std::string, std::function<T()>>&
    gauges() const {
        return data.gauges.get<T>();
    }

    /// \overload
    /// \warning must be called from event loop's thread.
    template<typename T>
    std::map<std::string, std::function<T()>>&
    gauges() {
        return data.gauges.get<T>();
    }

    /// \warning must be called from event loop's thread.
    template<typename T>
    const std::map<std::string, T>&
    counters() const {
        return data.counters.get<T>();
    }

    /// \overload
    /// \warning must be called from event loop's thread.
    template<typename T>
    std::map<std::string, T>&
    counters() {
        return data.counters.get<T>();
    }

    /// \warning must be called from event loop's thread.
    template<class Accumulate>
    const std::map<std::string, timer<clock_type, meter<clock_type>, histogram<Accumulate>>>&
    timers() const {
        return data.timers.get<Accumulate>();
    }

    /// \overload
    /// \warning must be called from event loop's thread.
    template<class Accumulate>
    std::map<std::string, timer<clock_type, meter<clock_type>, histogram<Accumulate>>>&
    timers() {
        return data.timers.get<Accumulate>();
    }

    /// \warning must be called from event loop's thread.
    template<typename T>
    boost::optional<std::function<T()>>
    gauge(const std::string& name) const {
        const auto& gauges = this->gauges<T>();
        const auto it = gauges.find(name);

        if (it != gauges.end()) {
            return boost::make_optional(it->second);
        } else {
            return boost::none;
        }
    }

    /// \warning must be called from event loop's thread.
    template<typename T>
    T&
    counter(const std::string& name) {
        return counters<T>()[name];
    }

    /// \warning must be called from event loop's thread.
    meter_t&
    meter(const std::string& name) {
        return data.meters[name];
    }

    /// \warning must be called from event loop's thread.
    template<typename Accumulate>
    timer<clock_type, metrics::detail::meter<clock_type>, histogram<Accumulate>>&
    timer(const std::string& name) {
        return timers<Accumulate>()[name];
    }
};

}  // namespace detail
}  // namespace metrics

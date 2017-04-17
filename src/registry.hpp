#pragma once

#include <functional>

#include "metrics/accumulator/sliding/window.hpp"
#include "metrics/accumulator/decaying/exponentially.hpp"
#include "metrics/registry.hpp"

#include "cpp14/tuple.hpp"
#include "histogram.hpp"
#include "meter.hpp"
#include "timer.hpp"

namespace metrics {

namespace tag {

template<typename T>
struct gauge {
    typedef std::function<T()> type;
};

template<typename T>
struct count {
    typedef std::atomic<T> type;
};

template<typename>
struct meter {
    typedef detail::meter_t type;
};

template<typename Accumulate>
struct timer {
    typedef std::chrono::high_resolution_clock clock_type;

    typedef detail::timer<clock_type, detail::meter<clock_type>, detail::histogram<Accumulate>> type;
};

}  // namespace tag

template<template<typename> class Tag, typename T>
struct collection_of;

/// Represents a tagged collection of metrics with various specializations.
template<template<typename> class Tag, typename... U>
struct collection_of<Tag, std::tuple<U...>> {
    std::tuple<
        std::map<
            tags_t,
            std::weak_ptr<typename Tag<U>::type>
        >...
    > containers;
    mutable std::mutex mutex;

    template<typename T>
    auto get() noexcept -> std::map<tags_t, std::weak_ptr<typename Tag<T>::type>>& {
        return cpp14::get<std::map<tags_t, std::weak_ptr<typename Tag<T>::type>>>(containers);
    }

    template<typename T>
    auto get() const noexcept -> std::map<tags_t, std::weak_ptr<typename Tag<T>::type>> const& {
        return cpp14::get<std::map<tags_t, std::weak_ptr<typename Tag<T>::type>>>(containers);
    }
};

template<template<typename> class Tag>
struct select_container;

class registry_t::inner_t {
public:
    collection_of<tag::gauge, std::tuple<std::int64_t, std::uint64_t, std::double_t, std::string>> gauges;
    collection_of<tag::count, std::tuple<std::int64_t, std::uint64_t>> counters;
    collection_of<tag::meter, std::tuple<detail::meter_t>> meters;
    collection_of<tag::timer, std::tuple<accumulator::sliding::window_t, accumulator::decaying::exponentially_t>> timers;
};

}  // namespace metrics

#pragma once

#include <future>

#include "metrics/metric.hpp"

namespace metrics {

/// \type `Accumulate` must implement `Accumulate` concept.
template<class Accumulate>
class timer : public metric_t {
public:
    typedef Accumulate accumulator_type;

    class context_t {
        timer* parent;

    public:
        context_t(timer* parent) :
            parent(parent)
        {}

        context_t(const context_t& other) = delete;
        context_t(context_t&&) = default;
    };

    typedef context_t context_type;

public:
    timer(std::string name, processor_t& processor);

    std::uint64_t
    count() const;

    // TODO: All `meter` getters.
    // TODO: All `snapshot` getters.

    // template<typename F>
    // auto
    // measure(F fn) -> decltype(fn());
    //
    // context_type
    // context();
};

}  // namespace metrics

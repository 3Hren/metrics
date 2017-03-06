#pragma once

#include <cmath>
#include <cstdint>

#include "fwd.hpp"

namespace metrics {
inline namespace v2 {

class visitor_t {
public:
    virtual ~visitor_t() = default;

    virtual auto visit(const gauge<std::int64_t>& metric) -> void = 0;
    virtual auto visit(const gauge<std::uint64_t>& metric) -> void = 0;
    virtual auto visit(const gauge<std::double_t>& metric) -> void = 0;
    virtual auto visit(const gauge<std::string>& metric) -> void = 0;
    virtual auto visit(const std::atomic<std::int64_t>& metric) -> void = 0;
    virtual auto visit(const std::atomic<std::uint64_t>& metric) -> void = 0;
    virtual auto visit(const meter_t& metric) -> void = 0;
    virtual auto visit(const timer<accumulator::sliding::window_t>& metric) -> void = 0;
    virtual auto visit(const timer<accumulator::decaying::exponentially_t>& metric) -> void = 0;
};

}  // namespace v2
}  // namespace metrics

#include "metrics/detail/metric/ewma.hpp"

#include <cmath>

#include "metrics/detail/metric/utility.hpp"

namespace {
    const auto m01alpha = -std::expm1(-5.0 / 60 / std::chrono::minutes(1).count());
    const auto m05alpha = -std::expm1(-5.0 / 60 / std::chrono::minutes(5).count());
    const auto m15alpha = -std::expm1(-5.0 / 60 / std::chrono::minutes(15).count());
}  // namespace

namespace metrics {
namespace detail {
namespace metric {

ewma_t::ewma_t(double alpha, clock_type::duration interval) :
    initialized(false),
    uncounted(0),
    alpha(alpha),
    interval(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count())
{
    d.rate = 0.0;
}

ewma_t
ewma_t::m01rate() {
    return ewma_t(m01alpha, std::chrono::seconds(5));
}

ewma_t
ewma_t::m05rate() {
    return ewma_t(m05alpha, std::chrono::seconds(5));
}

ewma_t
ewma_t::m15rate() {
    return ewma_t(m15alpha, std::chrono::seconds(5));
}

void
ewma_t::update(std::uint64_t value) {
    uncounted += value;
}

void
ewma_t::tick() {
    const auto count = std::exchange(uncounted, 0);
    const auto instant_rate = count / interval;

    if (std::exchange(initialized, true)) {
        d.rate += (alpha * (instant_rate - d.rate));
    } else {
        d.rate = instant_rate;
    }
}

}  // namespace metric
}  // namespace detail
}  // namespace metrics

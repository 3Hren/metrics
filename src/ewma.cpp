#include "metrics/ewma.hpp"

#include <cmath>

#include "metrics/detail/cpp14/utility.hpp"

namespace {
    const auto m01alpha = -std::expm1(-5.0 / 60 / std::chrono::minutes(1).count());
    const auto m05alpha = -std::expm1(-5.0 / 60 / std::chrono::minutes(5).count());
    const auto m15alpha = -std::expm1(-5.0 / 60 / std::chrono::minutes(15).count());
}  // namespace

namespace metrics {

ewma_t::ewma_t(double alpha, clock_type::duration interval) :
    uncounted(0),
    alpha(alpha),
    interval(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count())
{
    initialized.clear();
    d.rate = 0.0;
}

ewma_t
ewma_t::m01rate() {
    return {m01alpha, std::chrono::seconds(5)};
}

ewma_t
ewma_t::m05rate() {
    return {m05alpha, std::chrono::seconds(5)};
}

ewma_t
ewma_t::m15rate() {
    return {m15alpha, std::chrono::seconds(5)};
}

void
ewma_t::update(std::uint64_t value) {
    uncounted += value;
}

void
ewma_t::tick() {
    const auto count = std::atomic_exchange(&uncounted, std::uint64_t(0));
    const auto instant_rate = count / interval;

    if (initialized.test_and_set()) {
        // TODO: Spinlock required.
        d.rate = d.rate + (alpha * (instant_rate - d.rate));
    } else {
        d.rate = instant_rate;
    }
}

}  // namespace metrics

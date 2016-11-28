#include "metrics/usts/ewma.hpp"

namespace metrics {
namespace usts {

template<typename Clock>
ewma<Clock>::ewma(duration interval) :
    tau(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count()),
    average()
{
    if (interval == clock_type::duration::zero()) {
        throw std::invalid_argument("interval can not be zero");
    }
}

template<typename Clock>
auto ewma<Clock>::add(double value) -> void {
    add(clock_type::now(), value);
}

template<typename Clock>
auto ewma<Clock>::add(time_point time, double value) -> void {
    if (time < prev) {
        throw std::invalid_argument("time argument must monotonically increase");
    }

    if (initialized.test_and_set()) {
        const auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(time - prev).count();
        const auto alpha = delta / tau;
        const auto mu = std::exp(-alpha);
        average = mu * average + (1 - mu) * value;
    } else {
        average = value;
        birthstamp = time;
    }

    prev = time;
}

template<typename Clock>
auto ewma<Clock>::get() const -> double {
    return average;
}

template<typename Clock>
auto ewma<Clock>::warmed_up() const -> bool {
    const auto elapsed = std::chrono::duration_cast<
        std::chrono::nanoseconds
    >(prev - birthstamp).count();

    return elapsed >= 35 * tau;
}

template class ewma<std::chrono::high_resolution_clock>;

}  // namespace usts
}  // namespace metrics

#include "metrics/accumulator/sliding/window.hpp"

#include <algorithm>

namespace metrics {
namespace accumulator {
namespace sliding {

window_t::window_t():
    measurements(1024),
    count(0)
{}

window_t::window_t(std::size_t size):
    measurements(size),
    count(0)
{}

auto window_t::snapshot() const -> window_t::snapshot_type {
    const auto size = this->size();

    std::vector<std::uint64_t> result;
    result.reserve(size);

    std::unique_lock<std::mutex> lock(mutex);
    std::copy_n(measurements.begin(), size, std::back_inserter(result));
    lock.unlock();

    return snapshot_type(result);
}

auto window_t::size() const noexcept -> std::size_t {
    std::lock_guard<std::mutex> lock(mutex);
    return std::min(count.load(), measurements.size());
}

auto window_t::update(value_type value) noexcept -> void {
    std::lock_guard<std::mutex> lock(mutex);
    measurements[count++ % measurements.size()] = value;
}

auto window_t::operator()(value_type value) noexcept -> void {
    update(value);
}

}  // namespace sliding
}  // namespace accumulator
}  // namespace metrics

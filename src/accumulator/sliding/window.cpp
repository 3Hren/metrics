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

window_t::snapshot_type
window_t::snapshot() const {
    const auto size = this->size();

    std::vector<std::uint64_t> result;
    result.reserve(size);

    std::copy_n(measurements.begin(), size, std::back_inserter(result));

    return snapshot_type(result);
}

std::size_t
window_t::size() const noexcept {
    return std::min(count, measurements.size());
}

void
window_t::update(value_type value) noexcept {
    measurements[count++ % measurements.size()] = value;
}

void
window_t::operator()(value_type value) noexcept {
    update(value);
}

}  // namespace sliding
}  // namespace accumulator
}  // namespace metrics

#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <utility>

#include "metrics/accumulator/decaying/exponentially.hpp"

namespace metrics {
namespace accumulator {
namespace decaying {

constexpr auto DEFAULT_SIZE = 1024;
constexpr auto ALPHA_INIT = 0.015;

exponentially_t::exponentially_t() : exponentially_t(DEFAULT_SIZE, ALPHA_INIT) {}

exponentially_t::exponentially_t(std::size_t size, double alpha,
    duration_type rescale_period, boost::optional<std::mt19937::result_type> seed) :
    sample_size{size},
    alpha{alpha},
    start_time{clock_type::now()},
    rescale_threshold{rescale_period},
    uniform_dist{0.0, std::nextafter(1.0, std::numeric_limits<distr_real_type>::max())}
{
    if (sample_size == 0) {
        throw std::invalid_argument("sample reservoir can't be of zero size");
    }

    if (alpha <= 0) {
        throw std::invalid_argument("alpha should be greater then zero");
    }

    if (rescale_period.count() == 0) {
        throw std::invalid_argument("can't manage empty rescale interval");
    }

    if (seed) {
        gen.seed(*seed);
    } else {
        std::random_device dev;
        gen.seed(dev());
    }

    const auto rescale_since_epoch = start_time.time_since_epoch() + rescale_threshold;
    rescale_time = std::chrono::duration_cast<us_type>(rescale_since_epoch).count();
}

auto exponentially_t::update(std::uint64_t value, time_point t) -> void {
    // Rescale "if ever that time come".
    const auto now = clock_type::now();
    const auto rtm = us_type{rescale_time.load()};
    if (now.time_since_epoch() > rtm) {
        rescale(now, rtm.count());
    }

    const auto u = uniform_dist(gen);
    if (u == 0.0) {
        return;
    }

    const auto diff = std::chrono::duration_cast<seconds_type>(t - start_time);

    // Note: non normolized weights, sufficient for reservoir sampling
    const auto w = exp(alpha * diff.count());
    const auto prior = w / u;

    std::lock_guard<std::mutex> lock(samples_mut);

    samples.emplace(prior, sample_type{value, w});

    if (samples.size() > sample_size) {
        samples.erase(std::begin(samples));
    }
}

auto exponentially_t::operator()(std::uint64_t value, time_point timestamp) -> void {
    update(value, timestamp);
}

auto exponentially_t::size() const noexcept -> size_t {
    return std::min(sample_size, samples.size());
}

auto exponentially_t::snapshot() const -> snapshot_type {
    std::vector<sample_type> result;
    result.reserve(sample_size);

    {
        std::lock_guard<std::mutex> lock(samples_mut);

        for(const auto& smpl : samples) {
            result.emplace_back(smpl.second);
        }
    }

    return snapshot_type(std::move(result));
}

auto exponentially_t::rescale(time_point now, us_int_type next) -> void {
    const auto rsctm = now.time_since_epoch() + rescale_threshold;
    const auto addon = std::chrono::duration_cast<us_type>(rsctm).count();

    if (rescale_time.compare_exchange_strong(next, addon)) {
        const auto old_time = start_time;
        start_time = now;

        const auto tm_diff = std::chrono::duration_cast<seconds_type>(start_time - old_time);
        const auto scale = exp(-alpha * tm_diff.count());

        samples_mapping_type new_samples;

        std::lock_guard<std::mutex> lock(samples_mut);

        for(const auto& kv : samples) {
            const auto& k = kv.first;
            const auto& v = kv.second;

            new_samples.emplace(k * scale, sample_type{v.value, v.weight * scale});
        }

        samples.swap(new_samples);
    }
}

}  // namespace decaying
}  // namespace accumulator
}  // namespace metrics

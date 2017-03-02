#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <utility>

#include <metrics/accumulator/decaying/exponentially.hpp>

namespace metrics {
namespace accumulator {
namespace decaying {

constexpr auto RESCALE_THRESHOLD = std::chrono::hours(1);

exponentially_t::exponentially_t(std::size_t sz, double al) :
    sample_size{sz},
    alpha{al},
    start_time(clock_type::now()),
    uniform_dist(0.0, std::nextafter(1.0, std::numeric_limits<distr_real_type>::max()))
{
    if (sample_size == 0) {
        throw std::invalid_argument("sample reservoir can't be of zero size");
    }

    if (alpha <= 0) {
        throw std::invalid_argument("alpha should be greater then zero");
    }

    std::random_device dev;
    gen.seed(dev());

    const auto rescale_since_epoch = start_time.time_since_epoch() + RESCALE_THRESHOLD;
    rescale_time = std::chrono::duration_cast<us_type>(rescale_since_epoch).count();
}

auto exponentially_t::update(std::uint64_t value, time_point t) -> void {

    // rescale "if ever that time come"
    const auto now = clock_type::now();
    const auto rtm = us_type{rescale_time.load()};
    if (now.time_since_epoch() > rtm) {
        rescale(now, rtm.count());
    }

    const auto u = uniform_dist(gen);
    if (u == 0.0) {
        return;
    }

    const auto t_diff_ms = std::chrono::duration_cast<std::chrono::seconds>(t - start_time);

    // Note: non normolized weights, sufficient for reservoir sampling
    const auto w = exp(alpha * t_diff_ms.count());
    const auto prior = w / u;

    std::unique_lock<std::mutex> lock(samples_mut);

    samples.emplace(prior, sample_type{value, w});

    if (samples.size() > sample_size) {
        samples.erase(std::begin(samples));
    }
}

auto exponentially_t::size() const -> size_t {
    return std::min(sample_size, samples.size());
}

auto exponentially_t::snapshot() const -> snapshot_type {
    std::vector<sample_type> result;
    result.reserve(samples.size());

    {
        std::unique_lock<std::mutex> lock(samples_mut);

        std::transform(std::begin(samples), std::end(samples),
            std::back_inserter(result),
            [] (const priority_sample_type& smpl) {
                return smpl.second;
        });
    }

    return snapshot_type(std::move(result));
}

auto exponentially_t::rescale(time_point now, us_int_type next) -> void {

    const auto rescale_since_epoch = now.time_since_epoch() + RESCALE_THRESHOLD;
    const auto addon = std::chrono::duration_cast<us_type>(rescale_since_epoch).count();

    if (rescale_time.compare_exchange_strong(next, addon)) {
        const auto old_time = start_time;

        start_time = now;
        rescale_time.store(addon);

        const auto tm_diff = std::chrono::duration_cast<std::chrono::seconds>(start_time - old_time);
        const auto scale = exp(-alpha * tm_diff.count());

        decltype(samples) new_samples;

        std::unique_lock<std::mutex> lock(samples_mut);

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

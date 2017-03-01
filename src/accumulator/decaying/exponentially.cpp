#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <utility>

#include <metrics/accumulator/decaying/exponentially.hpp>

namespace metrics {
namespace accumulator {
namespace decaying {

constexpr auto RESCALE_TRESHOLD = std::chrono::hours(1);

namespace aux {

    // TODO: remove after testings
    typedef exponentially_t::priority_sample_type point_t;
    struct heap_comp_t {
        bool operator()(const point_t& a, const point_t& b) const {
            return a.first > b.first;
        }
    };

    constexpr heap_comp_t heap_rcmp;
}

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

    // samples.reserve(sample_size);

    std::random_device dev;
    gen.seed(dev());

    rescale_time = start_time + RESCALE_TRESHOLD;
}

auto exponentially_t::update(std::uint64_t value, time_point tp) -> void {
    // rescale "if ever that time come"
    const auto now = clock_type::now();
    if (now > rescale_time) {
        rescale(now);
    }

    const auto u = uniform_dist(gen);
    if (u == 0.0) {
        return;
    }

    const auto t_diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp - start_time);

    // Note: non normolized weights, sufficient for sampling
    const auto w = exp(alpha * t_diff_ms.count());
    const auto prior = w / u;

    std::unique_lock<std::mutex> lock(samples_mut);

    samples.emplace(prior, sample_type{value, w} );

    // samples.emplace_back(prior, {value, w});
    // std::push_heap(std::begin(samples), std::end(samples), heap_rcmp);

    if (samples.size() > sample_size) {
        // std::pop_heap(std::begin(samples), std::end(samples), heap_rcmp);
        // samples.pop_back();

        samples.erase(std::begin(samples));
    }
}

auto exponentially_t::size() const -> size_t {
    return std::min(sample_size, samples.size());
}

auto exponentially_t::snapshot() const -> snapshot::weighted_t {
    std::vector<sample_type> v;
    v.reserve(samples.size());

    std::unique_lock<std::mutex> lock(samples_mut);

    std::transform(std::begin(samples), std::end(samples),
        std::back_inserter(v),
        [] (const priority_sample_type& smpl) {
            return smpl.second;
    });

    return snapshot::weighted_t(std::move(v));
}

auto exponentially_t::rescale(time_point now) -> void {
    const auto old_time = start_time;

    start_time = now;
    rescale_time = start_time + RESCALE_TRESHOLD;

    const auto tm_diff = std::chrono::duration_cast<std::chrono::milliseconds>(start_time - old_time);
    const auto scale = exp(-alpha * tm_diff.count());

    decltype(samples) new_samples;

    std::unique_lock<std::mutex> lock(samples_mut);

    for(const auto& kv : samples) {
        const auto& k = kv.first;
        const auto& v = kv.second;

        new_samples.emplace(k * scale, sample_type{ v.value, v.weight * scale });
    }

    samples.swap(new_samples);
}

}  // namespace decaying
}  // namespace accumulator
}  // namespace metrics

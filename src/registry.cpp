#include "metrics/registry.hpp"

#include <boost/range/algorithm/transform.hpp>

#include "metrics/gauge.hpp"
#include "metrics/meter.hpp"
#include "metrics/metric.hpp"
// #include "metrics/timer.hpp"

#include "registry.hpp"

namespace metrics {

namespace {

const auto query_all = [](const tags_t&) -> bool {
    return true;
};

struct transformer_t {
    template<typename T>
    auto
    operator()(const std::pair<const tags_t, shared_metric<T>>& pair) const ->
        std::shared_ptr<tagged_t>
    {
        return std::make_shared<shared_metric<T>>(pair.second);
    }
};

template<typename R, typename T, typename M>
auto
instances(const query_t& query, M& map) -> registry_t::metric_set<R> {
    typedef shared_metric<R> value_type;

    registry_t::metric_set<R> result;

    std::lock_guard<std::mutex> lock(map.mutex);
    const auto& instances = map.template get<T>();

    tags_t tags("");
    std::shared_ptr<R> metric;
    for (const auto& it : instances) {
        std::tie(tags, metric) = it;
        if (query(tags) && metric) {
            result.insert(std::make_pair(tags, value_type(tags, metric)));
        }
    }

    return result;
}

} // namespace

registry_t::registry_t():
    inner(new inner_t)
{}

registry_t::~registry_t() = default;

template<typename R>
auto
registry_t::register_gauge(std::string name, tags_t::container_type other, std::function<R()> fn) ->
    void
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->gauges.mutex);
    auto& instances = inner->gauges.template get<R>();

    std::shared_ptr<metrics::gauge<R>> instance;
    if((instance = instances[tags]) == nullptr) {
        instance = std::make_shared<metrics::gauge<R>>(std::move(fn));
        instances[tags] = instance;
    }
}

template<typename T>
auto
registry_t::gauge(std::string name, tags_t::container_type other) const ->
    shared_metric<metrics::gauge<T>>
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->gauges.mutex);
    const auto& instances = inner->gauges.template get<T>();
    return {tags, instances.at(tags)};
}

template<typename T>
auto
registry_t::gauges() const -> metric_set<metrics::gauge<T>> {
    return gauges<T>(query_all);
}

template<typename T>
auto
registry_t::gauges(const query_t& query) const -> metric_set<metrics::gauge<T>> {
    return instances<metrics::gauge<T>, T>(query, inner->gauges);
}

template<typename T>
auto
registry_t::counter(std::string name, tags_t::container_type other) const ->
    shared_metric<std::atomic<T>>
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->counters.mutex);
    auto& instances = inner->counters.template get<T>();

    std::shared_ptr<std::atomic<T>> instance;
    if((instance = instances[tags]) == nullptr) {
        instance = std::make_shared<std::atomic<T>>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

template<typename T>
auto
registry_t::counters() const -> metric_set<std::atomic<T>> {
    return counters<T>(query_all);
}

template<typename T>
auto
registry_t::counters(const query_t& query) const -> metric_set<std::atomic<T>> {
    return instances<std::atomic<T>, T>(query, inner->counters);
}

auto
registry_t::meter(std::string name, tags_t::container_type other) const ->
    shared_metric<meter_t>
{
    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->meters.mutex);
    auto& instances = inner->meters.get<detail::meter_t>();

    std::shared_ptr<detail::meter_t> instance;
    if((instance = instances[tags]) == nullptr) {
        instance = std::make_shared<detail::meter_t>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

auto
registry_t::meters() const -> metric_set<meter_t> {
    return meters(query_all);
}

auto
registry_t::meters(const query_t& query) const -> metric_set<meter_t> {
    return instances<meter_t, detail::meter_t>(query, inner->meters);
}

template<class Accumulate>
auto registry_t::timer(std::string name, tags_t::container_type other) const ->
    shared_metric<metrics::timer<Accumulate>>
{
    typedef std::chrono::high_resolution_clock clock_type;
    typedef detail::timer<
        clock_type,
        detail::meter<clock_type>,
        detail::histogram<Accumulate>
    > result_type;

    tags_t tags(std::move(name), std::move(other));

    std::lock_guard<std::mutex> lock(inner->timers.mutex);
    auto& instances = inner->timers.template get<Accumulate>();

    std::shared_ptr<result_type> instance;
    if((instance = instances[tags]) == nullptr) {
        instance = std::make_shared<result_type>();
        instances[tags] = instance;
    }

    return {std::move(tags), std::move(instance)};
}

template<class Accumulate>
auto
registry_t::timers() const -> metric_set<metrics::timer<Accumulate>> {
    return timers<Accumulate>(query_all);
}

template<class Accumulate>
auto
registry_t::timers(const query_t& query) const -> metric_set<metrics::timer<Accumulate>> {
    return instances<metrics::timer<Accumulate>, Accumulate>(query, inner->timers);
}

auto
registry_t::select() const -> std::vector<std::shared_ptr<tagged_t>> {
    return select(query_all);
}

auto
registry_t::select(const query_t& query) const ->
    std::vector<std::shared_ptr<tagged_t>>
{
    std::vector<std::shared_ptr<tagged_t>> result;
    auto fn = transformer_t();
    auto out = std::back_inserter(result);

    boost::transform(gauges<std::int64_t>(query), out, fn);
    boost::transform(gauges<std::uint64_t>(query), out, fn);
    boost::transform(gauges<std::double_t>(query), out, fn);
    boost::transform(counters<std::int64_t>(query), out, fn);
    boost::transform(counters<std::uint64_t>(query), out, fn);
    boost::transform(meters(query), out, fn);
    boost::transform(timers<>(query), out, fn);

    return result;
}

/// Instantiations.

template
auto registry_t::register_gauge<std::int64_t>(std::string, tags_t::container_type other, std::function<std::int64_t()> fn) ->
    void;

template
auto registry_t::register_gauge<std::uint64_t>(std::string, tags_t::container_type other, std::function<std::uint64_t()> fn) ->
    void;

template
auto registry_t::register_gauge<double>(std::string, tags_t::container_type other, std::function<double()> fn) ->
    void;

template
auto registry_t::gauge<std::int64_t>(std::string, tags_t::container_type) const ->
    shared_metric<metrics::gauge<std::int64_t>>;

template
auto registry_t::gauge<std::uint64_t>(std::string, tags_t::container_type) const ->
    shared_metric<metrics::gauge<std::uint64_t>>;

template
auto registry_t::gauge<double>(std::string, tags_t::container_type) const ->
    shared_metric<metrics::gauge<double>>;

template
auto registry_t::gauges<std::int64_t>() const ->
    std::map<tags_t, shared_metric<metrics::gauge<std::int64_t>>>;

template
auto registry_t::gauges<std::uint64_t>() const ->
    std::map<tags_t, shared_metric<metrics::gauge<std::uint64_t>>>;

template
auto registry_t::gauges<double>() const ->
    std::map<tags_t, shared_metric<metrics::gauge<double>>>;

template
auto registry_t::counter<std::int64_t>(std::string, tags_t::container_type) const ->
    shared_metric<std::atomic<std::int64_t>>;

template
auto registry_t::counter<std::uint64_t>(std::string, tags_t::container_type) const ->
    shared_metric<std::atomic<std::uint64_t>>;

template
auto registry_t::counters<std::int64_t>() const ->
    std::map<tags_t, shared_metric<std::atomic<std::int64_t>>>;

template
auto registry_t::timer<accumulator::sliding::window_t>(std::string, tags_t::container_type tags) const ->
    shared_metric<metrics::timer<accumulator::sliding::window_t>>;

template
auto registry_t::timers<accumulator::sliding::window_t>() const ->
    std::map<tags_t, shared_metric<metrics::timer<accumulator::sliding::window_t>>>;

}  // namespace metrics

#include "metrics/tags.hpp"

#include <boost/optional/optional.hpp>

namespace metrics {

tags_t::tags_t(std::string name) : container({{"name", std::move(name)}}) {}

tags_t::tags_t(std::string name, container_type tags) : container(std::move(tags)) {
    container["name"] = std::move(name);
}

const tags_t::container_type&
tags_t::tags() const noexcept {
    return container;
}

const std::string&
tags_t::name() const noexcept {
    return container.at("name");
}

boost::optional<const std::string&>
tags_t::tag(const std::string& key) const {
    const auto it = container.find(key);

    if (it == container.end()) {
        return boost::none;
    }

    return it->second;
}

bool
tags_t::operator==(const tags_t& other) const {
    return container == other.container;
}

bool
tags_t::operator!=(const tags_t& other) const {
    return !(*this == other);
}

bool
tags_t::operator<(const tags_t& other) const {
    return container < other.container;
}

} // namespace metrics

std::hash<metrics::tags_t>::result_type
std::hash<metrics::tags_t>::operator()(const argument_type& v) const {
    result_type result = std::hash<std::string>()("");

    for (const auto& kv : v.tags()) {
        const result_type hk = std::hash<std::string>()(kv.first);
        const result_type hv = std::hash<std::string>()(kv.second);

        result ^= (hk << 1);
        result ^= (hv << 1);
    }

    return result;
}

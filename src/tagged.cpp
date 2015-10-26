#include "metrics/tagged.hpp"

#include <boost/optional/optional.hpp>

namespace metrics {

tagged_t::tagged_t(std::string name):
    container({{"name", std::move(name)}})
{}

tagged_t::tagged_t(std::string name, container_type tags):
    container(std::move(tags))
{
    container["name"] = std::move(name);
}

const tagged_t::container_type&
tagged_t::tags() const noexcept {
    return container;
}

const std::string&
tagged_t::name() const noexcept {
    return container.at("name");
}

boost::optional<std::string>
tagged_t::tag(const std::string& key) const {
    const auto it = container.find(key);

    if (it == container.end()) {
        return boost::none;
    }

    return it->second;
}

bool
tagged_t::operator==(const tagged_t& other) const {
    return container == other.container;
}

bool
tagged_t::operator!=(const tagged_t& other) const {
    return !(*this == other);
}

}  // namespace metrics

std::hash<metrics::tagged_t>::result_type
std::hash<metrics::tagged_t>::operator()(const argument_type& v) const {
    result_type result = std::hash<std::string>()("");

    for (const auto& kv : v.tags()) {
        const result_type hk = std::hash<std::string>()(kv.first);
        const result_type hv = std::hash<std::string>()(kv.second);

        result ^= (hk << 1);
        result ^= (hv << 1);
    }

    return result;
}

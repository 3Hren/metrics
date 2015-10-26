#pragma once

#include <map>
#include <string>

#include <boost/optional/optional_fwd.hpp>

namespace metrics {

/// The tagged struct represents tagged metric name.
class tagged_t {
public:
    typedef std::map<std::string, std::string> container_type;

private:
    container_type container;

public:
    /// Creates a tagged struct with a single `name` tag.
    tagged_t(std::string name);

    /// Creates a tagged struct with the given tags, overriding the `name` tag if required.
    tagged_t(std::string name, container_type tags);

    /// Returns a const reference to the underlying tag map.
    const container_type&
    tags() const noexcept;

    /// Returns a const reference to the name tag.
    const std::string&
    name() const noexcept;

    /// Returns the tag value that is mapped to a key equivalent to `key`, `boost::none` otherwise.
    boost::optional<std::string>
    tag(const std::string& key) const;

    // TODO: hash.
    // TODO: operator==.
};

}  // namespace metrics

#pragma once

#include <map>
#include <string>

#include <boost/optional/optional_fwd.hpp>

namespace metrics {

/// The tags struct represents immutable tagged metric name.
class tags_t {
public:
    typedef std::map<std::string, std::string> container_type;

private:
    container_type container;

public:
    /// Creates a tagged struct with a single `name` tag.
    tags_t(std::string name);

    /// Creates a tagged struct with the given tags, overriding the `name` tag
    /// if required.
    tags_t(std::string name, container_type tags);

    /// Returns a const reference to the name tag.
    auto
    name() const noexcept -> const std::string&;

    /// Returns a tag value that is mapped to a key equivalent to `key`, none
    /// otherwise.
    auto
    tag(const std::string& key) const -> boost::optional<const std::string&>;

    /// Returns a const reference to the underlying tags.
    auto
    tags() const noexcept -> const container_type&;

    /// Partial equality operators.

    auto
    operator==(const tags_t& other) const -> bool;

    auto
    operator!=(const tags_t& other) const -> bool;

    /// Ordering operators.

    auto
    operator<(const tags_t& other) const -> bool;
};

} // namespace metrics

namespace std {

template <>
struct hash<metrics::tags_t> {
    typedef metrics::tags_t argument_type;
    typedef std::size_t result_type;

    auto
    operator()(const argument_type& v) const -> result_type;
};

} // namespace std

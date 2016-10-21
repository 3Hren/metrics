#pragma once

#include <memory>
#include <string>

#include <boost/optional/optional_fwd.hpp>

#include "forwards.hpp"
#include "tags.hpp"

namespace metrics {

/// Represents immutable tagged wrapper standalone metrics.
template<typename T>
class tagged {
    tags_t value;

public:
    explicit tagged(tags_t value);

    /// Returns metric name.
    auto name() const noexcept -> const std::string&;

    /// Extracts a tag with the given key, returning nothing otherwise.
    auto tag(const std::string& key) const -> boost::optional<std::string>;

    /// Returns a const lvalue reference to the underlying tags.
    auto tags() const noexcept -> const tags_t&;
};

/// Represents a tagged shared metric wrapper over standalone metrics.
template<typename T>
class shared_metric : public tagged<T> {
    std::shared_ptr<T> inner;

public:
    shared_metric(tags_t tags, std::shared_ptr<T> inner);

    auto get() const -> std::shared_ptr<T>;

    auto operator->() const -> T*;
};

}  // namespace metrics

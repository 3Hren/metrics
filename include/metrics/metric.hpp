#pragma once

#include <cmath>
#include <memory>
#include <string>

#include <boost/optional/optional_fwd.hpp>

#include "fwd.hpp"
#include "gauge.hpp"
#include "tags.hpp"

namespace metrics {

/// Metric interface.
class metric_t {
public:
    virtual ~metric_t() = default;

    /// Returns metric name.
    virtual auto
    name() const -> std::string = 0;

    /// Returns string representation of metric type.
    virtual auto
    type() const -> std::string = 0;

    virtual auto
    apply(visitor_t& visitor) const -> void = 0;
};

/// An immutable tagged wrapper around standalone metric.
class tagged_t : public metric_t {
    tags_t value;

public:
    explicit tagged_t(tags_t value);

    auto name() const -> std::string override;

    /// Extracts a tag with the given key, returning nothing otherwise.
    auto tag(const std::string& key) const -> boost::optional<const std::string&>;

    /// Returns a const lvalue reference to the underlying tags.
    auto tags() const noexcept -> const tags_t&;
};

/// A tagged shared metric wrapper over standalone metrics.
template<typename T>
class shared_metric : public tagged_t {
    std::shared_ptr<T> inner;

public:
    shared_metric(tags_t tags, std::shared_ptr<T> inner);

    auto
    type() const -> std::string override;

    auto get() const -> std::shared_ptr<T>;

    auto operator->() const -> T*;

    auto apply(visitor_t& visitor) const -> void override;
};

}  // namespace metrics

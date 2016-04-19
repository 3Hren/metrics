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

template<typename T>
struct wrapper_traits {
    typedef std::shared_ptr<T> type;
};

template<typename T>
struct wrapper_traits<gauge<T>> {
    typedef std::function<T()> type;
};

/// Represents a tagged shared metric wrapper over standalone metrics.
template<typename T>
class metric : public tagged<T> {
public:
    typedef typename wrapper_traits<T>::type inner_type;

private:
    inner_type inner;

public:
    metric(tags_t tags, inner_type inner);

    auto get() const -> inner_type;

    auto operator->() const -> T*;
};

}  // namespace metrics

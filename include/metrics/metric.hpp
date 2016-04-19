#pragma once

#include <memory>
#include <string>

#include <boost/optional/optional_fwd.hpp>

#include "tags.hpp"

namespace metrics {

template<typename T>
class metric {
    struct data_t {
        tags_t tags;
        std::shared_ptr<T> inner;

        data_t(tags_t tags, std::shared_ptr<T> inner);
    } d;

public:
    metric(tags_t tags, std::shared_ptr<T> inner);

    auto tags() const noexcept -> const tags_t&;
    auto name() const noexcept -> const std::string&;

    auto tag(const std::string& key) const -> boost::optional<std::string>;

    auto operator->() const -> T*;
    auto get() const -> std::shared_ptr<T>;
};

}  // namespace metrics

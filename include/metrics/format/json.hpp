#pragma once

#include <vector>

#include "../fwd.hpp"

namespace metrics {
inline namespace v2 {
namespace format {

auto as_json(const std::vector<std::shared_ptr<tagged_t>>& metrics) -> std::string;

}  // namespace format
}  // namespace v2
}  // namespace metrics

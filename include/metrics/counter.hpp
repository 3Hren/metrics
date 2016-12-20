#pragma once

namespace metrics {
inline namespace v2 {

class counter_t {
public:
    typedef std::int64_t value_type;

public:
    virtual auto get() const -> value_type;

    virtual auto inc() -> value_type = 0;
    virtual auto inc(value_type value) -> value_type = 0;

    virtual auto dec() -> value_type = 0;
    virtual auto dec(value_type value) -> value_type = 0;
};

}  // namespace v2
}  // namespace metrics

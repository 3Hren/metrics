#pragma once

#include <map>
#include <string>

#include <boost/optional/optional_fwd.hpp>

namespace metrics {

class tagged_t {
public:
    typedef std::map<std::string, std::string> container_type;

private:
    container_type container;

public:
    tagged_t(std::string name);
    tagged_t(std::string name, container_type tags);

    virtual
    ~tagged_t();

    const container_type&
    tags() const noexcept;

    const std::string&
    name() const noexcept;

    boost::optional<std::string>
    tag(const std::string& key) const;

    // TODO: hash.
    // TODO: operator==.
};

}  // namespace metrics

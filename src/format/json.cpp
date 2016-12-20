#include "metrics/format/json.hpp"

#include <array>
#include <iostream>

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/pointer.h>

#include "metrics/accumulator/sliding/window.hpp"
#include "metrics/gauge.hpp"
#include "metrics/meter.hpp"
#include "metrics/timer.hpp"
#include "metrics/visitor.hpp"

namespace metrics {
inline namespace v2 {
namespace format {

template<typename D>
class json_t : public visitor_t {
    const std::string& name;
    D& root;

public:
    explicit json_t(const std::string& name, D& root) :
        name(name),
        root(root)
    {}

    auto visit(const gauge<std::int64_t>& metric) -> void override {
        do_visit(metric);
    }

    auto visit(const gauge<std::uint64_t>& metric) -> void override {
        do_visit(metric);
    }

    auto visit(const gauge<std::double_t>& metric) -> void override {
        do_visit(metric);
    }

    auto visit(const std::atomic<std::int64_t>& metric) -> void override {
        do_visit(metric);
    }

    auto visit(const std::atomic<std::uint64_t>& metric) -> void override {
        do_visit(metric);
    }

    auto visit(const meter_t& metric) -> void override {
        rapidjson::Pointer(name + "/count").Set(root, metric.count());
        rapidjson::Pointer(name + "/m01rate").Set(root, metric.m01rate());
        rapidjson::Pointer(name + "/m05rate").Set(root, metric.m05rate());
        rapidjson::Pointer(name + "/m15rate").Set(root, metric.m15rate());
    }

    auto visit(const timer<accumulator::sliding::window_t>& metric) -> void override {
        do_visit(metric);
    }

private:
    template<typename T>
    auto do_visit(const gauge<T>& metric) -> void {
        rapidjson::Pointer(name).Set(root, metric());
    }

    template<typename T>
    auto do_visit(const std::atomic<T>& metric) -> void {
        rapidjson::Pointer(name).Set(root, metric.load());
    }

    template<typename T>
    auto do_visit(const timer<T>& metric) -> void {
        rapidjson::Pointer(name + "/count").Set(root, metric.count());
        rapidjson::Pointer(name + "/m01rate").Set(root, metric.m01rate());
        rapidjson::Pointer(name + "/m05rate").Set(root, metric.m05rate());
        rapidjson::Pointer(name + "/m15rate").Set(root, metric.m15rate());

        const auto snapshot = metric.snapshot();
        rapidjson::Pointer(name + "/p50").Set(root, snapshot.median());
        rapidjson::Pointer(name + "/p75").Set(root, snapshot.p75());
        rapidjson::Pointer(name + "/p90").Set(root, snapshot.p90());
        rapidjson::Pointer(name + "/p95").Set(root, snapshot.p95());
        rapidjson::Pointer(name + "/p98").Set(root, snapshot.p98());
        rapidjson::Pointer(name + "/p99").Set(root, snapshot.p99());
        rapidjson::Pointer(name + "/mean").Set(root, snapshot.mean());
        rapidjson::Pointer(name + "/stddev").Set(root, snapshot.stddev());
    }
};

auto as_json(const std::vector<std::shared_ptr<tagged_t>>& metrics) -> std::string {
    typedef rapidjson::GenericDocument<
        rapidjson::UTF8<>,
        rapidjson::MemoryPoolAllocator<>,
        rapidjson::MemoryPoolAllocator<>
    > document_type;

    std::array<char, 4096> value_buffer;
    std::array<char, 1024> parse_buffer;
    rapidjson::MemoryPoolAllocator<> value_allocator(value_buffer.data(), value_buffer.size());
    rapidjson::MemoryPoolAllocator<> parse_allocator(parse_buffer.data(), parse_buffer.size());

    document_type root(&value_allocator, parse_buffer.size(), &parse_allocator);
    root.SetObject();

    for (const auto metric : metrics) {
        auto name = "/" + metric->name();
        std::replace(std::begin(name), std::end(name), '.', '/');
        json_t<document_type> visitor(name, root);
        metric->apply(visitor);
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> wr(buffer);
    root.Accept(wr);

    return buffer.GetString();
}

}  // namespace format
}  // namespace v2
}  // namespace metrics

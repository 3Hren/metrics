#pragma once

namespace metrics {
namespace detail {

/// A metric which calculates the distribution of a value.
///
/// \type `A` must implement `Accumulate` concept.
template<class A>
class histogram {
public:
    typedef A accumulator_type;
    typedef typename accumulator_type::snapshot_type snapshot_type;

private:
    struct data_t {
        accumulator_type accumulator;
        std::size_t count;

        template<typename... Args>
        data_t(Args&&... args) :
            accumulator(std::forward<Args>(args)...),
            count(0)
        {}
    } d;

public:
    /// Creates a new `histogram` with the given accumulator via perfect forwarding.
    template<typename... Args>
    histogram(Args&&... args) :
        d(std::forward<Args>(args)...)
    {}

    /// Observers.

    const accumulator_type&
    accumulator() const {
        return d.accumulator;
    }

    /// Lookup.

    std::uint64_t
    count() const noexcept {
        return d.count;
    }

    snapshot_type
    snapshot() const {
        return accumulator().snapshot();
    }

    /// Modifiers.

    /// Adds a recorded value.
    ///
    /// \param `value` the length of the value.
    void
    update(std::uint64_t value) {
        d.count++;
        d.accumulator.update(value);
    }
};

}  // namespace detail
}  // namespace metrics

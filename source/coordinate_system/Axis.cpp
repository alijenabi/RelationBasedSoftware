//
//  Axis.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.01.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Axis.h"
#include "../space/Point.h"
#include <set>

namespace rbs::coordinate_system {

Axis::Axis()
    : p_values{}
{}

Axis::Axis(const Axis::Values &values)
    : p_values{values}
{}

Axis::Value Axis::at(const Index index) const {
    if (p_values.empty())
        throw std::range_error("The axis is empty.");

    if (index < p_values.size()) {
        auto itr = p_values.begin();
        std::advance(itr, index);
        return *itr;
    }
    throw std::range_error("Index is out of IndexRange.");
}

Axis::OptionalValue Axis::closestTo(const Axis::Value value) const {
    if (p_values.empty())
        return nullptr;

    const auto itr = p_values.lower_bound(value);
    if (itr == p_values.begin())
        return std::make_shared<Value>(*itr);

    const auto preItr = std::prev(itr);
    if (itr == p_values.end() || value - *preItr <= *itr - value)
        return std::make_shared<Value>(*preItr);
    return std::make_shared<Value>(*itr);
}

Axis::OptionalIndex Axis::closestIndexTo(const Axis::Value value) const {
    const auto v = closestTo(value);
    return (v) ? indexOf(*v) : nullptr;
}

bool Axis::contains(const Axis::Value value) const {
    return p_values.find(value) != p_values.end();
}

bool Axis::covers(const Axis::Value value) const {
    if (isEmpty())
        return false;
    return *p_values.begin() <= value && value <= *p_values.rbegin();
}

bool Axis::covers(const Axis::Range range) const {
    return covers(range.first) || covers(range.second);
}

bool Axis::covers(const Axis::Value start, const Axis::Value end) const {
    return covers(start) || covers(end) || ( start < *p_values.begin() &&  *p_values.rbegin()< end );
}

Axis::OptionalIndexRange Axis::indicesBetween(const Axis::Value low, const Axis::Value high) const {
    if (high < low)
        return indicesBetween(high, low);

    OptionalIndexRange range;
    if (covers(low, high)) {
        const auto l = closestTo(low);
        const auto h = closestTo(high);
        if (l && h) {
            auto start = iteratorFor(*l);
            const auto end = iteratorFor(*h);

            if (start == end && !(low <= *start && *end <= high)) {
                return nullptr;
            } else if (++start == end && *start < low && high < *end) {
                return nullptr;
            }
            return std::make_shared<IndexRange>(std::distance(p_values.begin(),iteratorFor(*l)), std::distance(p_values.begin(), end));
        }
    }
    return nullptr;
}

bool Axis::isEmpty() const {
    return p_values.empty();
}

Axis::OptionalIndexRange Axis::neighborRange(const Axis::Value centre, const Axis::Value radius) const {
    return indicesBetween(centre - radius, centre + radius);
}

Axis::ValuePtrs Axis::neighborsValues(const Axis::Value centre, const Axis::Value radius) const {
    return valuesBetween(centre - radius, centre + radius);
}

size_t Axis::size() const {
    return p_values.size();
}

const Axis::Values& Axis::values() const {
    return p_values;
}

Axis::ValuePtrs Axis::valuesBetween(const Axis::Value low, const Axis::Value high) const {
    if (high < low)
        return valuesBetween(high, low);

    ValuePtrs values;
    if (covers(low, high)) {
        const auto l = closestTo(low);
        const auto h = closestTo(high);
        if (l && h) {
            const auto end = iteratorFor(*h);
            for(auto itr = iteratorFor(*l); itr != end; itr++) {
                values.emplace_back(std::make_shared<Value>(*itr));
            }
            if (values.size() == 2 && *values[0] < low && high < *values[1]) {
                values.clear();
            }
            if (values.size() == 1 && !(low <= *values[0] && *values[0] <= high)) {
                values.clear();
            }
        }
    }
    return values;
}

void Axis::clear() {
    p_values.clear();
}

void Axis::exclude(const Axis::Value value) {
    p_values.erase(value);
}

Axis::Index Axis::include(const Axis::Value value) {
    const auto set = p_values.insert(value);
    return static_cast<Index>(std::distance(p_values.begin(), set.first));
}

Axis::Values::iterator Axis::iteratorFor(const Axis::Value value) const {
    const auto f = p_values.find(value);
    if(f != p_values.end())
        return f;
    throw std::range_error("The value is not on the axis.");
}

Axis::Values::iterator Axis::iteratorOf(const Axis::Index index) const {
    if (index < p_values.size()) {
        auto itr = p_values.begin();
        std::advance(itr, index);
        return itr;
    }
    throw std::range_error("Index is out of range.");
}

Axis::OptionalIndex Axis::indexOf(const Axis::Value value) const {
    const auto v = p_values.find(value);
    if (v == p_values.end())
        return nullptr;
    return std::make_shared<Index>(static_cast<Index>(std::distance(p_values.begin(), v)));
}

} // namespace rbs::coordinate_system

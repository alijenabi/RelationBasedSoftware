//
//  Axis.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.01.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef AXIS_H
#define AXIS_H

#include <set>
#include <vector>

/**
 * The space namespace provides the means to handle matters related to the mathematical space.
 */
namespace rbs::space {

template<unsigned char SpaceDimension> class Point;

} // namespace rbs::space

/**
 * The coordinate_system namespace provides the means to work with different coordinate systems.
 */
namespace rbs::coordinate_system {

/**
 * @brief The Axis class provides the means to mamge coordinate systems axies and their data.
 * @note The axis is defined in a 3D space where but it presents a 1D space.
 */
class Axis
{
public:

    using Index = size_t;
    using OptionalIndex = std::shared_ptr<Index>;
    using IndexRange = std::pair<Index, Index>;
    using OptionalIndexRange = std::shared_ptr<IndexRange>;

    using Value = double;
    using OptionalValue = std::shared_ptr<Value>;
    using Range = std::pair<Value, Value>;
    using OptionalRange = std::shared_ptr<Range>;

    using Values = std::set<Value>;
    using ValuePtr = std::shared_ptr<Value>;
    using ValuePtrs = std::vector<ValuePtr>;

    /**
     * @brief The constructor.
     */
    Axis();

    /**
     * @brief The constructor.
     * @param values The valuse on the axis.
     */
    Axis(const Values & values);

    /**
     * @brief Gives the value at a index.
     * @param index The index of the value.
     * @throws If the index is out of range (i.e., [0 size() - 1]) or the Axis is empty.
     */
    Value at(const Index index) const;

    /**
     * @brief Gives the closest value on the axis to the given value.
     * @param value The value to search.
     * @return A shared pointer to the closest value to the given value or nullptr if axis dose not hold any values.
     */
    OptionalValue closestTo(const Value value) const;

    /**
     * @brief Gives the index of the closest value on the axis to the given value.
     * @param value The value to search.
     * @return A shared pointer to the index of closest value to the given value or nullptr if axis dose not hold any values.
     */
    OptionalIndex closestIndexTo(const Value value) const;

    /**
     * @brief Whether the axis contins a value exactly or not.
     */
    bool contains(const Value value) const;

    /**
     * @brief Whether the axis covers a value or not.
     */
    bool covers(const Value value) const;

    /**
     * @brief Whether the axis covers a range of values or not.
     */
    bool covers(const Range range) const;

    /**
     * @brief Whether the axis covers a range of values or not.
     */
    bool covers(const Value start, const Value end) const;

    /**
     * @brief Gives the index of the values between two values.
     * @param low The lower bond.
     * @param high The upper bond.
     */
    OptionalIndexRange indicesBetween(const Value low, const Value high) const;

    /**
     * @brief Whether the axis is empty or not.
     */
    bool isEmpty() const;

    /**
     * @brief Gives the index of the values around a centre.
     * @param centre The centre of neighborhood.
     * @param radius The radius of the neighborhood.
     */
    OptionalIndexRange neighborRange(const Value centre, const Value radius) const;

    /**
     * @brief Gives the values on the axis around a centre.
     * @param centre The centre of neighborhood.
     * @param radius The radius of the neighborhood.
     */
    ValuePtrs neighborsValues(const Value centre, const Value radius) const;

    /**
     * @brief Gives the number of values on the axis.
     */
    size_t size() const;

    /**
     * @brief Gives the values on the axis.
     */
    const Values& values() const;

    /**
     * @brief Gives the values on the axis between two values.
     * @param low The lower bond.
     * @param high The upper bond.
     */
    ValuePtrs valuesBetween(const Value low, const Value high) const;

    /**
     * @brief Clears the content of the Axis.
     */
    void clear();

    /**
     * @brief Removes the given value from the axis if its exists.
     * @param value The value to remove.
     */
    void exclude(const Value value);

    /**
     * @brief Adds a new value to the axis.
     * @param value The value to add.
     */
    Index include(const Value value);

private:

    /**
     * @brief Give the iterator for a given value.
     * @throws If the value does not exist on the axis.
     */
    Values::iterator iteratorFor(const Value value) const;

    /**
     * @brief Gives the iterator of  value having its index.
     * @param index The index.
     * @throws If the axis is empty.
     */
    Values::iterator iteratorOf(const Index index) const;

    /**
     * @brief Gives the index of a specific value.
     * @param value The value.
     * @return The index of the value or nullptr if the axis does not contain the value.
     */
    OptionalIndex indexOf(const Value value) const;

    Values p_values;    // The values on the axis.

};

} // namespace rbs::coordinate_system

#endif // AXIS_H

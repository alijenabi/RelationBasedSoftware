//
//  Variant.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 19.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VARIANT_H
#define VARIANT_H

#include "../space/Index.h"
#include "../space/Point.h"
#include <string>
#include <variant>

/**
 * The variables namespace introduces a new form of variables specific to RBS.
 */
namespace rbs::variables {

/**
 * @brief The variable can hold all of the built-in variables, std::string and space Vector, Point, and Index.
 * @details TThe RBS software directly works with the physical and mathematical concepts that may require us to work
 *              with different space dimensions, scales, or integers. Developing open-ended classes allowing to work
 *              with all the above types upon researcher need is cumbersome merely because C++ is a strongly-typed
 *              language. This removes this difficulty by allowing storing different types in the same memory space.
 */
class Variant
{
public:

    /**
     * @brief The constructor.
     */
    Variant();

    /**
     * @brief The copy constructor.
     * @param other The other Variant to copy.
     */
    Variant(const Variant & other);

    /**
     * @brief The move constructor.
     * @param other The other Variant to move.
     */
    Variant(Variant && other);

    /**
     * @brief The constructor for underling types.
     * @tparam T The underline type.
     * @param value The new value.
     */
    template<typename T>
    Variant(const T& value)
        : p_value{ value }
    {
        p_id = static_cast<TypeID>(p_value.index() + 1);
    }

    /**
     * @brief Move/copy assign operator.
     * @param other The other Variant to move or copy.
     * @return The new Variant.
     */
    Variant &operator =(Variant other);

    /**
     * @brief The assignment operation for underling types.
     * @tparam T The underline type.
     * @param value The new value.
     */
    template<typename T>
    Variant &operator =(const T& value) {
        p_value = value;
        p_id = static_cast<TypeID>(p_value.index() + 1);
        return *this;
    }

    /**
     * @brief Whether the value is set or not.
     */
    bool hasValue() const;

    /**
     * @brief Whether the value is empty (not set) or not.
     */
    bool isEmpty() const;

    /**
     * @brief Gives the value if not empty.
     * @tparam T The underline type.
     * @throws If the value is empty.
     * @throws If the requested type is not the same as the set type.
     */
    template<typename T>
    const T& value() const {
        if (isEmpty())
            throw std::runtime_error("The Variant has no value.");
        if (index() != p_value.index())
            throw std::runtime_error("The set value has a different type.");
        return std::get<T>(p_value);
    }

    /**
     * @brief Clears the value (i.e., makes the variant empty).
     */
    void clear();

    /**
     * @brief Sets a new value.
     * @tparam T The underline type.
     * @param value The new value.
     */
    template<typename T>
    void setValue(const T& value) {
        p_value = value;
        p_id = static_cast<TypeID>(p_value.index() + 1);
    }

    bool operator==(const Variant &other) const;
    bool operator!=(const Variant &other) const;

    /**
     * @brief Gives the string representation.
     */
    operator std::string() const;

protected:

    /**
     * @brief Gives the string representation of the value.
     */
    std::string value_to_string() const;

    /**
     * @brief Gives the string representation of the type.
     */
    std::string type_to_string() const;

private:
    using ValueType = std::variant<space::Vector<1>,   space::Vector<2>, space::Vector<3>,
                                   space::Point<1>,    space::Point<2>,  space::Point<3>,
                                   space::Index<1>,    space::Index<2>,  space::Index<3>,
                                   long double,        double,           float,
                                   unsigned long long, unsigned long,    unsigned int, unsigned short, unsigned char,
                                   long long,          long,             int,          short,          char,
                                   bool,               std::string>;
    /**
     * @brief The Type ID defines the set value type.
     * @note Zero ( 0 ) denotes not set and (TypeID - 1 == std::variant index)
     */
    enum class TypeID {
        None,
        Vector1D, Vector2D, Vector3D,
        Point1D, Point2D, Point3D,
        Index1D, Index2D, Index3D,
        LongDouble, Double, Float,
        UnsignedLongLong, UnsignedLong, UnsignedInt, UnsignedShort, UnsignedChar,
        LongLong, Long, Int, Short, Char,
        Bool, String
    };

    /**
     * @brief Gives the equivalent std::index of the value.
     */
    std::size_t index() const;

    /**
     * @brief Swap the other with this object.
     */
    void swap(Variant & other);

    TypeID p_id;        // The type id of the value
    ValueType p_value;  // the holding value.

};

std::ostream & operator << (std::ostream & out, const Variant & variant);

} // namespace rbs::variables


#endif // VARIANT_H

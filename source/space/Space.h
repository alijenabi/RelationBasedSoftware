//
//  Space.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.01.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef SPACE_H
#define SPACE_H

#include <algorithm>
#include <string>

/**
 * The space namespace provides the means to handle matters related to the mathematical space.
 */
namespace rbs::space {

/**
 * @brief Whether two paramenters are close enoght to be called equal or not
 * @param p1 The first parameter.
 * @param p2 The second paramenter.
 * @return True, if parameters are close to each other, false otherwise.
 */
template<typename T>
constexpr bool inline areEqual(T p1, T p2);

/**
 * @brief Whether the value is close enoght to zero or not.
 * @param value The value to check.
 * @return True, if the value is close to zero false otherwise.
 */
template<typename T>
constexpr bool isZero(T value);

/**
 * @brief The Space class provides the means to manage any given space by controlling the values
 *        of the linearly independent dimension of that space.
 * @details ​Since any simulation will be defined over a space (i.g. 2D), flexibility in the choice
 *          of space of any order is crucial to any simulator. This class does provide the required
 *          flexibility.
 */
template<typename T, unsigned char Dimension>
class Space {
public:

    using Index = unsigned char;

    /**
     * @brief The constructor.
     */
    Space();

    /**
     * @brief The constructor.
     * @param values The initializer list.
     * @throws If the values has any size but the size of space.
     */
    Space(std::initializer_list<T> values);

    /**
     * @brief The copy constructor.
     * @param other The other Space to copy.
     */
    Space(const Space & other);

    /**
     * @brief The move constructor.
     * @param other The other Space to move.
     */
    Space(Space && other);

    /**
     * @brief Default virtual destructor.
     */
    virtual ~Space() = default;

    /**
     * @brief Gives the dimension of the space.
     */
    constexpr unsigned char dimension() const;

    /**
     * @brief Gives the name of the class.
     */
    virtual std::string name() const;

    /**
     * @brief Move/copy assign operator.
     * @param other The other Space to move or copy.
     * @return The new Space.
     */
    Space &operator =(Space other);

    T &operator [](Index index);
    const T & operator [](Index index) const;

    constexpr friend inline bool operator==(const Space &v1, const Space &v2){
        for(unsigned char i = 0; i < Dimension; ++i)
            if(!areEqual(v1.p_components[i], v2.p_components[i]))
                return false;
        return true;
    }
    constexpr friend inline bool operator!=(const Space &v1, const Space &v2) {
        return !operator==(v1, v2);
    }

    operator std::string() const;

protected:

    /**
     * @brief Gives the pointer to the begin of the space components.
     */
    const T * begin() const;

    /**
     * @brief Gives the pointer to the end of the space components.
     */
    const T *end() const;

private:

    /**
     * @brief Swaps the content of this object with an other objec of same type.
     * @param other The other instance.
     */
    void swap(Space & other);

    T p_components[Dimension];

};

} // namespace rbs::space

#include "SpaceImp.h"

#endif // SPACE_H

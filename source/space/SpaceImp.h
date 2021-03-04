//
//  SpaceImp.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.01.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef SPACEIMP_H
#define SPACEIMP_H

#ifndef SPACE_H
#   include "Space.h"
#endif // SPACE_H

namespace rbs::space {

template<typename T>
constexpr bool inline areEqual(T p1, T p2) {
    constexpr auto abs = [](const T & p) { return (p >= 0)? p : -p;};
    constexpr auto min = [](const T & p1, const T & p2) { return (p1 < p2)? p1 : p2;};

    if (p1 == p2)
        return true;
    if (typeid(T) == typeid(double))
        return abs(p1 - p2) * 100000000000. <= min(abs(p1), abs(p2));
    if (typeid(T) == typeid(float))
        return abs(p1 - p2) * 10000.f <= min(abs(p1), abs(p2));
    return false;
}

template<typename T>
constexpr bool isZero(T value) {
    constexpr auto abs = [](const T & p) { return (p >= 0)? p : -p;};
    constexpr auto cst = [abs](const T & p) { return static_cast<size_t>(abs(p));};

    if (abs(value) >= 1)
        return false;
    if (typeid(T) == typeid(double))
        return cst(value * 10000000000.) == 0;
    if (typeid(T) == typeid(float))
        return cst(value * 10000.f) == 0;
    return value == 0;
}

template<typename T, unsigned char Dimension>
Space<T, Dimension>::Space() {}

template<typename T, unsigned char Dimension>
Space<T, Dimension>::Space(std::initializer_list<T> values) {
    if (values.size() != Dimension)
        throw std::range_error("Wrong number of inputs " + std::to_string(values.size())
                               + " to create a " + std::to_string(Dimension) + "D Space.");
    std::copy(values.begin(), values.end(), std::begin(p_components));
}

template<typename T, unsigned char Dimension>
Space<T, Dimension>::Space(const Space &other) {
    std::copy(std::begin(other.p_components),
              std::end(other.p_components),
              std::begin(p_components));
}

template<typename T, unsigned char Dimension>
Space<T, Dimension>::Space(Space &&other) {
    std::move(std::begin(other.p_components),
              std::end(other.p_components),
              std::begin(p_components));
}

template<typename T, unsigned char Dimension>
constexpr unsigned char Space<T, Dimension>::dimension() const {
    return Dimension;
}

template<typename T, unsigned char Dimension>
std::string Space<T, Dimension>::name() const {
    return "Space";
}

template<typename T, unsigned char Dimension>
Space<T, Dimension> &Space<T, Dimension>::operator =(Space other) {
    swap(other);
    return *this;
}

template<typename T, unsigned char Dimension>
T &Space<T, Dimension>::operator [](Space::Index index) {
    if(index > Dimension)
        throw std::range_error("Out of range index.");
    return p_components[index];
}

template<typename T, unsigned char Dimension>
const T &Space<T, Dimension>::operator [](Space::Index index) const {
    if(index > Dimension)
        throw std::range_error("Out of range index.");
    return p_components[index];
}

template<typename T, unsigned char Dimension>
Space<T, Dimension>::operator std::string() const {
    std::string ans = name() + "{";
    for(Index itr = 0; itr < Dimension; ++itr)
        if (itr + 1 != Dimension) {
            ans = ans + std::to_string(p_components[itr]) + ", ";
        } else {
            ans = ans + std::to_string(p_components[itr]) + "}";
        }
    return ans;
}

template<typename T, unsigned char Dimension>
void Space<T, Dimension>::swap(Space &other) {
    std::swap_ranges(std::begin(p_components), std::end(p_components), std::begin(other.p_components));
}

template<typename T, unsigned char Dimension>
const T * Space<T, Dimension>::begin() const {
    return std::begin(p_components);
}

template<typename T, unsigned char Dimension>
const T * Space<T, Dimension>::end() const {
    return std::end(p_components);
}

template<typename T, unsigned char Dimension>
std::ostream & operator << (std::ostream & out, const Space<T, Dimension> & space) {
    return out << std::string(space);
}

} // namespace rbs::space

#endif // SPACEIMP_H

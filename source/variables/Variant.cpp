//
//  Variant.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 19.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Variant.h"

namespace rbs::variables {

Variant::Variant()
    : p_id{ TypeID::None }
    , p_value{}
{
}

Variant::Variant(const Variant &other) = default;

Variant::Variant(Variant &&other) {
    p_id = std::move(other.p_id);
    p_value = std::move(other.p_value);
}

bool Variant::hasValue() const {
    return p_id != TypeID::None;
}

bool Variant::isEmpty() const {
    return p_id == TypeID::None;
}

void Variant::clear() {
    p_id = TypeID::None;
}

bool Variant::operator==(const Variant &other) const {
    if( p_id == other.p_id ) {
        switch (p_id) {
        case TypeID::None:              return true;
        case TypeID::Vector1D:          return std::get<space::Vector<1> >(p_value)     == std::get<space::Vector<1> >(other.p_value);
        case TypeID::Vector2D:          return std::get<space::Vector<2> >(p_value)     == std::get<space::Vector<2> >(other.p_value);
        case TypeID::Vector3D:          return std::get<space::Vector<3> >(p_value)     == std::get<space::Vector<3> >(other.p_value);
        case TypeID::Point1D:           return std::get<space::Point<1> >(p_value)      == std::get<space::Point<1> >(other.p_value);
        case TypeID::Point2D:           return std::get<space::Point<2> >(p_value)      == std::get<space::Point<2> >(other.p_value);
        case TypeID::Point3D:           return std::get<space::Point<3> >(p_value)      == std::get<space::Point<3> >(other.p_value);
        case TypeID::Index1D:           return std::get<space::Index<1> >(p_value)      == std::get<space::Index<1> >(other.p_value);
        case TypeID::Index2D:           return std::get<space::Index<2> >(p_value)      == std::get<space::Index<2> >(other.p_value);
        case TypeID::Index3D:           return std::get<space::Index<3> >(p_value)      == std::get<space::Index<3> >(other.p_value);
        case TypeID::LongDouble:        return std::get<long double>(p_value)           == std::get<double>(other.p_value);
        case TypeID::Double:            return std::get<double>(p_value)                == std::get<double>(other.p_value);
        case TypeID::Float:             return std::get<float>(p_value)                 == std::get<float>(other.p_value);
        case TypeID::UnsignedLongLong:  return std::get<unsigned long long>(p_value)    == std::get<unsigned long long>(other.p_value);
        case TypeID::UnsignedLong:      return std::get<unsigned long>(p_value)         == std::get<unsigned long>(other.p_value);
        case TypeID::UnsignedInt:       return std::get<unsigned int>(p_value)          == std::get<unsigned int>(other.p_value);
        case TypeID::UnsignedShort:     return std::get<unsigned short>(p_value)        == std::get<unsigned short>(other.p_value);
        case TypeID::UnsignedChar:      return std::get<unsigned char>(p_value)         == std::get<unsigned char>(other.p_value);
        case TypeID::LongLong:          return std::get<long long>(p_value)             == std::get<long long>(other.p_value);
        case TypeID::Long:              return std::get<long>(p_value)                  == std::get<long>(other.p_value);
        case TypeID::Int:               return std::get<int>(p_value)                   == std::get<int>(other.p_value);
        case TypeID::Short:             return std::get<short>(p_value)                 == std::get<short>(other.p_value);
        case TypeID::Char:              return std::get<char>(p_value)                  == std::get<char>(other.p_value);
        case TypeID::Bool:              return std::get<bool>(p_value)                  == std::get<bool>(other.p_value);
        case TypeID::String:            return std::get<std::string>(p_value)           == std::get<std::string>(other.p_value);
        default: throw std::out_of_range("The type of the Variant is not recognized.");
        }
    }
    return false;
}

bool Variant::operator!=(const Variant &other) const {
    return !operator==(other);
}

variables::Variant::operator std::string() const {
    std::string ans =  "Variant:{";
    if(isEmpty()) {
        ans = ans + "empty";
    } else {
        ans = ans + "type: " + type_to_string() + ", value: " + value_to_string();
    }
    return ans + "}";
}

std::string Variant::value_to_string() const {
    switch (p_id) {
    case TypeID::None:              return "uninitialized";
    case TypeID::Vector1D:          return std::string(std::get<space::Vector<1> >(p_value));
    case TypeID::Vector2D:          return std::string(std::get<space::Vector<2> >(p_value));
    case TypeID::Vector3D:          return std::string(std::get<space::Vector<3> >(p_value));
    case TypeID::Point1D:           return std::string(std::get<space::Point<1> >(p_value));
    case TypeID::Point2D:           return std::string(std::get<space::Point<2> >(p_value));
    case TypeID::Point3D:           return std::string(std::get<space::Point<3> >(p_value));
    case TypeID::Index1D:           return std::string(std::get<space::Index<1> >(p_value));
    case TypeID::Index2D:           return std::string(std::get<space::Index<2> >(p_value));
    case TypeID::Index3D:           return std::string(std::get<space::Index<3> >(p_value));
    case TypeID::LongDouble:        return std::to_string(std::get<long double>(p_value));
    case TypeID::Double:            return std::to_string(std::get<double>(p_value));
    case TypeID::Float:             return std::to_string(std::get<float>(p_value));
    case TypeID::UnsignedLongLong:  return std::to_string(std::get<unsigned long long>(p_value));
    case TypeID::UnsignedLong:      return std::to_string(std::get<unsigned long>(p_value));
    case TypeID::UnsignedInt:       return std::to_string(std::get<unsigned int>(p_value));
    case TypeID::UnsignedShort:     return std::to_string(std::get<unsigned short>(p_value));
    case TypeID::UnsignedChar:      return std::to_string(std::get<unsigned char>(p_value));
    case TypeID::LongLong:          return std::to_string(std::get<long long>(p_value));
    case TypeID::Long:              return std::to_string(std::get<long>(p_value));
    case TypeID::Int:               return std::to_string(std::get<int>(p_value));
    case TypeID::Short:             return std::to_string(std::get<short>(p_value));
    case TypeID::Char:              return std::to_string(std::get<char>(p_value));
    case TypeID::Bool:              return std::to_string(std::get<bool>(p_value));
    case TypeID::String:            return std::get<std::string>(p_value);
    default: throw std::out_of_range("The type of the Variant is not recognized.");
    }
}

std::string Variant::type_to_string() const {
    switch (p_id) {
    case TypeID::None:              return "none";
    case TypeID::Vector1D:          return "space::Vector<1>";
    case TypeID::Vector2D:          return "space::Vector<2>";
    case TypeID::Vector3D:          return "space::Vector<3>";
    case TypeID::Point1D:           return "space::Point<1>";
    case TypeID::Point2D:           return "space::Point<2>";
    case TypeID::Point3D:           return "space::Point<3>";
    case TypeID::Index1D:           return "space::Index<1>";
    case TypeID::Index2D:           return "space::Index<2>";
    case TypeID::Index3D:           return "space::Index<3>";
    case TypeID::LongDouble:        return "long double";
    case TypeID::Double:            return "double";
    case TypeID::Float:             return "float";
    case TypeID::UnsignedLongLong:  return "unsigned long long";
    case TypeID::UnsignedLong:      return "unsigned long";
    case TypeID::UnsignedInt:       return "unsigned int";
    case TypeID::UnsignedShort:     return "unsigned short";
    case TypeID::UnsignedChar:      return "unsigned char";
    case TypeID::LongLong:          return "long long";
    case TypeID::Long:              return "long";
    case TypeID::Int:               return "int";
    case TypeID::Short:             return "short";
    case TypeID::Char:              return "char";
    case TypeID::Bool:              return "bool";
    case TypeID::String:            return "std::string";
    default: throw std::out_of_range("The type of the Variant is not recognized.");
    }
}

std::size_t Variant::index() const {
    return static_cast<std::size_t>(p_id) - 1;
}

void Variant::swap(Variant &other) {
    std::swap(p_id, other.p_id);
    std::swap(p_value, other.p_value);
}

Variant &Variant::operator =(Variant other) {
    swap(other);
    return *this;
}

std::ostream &operator <<(std::ostream &out, const Variant &variant) {
    return out << std::string(variant);
}


} // namespace rbs::variant

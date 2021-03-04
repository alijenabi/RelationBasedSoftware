//
//  VTKAttributeSection.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 15.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "VTKAttributeSection.h"

namespace rbs::exporting {

VTKAttributeSection::VTKAttributeSection(const std::string &name, const std::string &attributeName, const VTKAttributeSection::AttributeType attribute, const VTKAttributeSection::SourceType source)
    : VTKSection(name + "_" + filterAttributeName(attributeName), VTKSection::Type::Attribute)
    , p_name{filterAttributeName(attributeName)}
    , p_type{attribute}
    , p_source{source}
    , p_dataType{DataType::Double}
{
    bodyPtr()->setSeperator(" ");
}

VTKAttributeSection::VTKAttributeSection(const std::string &path, const std::string &name, const std::string &attributeName, const VTKAttributeSection::AttributeType attribute, const VTKAttributeSection::SourceType source)
    : VTKAttributeSection(path+name, attributeName, attribute, source)
{}

const std::string VTKAttributeSection::filterAttributeName(const std::string &name) {
    std::string ans = name;

    // removing unexpected signs from the attribute name!
    ans.erase(std::remove_if(ans.begin(), ans.end(), [](char c){
        return c == ' ' || c == '.' || c == '!' || c == '@'|| c == '#'|| c == '$'
                || c == '%' || c == '^' || c == '&' || c == '*' || c == '('
                || c == ')' || c == '`' || c == '~' || c == '+' || c == '='
                || c == '/' || c == '>' || c == '<' || c == ',';}));

    // If the file name is set to empty string change it to "un-named".
    if (ans.compare("") == 0)
        ans = "un-named-file";

    // Removing any leading space in the name and path.
    const std::string whitespaces = " \n\r\t\f\v";
    const auto nonSpaceIndex = ans.find_first_not_of(whitespaces);
    ans = ans.substr(nonSpaceIndex);

    return ans;
}

std::string VTKAttributeSection::header() const {
    switch (p_type) {
    case AttributeType::Scalar:
        return "SCALARS " + p_name + " " +  to_string(p_dataType) + " 1";
    case AttributeType::Vector:
        return "VECTORS " + p_name + " " + to_string(p_dataType);
    case AttributeType::Tensor:
        return "TENSORS " + p_name + " " + to_string(p_dataType);
    }
    throw std::runtime_error("The source type is not defined.");
}

const std::string &VTKAttributeSection::attributeName() const {
    return p_name;
}

VTKAttributeSection::AttributeType VTKAttributeSection::attributeType() const {
    return p_type;;
}

VTKAttributeSection::Size VTKAttributeSection::size() const {
    return body().cellCount() / cellPerType(p_type);
}

VTKAttributeSection::SourceType VTKAttributeSection::source() const {
    return p_source;
}

void VTKAttributeSection::setAttributeType(const VTKAttributeSection::AttributeType type) {
    if (bodyPtr()->cellCount())
        throw std::runtime_error("Cannot change the attribute type if the body is not empty.");
    p_type = type;
    bodyPtr()->maxColumn() = cellPerRow(p_type);
}

void VTKAttributeSection::setName(const std::string &name) {
    p_name = filterAttributeName(name);
}

void VTKAttributeSection::setSource(const VTKAttributeSection::SourceType source) {
    p_source = source;
}

VTKSection::CellNumber VTKAttributeSection::cellPerRow(const VTKAttributeSection::AttributeType type){
    switch (type) {
    case AttributeType::Scalar:
    case AttributeType::Vector:      return 9;
    case AttributeType::Tensor:      return 3;
    }
    throw std::range_error("The type of the section is out of known range.");
}

VTKAttributeSection::Size VTKAttributeSection::cellPerType(const VTKAttributeSection::AttributeType type) {
    switch (type) {
    case AttributeType::Scalar:     return 1;
    case AttributeType::Vector:     return 3;
    case AttributeType::Tensor:     return 9;
    default:                        break;
    }
    throw std::range_error("The name of the attribute type is out of known range.");
}

const std::string VTKAttributeSection::to_string(VTKAttributeSection::DataType type) {
    switch (type) {
    case DataType::Int:     return "int";
    case DataType::Float:   return "float";
    case DataType::Double:  return "double";
    }
    throw std::range_error("The data type of the section is not known.");
}

std::ostream &operator <<(std::ostream &out, const VTKAttributeSection::AttributeType &type)
{
    using Type = VTKAttributeSection::AttributeType;
    switch (type) {
    case Type::Scalar:        out << "scalar";        break;
    case Type::Vector:        out << "vector";        break;
    case Type::Tensor:        out << "tensor";        break;
    default:                                          break;
    }
    return out;
}

std::ostream &operator <<(std::ostream &out, const VTKAttributeSection::SourceType &type)
{
    using DataType = VTKAttributeSection::SourceType;
    switch (type) {
    case DataType::Point:        out << "point";        break;
    case DataType::Cell:         out << "cell";         break;
    default:                                            break;
    }
    return out;
}

} // namespace rbs::exporting


//
//  VTKSection.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 11.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "VTKSection.h"

namespace rbs::exporting {

VTKSection::VTKSection(const std::string &name, VTKSection::Type type)
    : p_bodyPtr{std::make_shared<Body>(name, "tempvtk")}
    , p_type{type}
{
    p_bodyPtr->maxColumn() = tableCellPerRow(p_type);
}

VTKSection::VTKSection(const std::string &path,
                       const std::string &name,
                       VTKSection::Type type)
    : VTKSection{path+name, type}
{
}

VTKSection::CellNumber VTKSection::tableCellPerRow(const VTKSection::Type type) {
    using Type = VTKSection::Type;
    switch (type) {
    case Type::CellTypes:   return 1;
    case Type::Points:      return 9;
    case Type::Attribute:
    case Type::Cells:       return 0; // Zero will be interpreted as no limit by CSV file.
    }
    throw std::range_error("The type of the section is out of known range.");
}

std::string VTKSection::to_string(const VTKSection::Type type) {
    using Type = VTKSection::Type;
    switch (type) {
    case Type::Points:        return "points";
    case Type::Cells:         return "cells";
    case Type::CellTypes:     return "cell_types";
    case Type::Attribute:     return "attribute";
    default:                  break;
    }
    return "";
}

std::string VTKSection::header() const {
    throw std::logic_error("The VTKSection dow not have a header.");
}

const VTKSection::Body & VTKSection::body() const {
    return *p_bodyPtr;
}

const std::string VTKSection::path() const {
    return body().fullName(true);
}

VTKSection::Type VTKSection::type() const {
    return p_type;
}

bool VTKSection::removeBody() {
    return bodyPtr()->remove();
}

const VTKSection::Body_ptr &VTKSection::bodyPtr() {
    return p_bodyPtr;
}


std::ostream &operator <<(std::ostream &out, const VTKSection::Type &type) {
    using Type = VTKSection::Type;
    switch (type) {
    case Type::Points:        out << "points";        break;
    case Type::Cells:         out << "cells";         break;
    case Type::CellTypes:     out << "cell_types";    break;
    case Type::Attribute:     out << "attribute";     break;
    default:                                          break;
    }
    return out;
}

} // namespace rbs::exporting

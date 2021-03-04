//
//  VTKCellSection.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 11.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "VTKCellSection.h"

namespace rbs::exporting {

VTKCellSection::VTKCellSection(const std::string &name)
    : VTKSection(name + "_" + VTKSection::to_string(Type::Cells), VTKSection::Type::Cells)
{
    bodyPtr()->setSeperator(" ");
}

VTKCellSection::VTKCellSection(const std::string &path, const std::string &name)
    : VTKCellSection(path + name)
{
}

VTKCellSection::CellIndex VTKCellSection::cellCount() const {
    return body().rowIndex();
}

std::string VTKCellSection::header() const{
    return "CELLS " + std::to_string(body().rowIndex()) + " " + std::to_string(body().cellCount());
}

VTKCellSection::Size VTKCellSection::pointCount() const {
    return body().cellCount() - cellCount();
}

} // namespace rbs::exporting

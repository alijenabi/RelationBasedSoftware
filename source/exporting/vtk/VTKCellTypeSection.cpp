//
//  VTKCellTypeSection.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 11.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "VTKCellTypeSection.h"

namespace rbs::exporting {

VTKCellTypeSection::VTKCellTypeSection(const std::string &name)
    : VTKSection{name + "_" + VTKSection::to_string(Type::CellTypes), Type::CellTypes}
{
    bodyPtr()->setSeperator(" ");
}

VTKCellTypeSection::VTKCellTypeSection(const std::string &path, const std::string &name)
    : VTKCellTypeSection{path + name}
{
}

VTKCellTypeSection::Size VTKCellTypeSection::cellCount() const {
    return body().rowIndex();
}

std::string VTKCellTypeSection::header() const {
    return "CELL_TYPES " + std::to_string(body().rowIndex());
}

VTKCellTypeSection::CellIndex VTKCellTypeSection::appendCell(const CellType type) {
    bodyPtr()->appendCell(Cell::code(type), false, false); // It will be flushed on the assembly time.
    return bodyPtr()->rowIndex();
}

} // namespace rbs::exporting

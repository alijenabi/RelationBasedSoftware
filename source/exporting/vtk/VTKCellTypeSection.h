//
//  VTKCellTypeSection.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 11.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VTKCELLTYPESECTION_H
#define VTKCELLTYPESECTION_H

#include "VTKCell.h"
#include "VTKSection.h"

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

/**
 * The VTKCellTypeSection class provides the means to handle exporting cell-type map to the tempvtk file.
 */
class VTKCellTypeSection: public VTKSection
{
public:

    using Cell = VTKCell;
    using CellIndex = unsigned long;
    using CellType = VTKCell::Type;
    using Size = unsigned long;

    /**
     * @brief The constructor.
     * @param name The name of the parent VTK file (the path to the file included).
     */
    explicit VTKCellTypeSection(const std::string &name);

    /**
     * @brief The constructor.
     * @param path The directory path of the file.
     * @param name The name of the parent VTK file.
     */
    explicit VTKCellTypeSection(const std::string &path, const std::string &name);

    /**
     * @brief Gives the number of cells inside the body.
     */
    Size cellCount() const;

    /**
     * @brief Gives the header of the points section.
     */
    virtual std::string header() const override;

    /**
     * @brief Appends a cell to the section having the type of the cell
     * @param type The type of the cell.
     * @returns The number of cells currently exist on the body.
     */
    CellIndex appendCell(const CellType type);

};

} // namespace rbs::exporting

#endif // VTKCELLTYPESECTION_H

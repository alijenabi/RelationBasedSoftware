//
//  VTKFile.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VTKFILE_H
#define VTKFILE_H

#include "TextFile.h"
#include "vtk/VTKAttributeSection.h"
#include "vtk/VTKCell.h"
#include "vtk/VTKCellSection.h"
#include "vtk/VTKCellTypeSection.h"
#include "vtk/VTKPointSection.h"
#include <map>
#include <vector>

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

class VTKFile: public TextFile
{
public:

    using AttributeSource = VTKAttributeSection::SourceType;
    using Cell = VTKCell;
    using CellType = VTKCell::Type;
    using CellPointIndex = unsigned long;
    using Point = std::vector<double>;
    using PointIndex = unsigned long;

    /**
     * @brief The constructor.
     * @param name The name of the file (the path to the file included).
     * @param extension The extention of the file without the dot(.).
     */
    explicit VTKFile(const std::string &name,
                     const std::string &extension = "vtk");

    /**
     * @brief The constructor.
     * @param path The directory path of the file.
     * @param name The name of the file.
     * @param extension The extention of the file without the dot(.).
     * @param seperator The Comma-separated values, seperator string.
     */
    explicit VTKFile(const std::string &path,
                     const std::string &name,
                     const std::string &extension);

    void append(const std::string & text) = delete;
    template<typename DATA>
    void append(const DATA& data) = delete;
    void nextLine() = delete;

    /**
     * @brief Whether the configuration is locked or not.
     */
    bool isLocked() const;

    /**
     * @brief Whether the file exporting data on the initial configuration or not.
     */
    bool isSharingConfiguration() const;

    /**
     * @brief Locks the configuration of the file.
     */
    void lock();

    /**
     * @brief Append a cell to the vtk sections' body's files (temp files).
     * @note This will not add any point to the point section but instead maps them to the new cell.
     * @param type The type of the cell.
     * @param cellPointsMap The points-vertices map of the cell.
     * @throws If the number of points on the map is not in the acceptable range.
     * @throws If the configuration is locked.
     */
    void appendCell(const CellType type,
                    const std::vector<PointIndex> & cellPointsMap);

    /**
     * @brief Append a cell to the vtk sections' body's files (temp files).
     * @note This will not add points to the point section and maps them to the new cell.
     * @param type The type of the cell.
     * @param points The location of cell's vertices.
     * @throws If the number of points is not in the acceptable range.
     * @throws If the configuration is locked.
     */
    void appendCell(const CellType type,
                    const std::vector<Point> & points);

    /**
     * @brief Append a cell to the vtk sections' body's files (temp files).
     * @note This will not new point(s) to the point section but does not maps them to the new cell automaticly.
     * @note The points can be added afterward by other cells too.
     * @note The map should be complete (including the current added points)
     * @param type The type of the cell.
     * @param points The location of cell's vertices.
     * @param cellPointsMap The points-vertices map of the cell.
     * @throws If the number of points on the map is not in the acceptable range.
     * @throws If the configuration is locked.
     */
    void appendCell(const CellType type,
                    const std::vector<Point> & points,
                    const std::vector<PointIndex> & cellPointsMap);

    /**
     * @brief Assembles the sections headers and bodies to create the vtk file.
     * @note The vtk file can be assembled any time if the sections are not removed.
     * @param removeTemp If true, removes the temp files of the section bodies afterward.
     * @throws If the size of the sections' content are not compatible.
     */
    void assemble(bool removeSections = true);

    /**
     * @brief Removes the file without move it to bin/trash directory.
     * @return True if successful, false otherwise.
     */
    bool remove();

    /**
     * @brief Sets the data title at the header of the vtk file.
     * @note The title can be up to 150 character, more than that will be cut off.
     * @param title The vtk title.
     */
    void setDataTitle(const std::string & title);

    /**
     * @brief Set the series initial configuration and export data only on it.
     * @note Any current configuration (cell, point, cell_type sections) will be ignored.
     * @param file The file containg the initial configuration.
     */
    void shareConfiguration(const std::shared_ptr<VTKFile> & file);

    /**
     * @brief Gives read-only access to attribute sections.
     * @param name The name of the attribute.
     */
    const VTKAttributeSection & operator[](const std::string & name) const;

    /**
     * @brief Gives direct access to attribute section.
     * @note It will create an attrubute section if not found.
     * @param name The name of the attribute.
     */
    VTKAttributeSection & operator[](const std::string & name);

protected:

    using AttributeList = std::map<std::string, std::shared_ptr<VTKAttributeSection> >;
    using Section = VTKSection;
    using SectionName = VTKSection::Type;

    /**
     * @brief Assembles the configuration of the file (i.e., point, cell, and cell_type sections).
     * @param lock Whether to lock the file or not.
     */
    void assembleConfiguration(bool lock = false);

    /**
     * @brief Removes the temperory files related to this vtk (i.e., The section's bodies)
     * @return True if successful, false otherwise.
     */
    bool removeTemperoryFiles();

    /**
     * @brief Computes whether the section's bodies have a compatible sizes or not.
     */
    bool sectionBodySizesMatch();

private:

    AttributeList p_attributeSections;
    VTKCellSection p_cellSection;
    VTKCellTypeSection p_cellTypeSection;
    std::shared_ptr<VTKFile> p_configuration;
    std::string p_dataTitle; // <= The data title, at the header of the vtk file.
    bool p_locked;
    VTKPointSection p_pointSection;

};

} // namespace rbs::exporting

#endif // VTKFILE_H

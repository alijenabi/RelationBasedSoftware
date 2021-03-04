//
//  VTKFile.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "VTKFile.h"

namespace rbs::exporting {

VTKFile::VTKFile(const std::string &name, const std::string &extension)
    : TextFile{name, extension}
    , p_attributeSections{}
    , p_cellSection{name}
    , p_cellTypeSection{name}
    , p_configuration{}
    , p_dataTitle{"Very important data."}
    , p_locked{false}
    , p_pointSection{name}
{
}

VTKFile::VTKFile(const std::string &path,
                 const std::string &name,
                 const std::string &extension)
    : VTKFile{path + name, extension}
{
}

bool VTKFile::isLocked() const {
    if(p_configuration)
        return p_configuration->p_locked;
    return p_locked;
}

bool VTKFile::isSharingConfiguration() const {
    return static_cast<bool>(p_configuration);
}

void VTKFile::lock() {
    if(p_configuration)
        p_configuration->p_locked = true;
    p_locked = true;
}

void VTKFile::appendCell(const VTKFile::CellType type,
                         const std::vector<VTKFile::PointIndex> &cellPointsMap) {
    if (p_locked)
        throw std::runtime_error("The vtk configuration is blocked, you cannot append to it.");

    if (p_configuration) {
        p_configuration->appendCell(type, cellPointsMap);
    } else {
        p_cellSection.appendCell(type, cellPointsMap);
        p_cellTypeSection.appendCell(type);
    }
}

void VTKFile::appendCell(const VTKFile::CellType type,
                         const std::vector<VTKFile::Point> &points) {
    if (p_locked)
        throw std::runtime_error("The vtk configuration is blocked, you cannot append to it.");

    if (p_configuration) {
        p_configuration->appendCell(type, points);
    } else {
        p_cellSection.appendCell(type, p_pointSection.appendPoints(points));
        p_cellTypeSection.appendCell(type);
    }
}

void VTKFile::appendCell(const VTKFile::CellType type,
                         const std::vector<VTKFile::Point> &points,
                         const std::vector<VTKFile::PointIndex> &cellPointsMap) {
    if (p_locked)
        throw std::runtime_error("The vtk configuration is blocked, you cannot append to it.");

    if (p_configuration) {
        p_configuration->appendCell(type, points, cellPointsMap);
    } else {
        p_pointSection.appendPoints(points);
        p_cellSection.appendCell(type, cellPointsMap);
        p_cellTypeSection.appendCell(type);
    }
}

void VTKFile::assemble(bool removeSections) {
    if (!sectionBodySizesMatch())
        throw std::runtime_error("The size of the sections' content are not compatible..");

    const auto pointSize = p_cellSection.pointCount();
    const auto cellSize = p_cellSection.cellCount();

    try {
        assembleConfiguration(false);
    } catch (const std::exception& e) {
        auto allRemoved = false;
        if (removeSections)
            if (removeTemperoryFiles())
                    allRemoved = remove();

        if (!allRemoved)
            throw std::runtime_error("Could not remove the files (sections' boies).");
        throw e.what();
    }

    // Attribute Section
    bool headerPrinted = false;
    for (const auto & attributeSection : p_attributeSections) {
        if (attributeSection.second->source() == VTKAttributeSection::SourceType::Point) {
            if(!headerPrinted) {
                TextFile::append("POINT_DATA " + std::to_string(pointSize), false);
                TextFile::nextLine(false);
                headerPrinted = true;
            }

            if (!TextFile::append(attributeSection.second->header(), false) || !TextFile::nextLine(false))
                throw std::runtime_error("Could not append the header of the attribute section.");

            if (attributeSection.second->attributeType() == VTKAttributeSection::AttributeType::Scalar) {
                TextFile::append("LOOKUP_TABLE default", false);
                TextFile::nextLine(false);
            }

            if (!TextFile::appendFileContent(attributeSection.second->body()))
                throw std::runtime_error("Could not append the body of the attribute section.");

            if (attributeSection.second->body().columnCount() != 0)
                TextFile::nextLine(false);

            TextFile::nextLine(true);
        }
    }

    headerPrinted = false;
    for (auto & attributeSection : p_attributeSections) {
        if (attributeSection.second->source() == VTKAttributeSection::SourceType::Cell) {
            if(!headerPrinted) {
                TextFile::append("CELL_DATA " + std::to_string(cellSize), false);
                TextFile::nextLine(false);
                headerPrinted = true;
            }

            if (!TextFile::append(attributeSection.second->header(), false) || !TextFile::nextLine(true))
                throw std::runtime_error("Could not append the header of the attribute section.");

            if (attributeSection.second->attributeType() == VTKAttributeSection::AttributeType::Scalar) {
                TextFile::append("LOOKUP_TABLE default", false);
                TextFile::nextLine(true);
            }

            if (!TextFile::appendFileContent(attributeSection.second->body()))
                throw std::runtime_error("Could not append the body of the attribute section.");

            if (attributeSection.second->body().columnCount() != 0)
                TextFile::nextLine(false);
            TextFile::nextLine(true);
        }
    }

    if(removeSections && !p_configuration)
        if(!removeTemperoryFiles())
            throw std::runtime_error("Could not remove the temp files (sections' boies).");
}

bool VTKFile::remove() {
    return removeTemperoryFiles() || TextFile::remove();
}

void VTKFile::setDataTitle(const std::string &title) {
    if (p_configuration) {
        p_configuration->setDataTitle(title);
    }
    if (title.length() < 150)
        p_dataTitle = title;
    else
        p_dataTitle = title.substr(0, 146) + "...";
}

void VTKFile::shareConfiguration(const std::shared_ptr<VTKFile> & file) {
    p_configuration = file;
}

const VTKAttributeSection &VTKFile::operator[](const std::string &name) const {
    return *p_attributeSections.at(name);
}

VTKAttributeSection &VTKFile::operator[](const std::string &name) {
    const auto itr = p_attributeSections.find(name);
    if (itr == p_attributeSections.end()) {
        p_attributeSections[name] = std::make_shared<VTKAttributeSection>(
                    this->name(true),
                    name,
                    VTKAttributeSection::Scalar,
                    VTKAttributeSection::SourceType::Point);
    }
    return *p_attributeSections[name];
}

void VTKFile::assembleConfiguration(bool block) {
    const auto exportOn = [this, block](const std::shared_ptr<VTKFile>& file) {
        // header
        TextFile::append("# vtk DataFile Version 2.0", false);
        TextFile::nextLine(false);
        TextFile::append(p_dataTitle, false);
        TextFile::nextLine(false);
        TextFile::append("ASCII", false);
        TextFile::nextLine(false);
        TextFile::append("DATASET UNSTRUCTURED_GRID", false);
        TextFile::nextLine(false);

        // Point Section
        if (!TextFile::append(file->p_pointSection.header(), false) || !TextFile::nextLine(false)
            || !TextFile::appendFileContent(file->p_pointSection.body()))
            throw std::runtime_error("Could not append the content of the point section.");
        if (file->p_pointSection.pointCount() % 3)
            TextFile::nextLine(false);
        TextFile::nextLine(false);

        // Cell Section
        if (!TextFile::append(file->p_cellSection.header(), false) || !TextFile::nextLine(false)
            || !TextFile::appendFileContent(file->p_cellSection.body()))
            throw std::runtime_error("Could not append the content of the cell section.");
        TextFile::nextLine(false);

        // Cell_Type Section
        if (!TextFile::append(file->p_cellTypeSection.header(), false) || !TextFile::nextLine(false)
            || !TextFile::appendFileContent(file->p_cellTypeSection.body()))
            throw std::runtime_error("Could not append the content of the cell type section.");
        TextFile::nextLine(true);

        if (block)
            file->lock();
    };

    if (p_configuration) {
        exportOn(p_configuration);
    } else {
        exportOn(std::shared_ptr<VTKFile>(this, [](auto){}));
    }
}

bool VTKFile::removeTemperoryFiles() {
    if (p_configuration) {
        p_cellSection.removeBody();
        p_pointSection.removeBody();
        p_cellTypeSection.removeBody();
    } else {
        if (!p_cellSection.removeBody()
                || !p_pointSection.removeBody()
                || !p_cellTypeSection.removeBody() )
            return false;
    }
    for (auto & attributeSection : p_attributeSections)
        if (!attributeSection.second->removeBody())
            return false;
    return true;
}

bool VTKFile::sectionBodySizesMatch() {
    const auto pointSize = (p_configuration)? p_configuration->p_pointSection.pointCount() : p_pointSection.pointCount();
    const auto cellSize = (p_configuration)? p_configuration->p_cellSection.cellCount() : p_cellSection.cellCount();
    if (p_configuration) {
        if (p_configuration->p_cellSection.pointCount() < pointSize
                || p_configuration->p_cellTypeSection.cellCount() != cellSize)
            return false;
    } else {
        if (p_cellSection.pointCount() < pointSize
                || p_cellTypeSection.cellCount() != cellSize)
            return false;
    }
    for (const auto & attributeSection : p_attributeSections) {
        if (attributeSection.second->source() == VTKAttributeSection::SourceType::Point
                && attributeSection.second->size() != pointSize)
            return false;
        else if (attributeSection.second->source() == VTKAttributeSection::SourceType::Cell
                 && attributeSection.second->size() != cellSize)
            return false;
    }
    return true;
}

} // namespace rbs::exporting

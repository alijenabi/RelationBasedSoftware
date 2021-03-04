//
//  VTKPointSection.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 11.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "VTKPointSection.h"

namespace rbs::exporting {

VTKPointSection::VTKPointSection(const std::string &name)
    : VTKSection{name + "_" + VTKSection::to_string(Type::Points), Type::Points}
{
    bodyPtr()->setSeperator(" ");
}

VTKPointSection::VTKPointSection(const std::string &path, const std::string &name)
    : VTKPointSection{path + name}
{
}

VTKPointSection::PointIndex VTKPointSection::pointCount() const {
    return body().cellCount() / 3;
}

std::string VTKPointSection::header() const
{
    return  "POINTS " + std::to_string(pointCount()) + " double";
}

VTKPointSection::PointIndex VTKPointSection::appendPoint(VTKPointSection::Dimension x,
                                                         VTKPointSection::Dimension y,
                                                         VTKPointSection::Dimension z){
    if (!bodyPtr()->appendCell(x, false, true)         // It will be flushed on the assembly time.
            || !bodyPtr()->appendCell(y, false, true)  // It will be flushed on the assembly time.
            || !bodyPtr()->appendCell(z, false, true)) // It will be flushed on the assembly time.
        throw std::runtime_error("Could not append one of the point dimension to the file.");
    return currentPointIndex();
}

VTKPointSection::PointIndex VTKPointSection::appendPoint(const VTKPointSection::Point &dimensions) {
    if (dimensions.size() != 3)
        throw std::range_error("The points of vtk has to have 3 dimensions (3D).");
    return appendPoint(dimensions[0], dimensions[1], dimensions[2]);
}

std::vector<VTKPointSection::PointIndex> VTKPointSection::appendPoints(std::vector<VTKPointSection::Dimension> dimensions){
    std::vector<PointIndex> ans;
    if(dimensions.empty())
        return ans;
    if(dimensions.size() % 3)
        throw std::range_error("The points of vtk has to have 3 dimensions (3D).");

    ans.reserve(dimensions.size() / 3);
    ans.emplace_back(pointCount());

    if ( bodyPtr()->appendRow(dimensions, false, true) != dimensions.size() ) // It will be flushed on the assembly time.
        throw std::runtime_error("Could not append one of the point dimension to the file.");

    const auto last = currentPointIndex();
    while(ans.back() < last)
        ans.emplace_back(ans.back() + 1);
    return ans;
}

VTKPointSection::PointIndex VTKPointSection::currentPointIndex() const {
    return body().cellCount() / 3 - 1;
}

} // namespace rbs::exporting

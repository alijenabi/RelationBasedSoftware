//
//  VTK.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 19.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VTK_H
#define VTK_H

#include "VTKFile.h"
#include "VTKFileSeries.h"

/**
 * The file_system namespace provides the means to handle files on the operating system file system.
 */
namespace rbs::exporting {

/**
 * The vtk namespace provides easy access to all of the namespaces realted to VTK files.
 */
namespace vtk {

using File = VTKFile;
using Point = VTKFile::Point;
using Cell = VTKCell;
using AttSource = VTKAttributeSection::SourceType;
using AttType = VTKAttributeSection::AttributeType;
using FileSeries = VTKFileSeries;

} // namespace vtk

} // namespace rbs::exporting

#endif // VTK_H

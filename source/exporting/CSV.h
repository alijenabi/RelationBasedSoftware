//
//  CSV.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 19.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef CSV_H
#define CSV_H

#include "CSVFile.h"
#include "CSVFileSeries.h"

/**
 * The file_system namespace provides the means to handle files on the operating system file system.
 */
namespace rbs::exporting {

/**
 * The vtk namespace provides easy access to all of the namespaces realted to VTK files.
 */
namespace rbs::csv {

using File = CSVFile;
using FileSeries = CSVFileSeries;

} // namespace rbs::csv

} // namespace rbs::exporting

#endif // CSV_H

//
//  TextFileSeries.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef CSVFILESERIES_H
#define CSVFILESERIES_H

#include "CSVFile.h"
#include "file_system/FileSeries.h"

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

/**
 * @brief The TextFileSeries class provides the means to handle exporting data to series of csv files.
 */
using CSVFileSeries = file_system::FileSeries<CSVFile>;

} // namespace rbs::exporting

#endif // CSVFILESERIES_H

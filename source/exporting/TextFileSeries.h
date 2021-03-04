//
//  TextFileSeries.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef TEXTFILESERIES_H
#define TEXTFILESERIES_H

#include "TextFile.h"
#include "file_system/FileSeries.h"

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

/**
 * @brief The TextFileSeries class provides the means to handle exporting data to series of text files.
 */
using TextFileSeries = file_system::FileSeries<TextFile>;

} // namespace rbs::exporting

#endif // TEXTFILESERIES_H

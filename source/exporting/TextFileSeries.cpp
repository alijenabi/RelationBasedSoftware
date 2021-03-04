//
//  TextFileSeries.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "TextFileSeries.h"

namespace exporting {

TextFileSeries::TextFileSeries(const std::string &basename,
                               const std::string &extension,
                               const Index allowedSize,
                               const bool override,
                               const bool fillVocations)
    : file_system::FileSeries<TextFile>(basename, extension, allowedSize, override, fillVocations)
{
}

TextFileSeries::TextFileSeries(const std::string &path,
                               const std::string &base_name,
                               const std::string &extension,
                               const Index allowedSize,
                               const bool override,
                               const bool fillVocations)
    : file_system::FileSeries<TextFile>(path, base_name, extension, allowedSize, override, fillVocations)
{
}

} // namespace exporting

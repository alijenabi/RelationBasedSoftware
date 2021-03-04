//
//  SingleFile.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 28.06.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "FileRegistar.h"
#include "SingleFile.h"

namespace rbs::exporting::file_system {

SingleFile::SingleFile(const std::string &name, const std::string &extension)
    : BaseFile(name, extension)
{
    cleanContent();
}

SingleFile::SingleFile(const std::string &path,
                       const std::string &name,
                       const std::string &extension)
    : SingleFile(path + name, extension)
{
}

SingleFile::~SingleFile()
{
}
bool SingleFile::cleanContent() noexcept
{
    try {
        using namespace rbs::exporting::file_system;
        return FileRegistar::apply(fullName(true), FileRegistar::Mode::Write, [](auto){ return true; }, true);
    } catch (...) {
        return false;
    }
    return true;
}

bool SingleFile::remove()
{
    return BaseFile::remove();
}

} // namespace rbs::exporting::file_system

//
//  TextFile.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "file_system/FileRegistar.h"
#include "TextFile.h"

namespace rbs::exporting {

using SingleFile = file_system::SingleFile;

TextFile::TextFile(const std::string &name, const std::string &extension)
    : SingleFile(name, extension)
{
}

TextFile::TextFile(const std::string &path, const std::string &name, const std::string &extension)
    : SingleFile(path, name, extension)
{
}

TextFile::CharCount TextFile::append(const char *c_str, const bool flush) {
    using file_system::FileRegistar;
    return FileRegistar::apply(fullName(true), FileRegistar::Mode::Append, [c_str](std::FILE* file){
        return std::fputs(c_str, file);
    }, flush);
}

TextFile::CharCount TextFile::append(const std::string &str, const bool flush) {
    return append(str.c_str(), flush);
}

bool TextFile::appendFileContent(const TextFile& other) {
    try {
        file_system::FileRegistar::append(other.fullName(true), fullName(true));
    }  catch (...) {
        return false;
    }
    return true;
}

bool TextFile::nextLine(const bool flush)
{
    return static_cast<bool>(append("\n", flush));
}

} // namespace rbs::exporting

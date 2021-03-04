//
//  BaseFile.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 10.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "BaseFile.h"
#include "FileRegistar.h"

namespace rbs::exporting::file_system {

BaseFile::BaseFile(const std::string &name, const std::string &extension)
    : p_extension{extension}
    , p_name{std::filesystem::path(name).filename()}
    , p_path{"."}
{
    // removing unexpected signs from extension!
    p_extension.erase(std::remove_if(p_extension.begin(), p_extension.end(), [](char c){
        return c == '.' || c == '!' || c == '@'|| c == '#'|| c == '$'
                || c == '%' || c == '^' || c == '&' || c == '*' || c == '('
                || c == ')' || c == '`' || c == '~' || c == '+' || c == '='
                || c == '/' || c == '>' || c == '<' || c == ',';}));

    // If the file name is set to empty string change it to "un-named".
    if (p_name.compare("") == 0)
        p_name = "un-named-file";

    const auto path = std::filesystem::path(name);
    if (path.has_parent_path() && path.parent_path().compare("") != 0)
        p_path = path.parent_path();

    // Removing any leading space in the name and path.
    const std::string whitespaces = " \n\r\t\f\v";
    auto nonSpaceIndex = p_path.find_first_not_of(whitespaces);
    p_path = p_path.substr(nonSpaceIndex);
    nonSpaceIndex = p_name.find_first_not_of(whitespaces);
    p_name = p_name.substr(nonSpaceIndex);
}

BaseFile::BaseFile(const std::string &path,
                   const std::string &name,
                   const std::string &extension)
    : BaseFile(path + name, extension)
{
}

const std::string BaseFile::directory() const
{
    return p_path + seperator;
}

const std::string & BaseFile::extension() const
{
    return p_extension;
}

const std::string BaseFile::fullName(const bool includePath) const
{
    return name(includePath) + "."  + p_extension;
}

const std::string BaseFile::name(const bool includePath) const
{
    if(includePath)
        return p_path + std::filesystem::path::preferred_separator +  name(false);
    return p_name;
}

bool BaseFile::create(const bool override)
{
    try {
        const auto option = (override)? FileRegistar::Mode::Write : FileRegistar::Mode::Append;
        return FileRegistar::apply(fullName(true), option, [](std::FILE*){ return true; }, true);
    }  catch (...) {
        return false;
    }
    return true;
}

bool BaseFile::remove()
{
    return !static_cast<bool>(std::remove(fullName(true).c_str()));
}

} // namespace rbs::exporting::file_system

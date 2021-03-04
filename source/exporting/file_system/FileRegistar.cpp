//
//  FileRegistar.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 07.11.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "FileRegistar.h"

namespace rbs::exporting::file_system {

size_t FileRegistar::append(const Path &sourcePath, const Path &destinationPath) {
    const auto open = [](const Path& path, const Mode mode) {
        const auto file = current().open(path, mode);
        if (!file)
            throw std::runtime_error("Cannot open the file at: \"" + path + "\".");
        current().p_locked.insert(file);
        return file;
    };
    const auto sourceFile = open(sourcePath, Mode::Read);
    const auto destinationFile = open(destinationPath, Mode::Append);

    size_t addedChars = 0;
    char buffer[1024];
    while (std::fgets(buffer, sizeof(buffer), sourceFile))
        addedChars += std::fputs(buffer, destinationFile);

    current().p_locked.erase(sourceFile);
    current().p_locked.erase(destinationFile);
    std::fflush(destinationFile);
    return addedChars;
}

size_t FileRegistar::apply(const Path &path, Mode mode, const Action &action, const bool flush) {
    std::FILE * file = current().open(path, mode);
    if (!file)
        throw std::runtime_error("Cannot open the file at: \"" + path + "\".");

    current().p_locked.insert(file);
    const auto result = action(file);
    current().p_locked.erase(file);

    if (flush)
        std::fflush(file);
    return result;
}

FileRegistar &FileRegistar::current() {
    static FileRegistar singleRegistar;
    return singleRegistar;
}

bool FileRegistar::canOpen() const {
    return freeSpace() && p_locked.size() < maxAllowed();
}

char const * FileRegistar::convert(const Mode mode)
{
    switch (mode) {
    case Mode::Read:        return "r";
    case Mode::Write:       return "w";
    case Mode::Append:      return "a";
    case Mode::ReadWrite:   return "w+";
    case Mode::ReadAppend:  return "a+";
    }
    throw std::runtime_error("Unknown mode.");
}

FileRegistar::Size FileRegistar::freeSpace() const {
    return maxAllowed() - p_files.size();
}

std::FILE *FileRegistar::open(const Path &path, const Mode& mode) {
    if (path.empty())
        throw std::runtime_error("Cannot open file on empty path.");

    const auto item = std::find_if(p_files.begin(), p_files.end(), [&path](const FileInfo& info) {
        return path.compare(std::get<0>(info)) == 0;
    });

    if(item != p_files.end()) {
        const auto& lastFile = std::get<1>(*item);
        const auto& lastMode = std::get<2>(*item);
        if (lastMode == mode) {
            return lastFile;
        } else {
            if (lastMode != Mode::Read) { std::fflush(lastFile); }
            std::fclose(lastFile);
            const auto file = std::fopen(path.c_str(), convert(mode));
            if (!file)
                throw std::runtime_error("Cannot open the file at: \"" + path + "\".");

            *item = std::make_tuple(path, file, mode);
            return file;
        }
    }

    openFreeSpace();
    std::FILE* file = std::fopen(path.c_str(), convert(mode));
    if (!file)
        throw std::runtime_error("Cannot open the file at: \"" + path + "\".");

    p_files.push_back(std::make_tuple(path, file, mode));
    return file;
}

bool FileRegistar::openFreeSpace() {
    if (freeSpace())
        return true;

    while(p_locked.size() >= maxAllowed()) { /* Wait untill there is a place available. */  }

    const auto available = std::find_if(p_files.begin(), p_files.end(), [this](const FileInfo& info) {
        return std::find(p_locked.begin(), p_locked.end(), std::get<1>(info)) == p_locked.end();
    });

    if (available == p_files.end())
        return openFreeSpace();

    const auto file = std::get<1>(*available);
    std::fflush(file);
    std::fclose(file);
    p_files.remove(*available);
    return true;
}

FileRegistar::FileRegistar()
    : p_files{}
{}

FileRegistar::FileRegistar(const FileRegistar &other) = default;
FileRegistar& FileRegistar::operator =(FileRegistar&) = default;

constexpr FileRegistar::Size FileRegistar::maxAllowed() const {
    return FOPEN_MAX;
}

} // namespace rbs::exporting::file_system

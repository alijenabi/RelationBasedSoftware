//
//  FileSeriesImp.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 28.06.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef FILESERIESIMP_H
#define FILESERIESIMP_H

#ifndef FILESERIES_H
#include "FileSeries.h"
#endif

#include <iostream>
#include <string>

namespace rbs::exporting::file_system {

template<typename FileType>
FileSeries<FileType>::FileSeries(const std::string &basename,
                                 const std::string &extension,
                                 const Index allowedSize,
                                 const bool override,
                                 const bool fillVocations)
    : p_basename{std::filesystem::path(basename).filename()}
    , p_dummy{nullptr}
    , p_extension{extension}
    , p_exsistedIndices{}
    , p_fillVocations{fillVocations}
    , p_headIndex{0}
    , p_overriding{override}
    , p_path{"."}
    , p_maximumSize{allowedSize}
    , p_warningSent{false}
{
    // removing unexpected signs from extension!
    p_extension.erase(std::remove_if(p_extension.begin(), p_extension.end(), [](char c){
        return c == '.' || c == '!' || c == '@'|| c == '#'|| c == '$'
                || c == '%' || c == '^' || c == '&' || c == '*' || c == '('
                || c == ')' || c == '`' || c == '~' || c == '+' || c == '='
                || c == '/' || c == '>' || c == '<' || c == ',';}));

    // If the file name is set to empty string change it to "un-named".
    if (p_basename.compare("") == 0)
        p_basename = "un-named-file-series";

    const auto path = std::filesystem::path(basename);
    if (path.has_parent_path() && path.parent_path().compare("") != 0)
        p_path = path.parent_path();

    // Removing any leading white space from the basename and path.
    const std::string whitespaces = " \n\r\t\f\v";
    auto nonSpaceIndex = p_path.find_first_not_of(whitespaces);
    p_path = p_path.substr(nonSpaceIndex);
    nonSpaceIndex = p_basename.find_first_not_of(whitespaces);
    p_basename = p_basename.substr(nonSpaceIndex);

    if (!p_overriding) {
        recoverExistedFiles();
        if (isFull())
            reportSizeReachedWarning();
    }
}

template<typename FileType>
FileSeries<FileType>::FileSeries(const std::string &path,
                                 const std::string &base_name,
                                 const std::string &extension,
                                 const FileSeries::Index allowedSize,
                                 const bool override,
                                 const bool fillVocations)
    : FileSeries(path + base_name, extension, allowedSize, override, fillVocations)
{
}

template<typename FileType>
FileSeries<FileType>::~FileSeries()
{
    if(p_dummy)
        p_dummy->remove();
}

template<typename FileType>
const std::string & FileSeries<FileType>::basename() const
{
    return p_basename;
}

template<typename FileType>
typename FileSeries<FileType>::Size FileSeries<FileType>::count() const
{
    return readExistedFiles().size();
}

template<typename FileType>
const std::string FileSeries<FileType>::directory() const
{
    return p_path + seperator;
}

template<typename FileType>
const std::string & FileSeries<FileType>::extension() const
{
    return p_extension;
}

template<typename FileType>
const std::string FileSeries<FileType>::fullName(const FileSeries<FileType>::Index index,
                                           const bool includePath) const
{
    return name(index, includePath) + "." + p_extension;
}

template<typename FileType>
bool FileSeries<FileType>::isFull() const
{
    return length() >= p_maximumSize;
}

template<typename FileType>
bool FileSeries<FileType>::isCreated(const FileSeries::Index index) const
{
    return p_files.find(index) != p_files.end();
}

template<typename FileType>
typename FileSeries<FileType>::Size FileSeries<FileType>::maximumSize() const
{
    return p_maximumSize;
}

template<typename FileType>
typename FileSeries<FileType>::Size FileSeries<FileType>::length() const
{
    return p_files.size() + p_exsistedIndices.size();
}

template<typename FileType>
typename FileSeries<FileType>::Index FileSeries<FileType>::highestIndex() const
{
    const auto createdIndex = (p_files.empty()) ? 0 : p_files.rbegin()->first;
    const auto existedIndex = (p_exsistedIndices.empty()) ? 0 : *p_exsistedIndices.rbegin();
    return std::max(createdIndex, existedIndex );
}

template<typename FileType>
const std::string FileSeries<FileType>::name(const FileSeries<FileType>::Index index, const bool includePath) const
{
    if(includePath)
        return directory() + name(index, false);
    return p_basename + "_" + std::to_string(index);
}

template<typename FileType>
bool FileSeries<FileType>::wasExisted(const FileSeries<FileType>::Index index) const
{
    return p_exsistedIndices.find(index) != p_exsistedIndices.end();
}

template<typename FileType>
FileType & FileSeries<FileType>::at(const FileSeries<FileType>::Index index)
{
    if (isCreated(index)) {
        const auto content_ptr = p_files[index];
        if (content_ptr)
            return *content_ptr;
        throw std::runtime_error("The file in index " + std::to_string(index) + " is set to null.");
    }

    if(isFull()) {
        this->reportSizeReachedWarning();
        if(!p_dummy)
            p_dummy = std::make_shared<FileType>(dummyFilePath(), p_extension);
        return *p_dummy;
    }

    if(p_overriding) {
        // Get the content and clear it.
        p_files[index] = std::make_shared<FileType>(name(index, true), p_extension);
        p_exsistedIndices.erase(index);
        p_files[index]->cleanContent();
    } else if(wasExisted(index)) {
        // Get the content of the file to append to it.
        p_files[index] = std::make_shared<FileType>(name(index, true), p_extension);
    } else {
        // Create the file...
        p_files[index] = std::make_shared<FileType>(name(index, true), p_extension);
        p_exsistedIndices.erase(index);
    }
    return at(index);
}

template<typename FileType>
FileType &FileSeries<FileType>::last() {
    at(p_headIndex);
}

template<typename FileType>
FileType &FileSeries<FileType>::next() {
    if(p_overriding || (!p_overriding && !p_fillVocations)) {
        p_headIndex = p_files.empty() ? 0 : highestIndex() + 1;
        return at(p_headIndex);
    } else if (!wasExisted(p_headIndex)) {
        return at(p_headIndex);
    } else {
        p_headIndex += 1;
        return next();
    }
}

template<typename FileType>
bool FileSeries<FileType>::remove(const FileSeries<FileType>::Index index)
{
    const auto isRemoved = !static_cast<bool>(std::remove(fullName(index, true).c_str()));
    if (isRemoved)
        p_exsistedIndices.erase(index);
    if (length() < p_maximumSize)
        p_warningSent = false;
    return isRemoved;
}

template<typename FileType>
unsigned long FileSeries<FileType>::removeAll(const bool ownedOnly)
{
    auto ans = 0;
    for(auto itr = p_files.begin(); itr != p_files.end(); itr++) {
        if (itr->second->remove())
            ans++;
    }
    p_files.clear();
    if(!ownedOnly) {
        recoverExistedFiles();
        for(auto itr = p_exsistedIndices.begin(); itr != p_exsistedIndices.end(); itr++, ans++ ) {
            std::remove(fullName(*itr, true).c_str());
        }
        p_exsistedIndices.clear();
    }
    p_warningSent = false;
    return ans;
}

template<typename FileType>
const std::string FileSeries<FileType>::dummyFilePath() const
{
    return directory() + ".dummy";
}

template<typename FileType>
typename FileSeries<FileType>::IndexSet FileSeries<FileType>::readExistedFiles() const
{
    IndexSet ans;
    for (const auto & directory_entry : std::filesystem::directory_iterator(p_path))
    {
        const std::string extention = directory_entry.path().extension();
        if (extention.compare("." + p_extension) == 0)
        {
            std::string filename = directory_entry.path().filename();
            const auto extentionStart= filename.find(extention);
            if (extentionStart != std::string::npos)
                filename.erase(extentionStart, extention.length());

            const auto filenameStart = filename.find(p_basename);
            if (filenameStart == 0) {
                filename.erase(0, p_basename.length() + 1); // +1 stands for the "_" seperator and "." before extention.

                const auto isInteger = !filename.empty() && std::find_if(filename.begin(), filename.end(), [](unsigned char c) { return !std::isdigit(c); }) == filename.end();
                const auto index = std::stoi(filename);
                if (isInteger) {
                    ans.insert(index);
                }
            }
        }
    }
    return ans;
}

template<typename FileType>
typename FileSeries<FileType>::Size FileSeries<FileType>::recoverExistedFiles()
{
    p_exsistedIndices = readExistedFiles();
    return p_exsistedIndices.size();
}

template<typename FileType>
void FileSeries<FileType>::reportSizeReachedWarning()
{
    if (!p_warningSent) {
        std::cout << "The maximum indexing of "<< std::to_string(p_maximumSize) << " for file series \""
                  << p_basename << "\" is reached. Further extension will be ignored. " << std::endl;
        p_warningSent = true;
    }
}

} // namespace rbs::exporting::file_system

#endif // FILESERIESIMP_H

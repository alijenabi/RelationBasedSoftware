//
//  SingleFile.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 28.06.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef FILESERIES_H
#define FILESERIES_H

#include "SingleFile.h"
#include <map>
#include <set>

/**
 * The file_system namespace provides the means to handle files on the operating system file system.
 */
namespace rbs::exporting::file_system {

/**
 * @brief The FileSeries class provides the means to handle a series of "SingleFile"s on the operating system file system.
 * @note The class is not default constructable.
 * @note The class is not copiable or moveable.
 * @tparam FileType The expected type of content file.
 */
template<typename FileType>
class FileSeries
{
public:

    using Index = unsigned long;
    using Size = unsigned long;

    using file_ptr = std::shared_ptr<FileType>;

    static constexpr auto seperator = std::filesystem::path::preferred_separator;

    /**
     * @brief The constructor.
     * @param basename The basename of the series (the part of the files name that repeats).
     * @param extension The extension of the file without the dot(.)
     * @param allowedSize The maximum number of files that are allowed to be created by this file series.
     * @param override If true, the series will override the content of all the files that are existed befor constructing this object.
     * @param fillVocations If true, the series will fill the vocations in the existed indices befor createing a higher index.
     */
    explicit FileSeries(const std::string &basename,
                        const std::string &extension,
                        const Index allowedSize,
                        const bool override = true,
                        const bool fillVocations = false);

    /**
     * @brief The constructor.
     * @param path The directory path of the series.
     * @param basename The basename of the series (the part of the files name that repeats).
     * @param extension The extension of the file without the dot(.)
     * @param allowedSize The maximum number of files that are allowed to be created by this file series.
     * @param override If true, the series will override the content of all the files that are existed befor constructing this object.
     * @param fillVocations If true, the series will fill the vocations in the existed indices befor createing a higher index.
     */
    explicit FileSeries(const std::string &path,
                        const std::string &base_name,
                        const std::string &extension,
                        const FileSeries::Index allowedSize,
                        const bool override = true,
                        const bool fillVocations = false);

    /**
     * @brief The deconstructor.
     */
    ~FileSeries();

    /**
     * @brief Gives the basename of the series (the part of the files name that repeats).
     */
    const std::string & basename() const;

    /**
     * @brief Searchs the number of files belonging to this series on the directory path, regardless of their creator.
     * @note Expensive operation.
     */
    Size count() const;

    /**
     * @brief Gives the path of the directory containing the series.
     */
    const std::string directory() const;

    /**
     * @brief Gives the extension of the file without the dot(.)
     */
    const std::string & extension() const;

    /**
     * @brief Gives the full name of the file at the index, with and without its complete path.
     * @param index The index of the file.
     * @param includePath If True, the complete path of the file will be included.
     */
    const std::string fullName(const Index index, const bool includePath) const;

    /**
     * @brief Computes whether the series is full or not.
     * @return Ture if the sereis include equal or more files that the maximumSize(), false otherwise.
     */
    bool isFull() const;

    /**
     * @brief Computes whether the file with a give index is created by this object or not.
     * @param index The index of the file.
     * @return True if the file is created by this object, false otherwise.
     */
    bool isCreated(const FileSeries::Index index) const;

    /**
     * @brief Gives the maximum number of files that are allowed to be created by this file series.
     */
    Size maximumSize() const;

    /**
     * @brief Computes the lenght of the series.
     * @note The list of existed file will only be updated at the contstruction time.
     */
    Size length() const;

    /**
     * @brief Computes the highest index of the files in the series.
     * @note The list of existed file will only be updated at the contstruction time.
     */
    Index highestIndex() const;

    /**
     * @brief Gives the name of the file (without extention) at an index, with or without its complete path.
     * @param index The index of the file.
     * @param includePath If True, the complete path of the file will be included.
     */
    const std::string name(const FileSeries::Index index, const bool includePath) const;

    /**
     * @brief computes whether the file related to the given index existed before construction of this object or not.
     * @note The list of existed file will only be updated at the contstruction time.
     * @param index The index of the file.
     */
    bool wasExisted(const FileSeries::Index index) const;

    /**
     * @brief Gives the content of the file at a given index.
     * @note Creates the file if not exists.
     * @note If the override is on, the content of the existed files before creation of this object will be removed.
     * @note If the override is off, this will append to the existed files before creation of this object.
     * @param index The index of the file.
     * @return The content of the file at a given index.
     */
    FileType & at(const FileSeries::Index index);

    /**
     * @brief Gives the last created file in the series.
     */
    FileType & last();

    /**
     * @brief Gives the content of the next file.
     */
    FileType & next();

    /**
     * @brief Removes the file form the series regardless of its creator.
     * @param index The index of the file.
     * @return Ture if successful, false otherwise.
     */
    bool remove(const FileSeries::Index index);

    /**
     * @brief Removes all the files of the series.
     * @param ownedOnly If true, it will only remove all the files created by this object.
     * @return The number of removed files.
     */
    unsigned long removeAll(const bool ownedOnly);

protected:

    using IndexSet = std::set<Index>;
    using FileSet = std::map<Index, file_ptr>;

    /**
     * @brief Computes the dummy file path.
     */
    const std::string dummyFilePath() const;

    /**
     * @brief Searches for the file's indices on the directory.
     * @return The indeces of the files existing on the directory.
     */
    IndexSet readExistedFiles() const;

    /**
     * @brief Updates the p_exsistedIndices by serching for the existing file's indices on the path directory.
     * @return The number of found files.
     */
    FileSeries::Size recoverExistedFiles();

    /**
     * @brief Reports the size reached to the ostream.
     * @note This will send a message only once.
     * @note This would send a message if the number of files reduced bellow the maximum allowed and now raising again above the limit.
     */
    void reportSizeReachedWarning();

private:

    std::string p_basename;         // The basename of the series (the part of the files name that repeats).
    file_ptr p_dummy;               // This is the content file that will be hand over if the index is out of range. (it will be removed at the deconstruction of the object).
    std::string p_extension;        // The extension of the file without the dot(.)
    IndexSet p_exsistedIndices;     // The indices that their relative files were existed on the directory path before constructing this object.
    const bool p_fillVocations;     // If true, the series will fill the vocations in the existed indices befor createing a higher index.
    FileSet p_files;                // The files that are currently open (Their content is ready to use!).
    Index p_headIndex;              // The minimum creatable index. Head + 1 can be a posible new file index.
    const bool p_overriding;        // If true, the series will override the content of all the files that are existed befor constructing this object.
    std::string p_path;             // The directory path of the series.
    Size p_maximumSize;             // The maximum number of files that are allowed to be created by this file series.
    bool p_warningSent;             // If true, the user is already noted that the maximum allowed index is reached, and the series will ignore any input.

};

} // namespace rbs::exporting::file_system

#include "FileSeriesImp.h"

#endif // FILESERIES_H

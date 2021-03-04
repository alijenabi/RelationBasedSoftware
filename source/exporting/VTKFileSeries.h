//
//  VTKFileSeries.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VTKFILESERIES_H
#define VTKFILESERIES_H

#include "VTKFile.h"
#include "file_system/FileSeries.h"

/**
 * The file_system namespace provides the means to handle files on the operating system file system.
 */
namespace rbs::exporting {

/**
 * @brief The VTKFileSeries class provides the means to handle exporting data to series of VTK files.
 */
class VTKFileSeries : public file_system::FileSeries<VTKFile>
{
public:

    /**
     * @brief The constructor.
     * @param basename The basename of the series (the part of the files name that repeats).
     * @param extension The extension of the file without the dot(.)
     * @param allowedSize The maximum number of files that are allowed to be created by this file series.
     * @param override If true, the series will override the content of all the files that are existed befor constructing this object.
     * @param fillVocations If true, the series will fill the vocations in the existed indices befor createing a higher index.
     */
    explicit VTKFileSeries(const std::string &basename,
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
    explicit VTKFileSeries(const std::string &path,
                           const std::string &base_name,
                           const std::string &extension,
                           const FileSeries::Index allowedSize,
                           const bool override = true,
                           const bool fillVocations = false);

    /**
     * @brief The deconstructor.
     */
    ~VTKFileSeries();

    /**
     * @brief Whether the file series are exporting data on a sheared configuration or not.
     * @note If true, then adding a cell to one file will be added to all future files.
     */
    bool isShareingConfiguration() const;

    /**
     * @brief Locks the shared configuration from appending new ponts/cells to it.
     */
    void lockSharedConfiguration() const;

    /**
     * @brief Gives the content of the file at a given index.
     * @note Creates the file if not exists.
     * @note If the override is on, the content of the existed files before creation of this object will be removed.
     * @note If the override is off, this will append to the existed files before creation of this object.
     * @param index The index of the file.
     * @return The content of the file at a given index.
     */
    VTKFile & at(const Index index);

    /**
     * @brief Gives the content of the next file.
     */
    VTKFile & next();

    /**
     * @brief Shares the future files configurations.
     */
    void shareConfiguration();

    /**
     * @brief Sets all the future files configuration with an external VTKFile configuration.
     * @note By adding a cell to one file of the series, it will be added to all future files.
     * @param file The file containg the configuration.
     */
    void shareConfiguration(const std::shared_ptr<VTKFile> file);

private:

    bool p_isSharedConfigIsInteranl;
    std::shared_ptr<VTKFile> p_sharedConfiguration;

};

} // namespace rbs::exporting

#endif // VTKFILESERIES_H

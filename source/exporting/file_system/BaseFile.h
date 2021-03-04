//
//  BaseFile.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 10.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef BASEFILE_H
#define BASEFILE_H

#include <filesystem>
#include <string>

/**
 * The file_system namespace provides the means to handle files on the operating system file system.
 */
namespace rbs::exporting::file_system {

/**
 * @brief The BaseFile class provides a wrapper around the FILE to limits access and only appending and writing privileges.
 * @note The class is not default constructable.
 * @note The class is not copiable or moveable.
 */
class BaseFile
{
public:

    static constexpr auto seperator = std::filesystem::path::preferred_separator;

    /**
     * @brief The constructor.
     * @param name The name of the file (the path to the file included).
     * @param extension The extention of the file without the dot(.).
     */
    explicit BaseFile(const std::string &name,
                      const std::string &extension);

    /**
     * @brief The constructor.
     * @param path The directory path of the file.
     * @param name The name of the file.
     * @param extension The extention of the file without the dot(.).
     */
    explicit BaseFile(const std::string &path,
                      const std::string &name,
                      const std::string &extension);

    /**
     * @brief Gives the path of the directory containing the file.
     */
    const std::string directory() const;

    /**
     * @brief The extention of the file without the dot(.).
     */
    const std::string & extension() const;

    /**
     * @brief The full name of the file with the extension.
     * @param includePath If True, the complete path of the file will be included.
     */
    const std::string fullName(const bool includePath) const;

    /**
     * @brief The name of the file without the extension.
     * @param includePath If True, the complete path of the file will be included.
     */
    const std::string name(const bool includePath) const;

    /**
     * @brief Creates the file.
     * @param override If true, it will override the existing file (if any), and appends if false.
     * @return True if successful, false otherwise.
     */
    bool create(const bool override);

    /**
     * @brief Removes the file without move it to bin/trash directory.
     * @return True if successful, false otherwise.
     */
    bool remove();

private:

   std::string p_extension;     // The extention of the file with out the dot(.).
   std::string p_name;          // The name of the file with the path.
   std::string p_path;          // The path of the directory that the file located in it.

};

} // namespace rbs::exporting::file_system

#endif // BASEFILE_H

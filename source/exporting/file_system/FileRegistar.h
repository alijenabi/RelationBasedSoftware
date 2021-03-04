//
//  FileRegistar.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 07.11.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef FILEREGISTAR_H
#define FILEREGISTAR_H

#include "BaseFile.h"
#include <cstdio>
#include <list>
#include <set>
#include <string>

/**
 * The file_system namespace provides the means to handle files on the operating system file system.
 */
namespace rbs::exporting::file_system {

/**
 * @brief The FileRegistar class.
 * @details The file system, due to security reasons, does not allow the software to open files indefinitely.
 *          This class is a singleton that counts the number of opened files and opens a new one if allowed by the operating system.
 */
class FileRegistar
{
public:

    using Ptr = std::FILE*;
    using Path = std::string;

    /**
     * @brief The Mode enum
     */
    enum class Mode {
        Read,           // Opens the file only to read from the start.
        Write,          // Opens the file to rewrite it from the start.
        Append,         // Opens the file to append to it.
        ReadWrite,      // Opens the file to both read and rewrtie it.
        ReadAppend,     // Opens the file to both read and append to it.
    };

    /**
     * @brief The action to be done on the file.
     * @param [in, out] The file handler pointer.
     * @return The size of character added/removed/modified on the file.
     */
    using Action = std::function<size_t(Ptr)>;

    /**
     * @brief Appends one file content to the other one.
     * @param targetFile The file that its content will be read.
     * @param destinationFile The file that its content will be appended.
     * @return The number of characters appended to the destination file.
     */
    static size_t append(const Path& sourcePath, const Path& destinationPath);

    /**
     * @brief Applys an action on a file.
     * @param filename The name of the file
     * @param mode The access mode same as std::fopen mode.
     * @param action The action to be taken on the file.
     * @param flush Whether to flush to file or not.
     * @return The action return value.
     */
    static size_t apply(const Path& path, Mode mode, const Action& action, bool flush);

protected:

    using Size = size_t;
    using FileInfo = std::tuple<Path, Ptr, Mode>;
    using LockedFiles = std::set<Ptr>;
    using FilePtrs = std::list<FileInfo>;

    /**
     * @brief Provides the current FileRegister.
     */
    static FileRegistar& current();

    /**
     * @brief Whether there is a place left for opening a new file.
     */
    bool canOpen() const;

    /**
     * @brief Converts the mode to its relative std::fopen mode.
     */
    char const * convert(Mode mode);

    /**
     * @brief Gives free spaces that can be opened. (i.e., either empty file space or unlocked file hander).
     */
    Size freeSpace() const;

    /**
     * @brief Opens a file and returns the pointer to it.
     * @param path The file path.
     * @param mode The access mode same as std::fopen mode.
     * @throws If the file path is empty.
     */
    std::FILE* open(const Path& path, const Mode& mode);

    /**
     * @brief Opens free space.
     * @note This will wait until a free space becomes available.
     */
    bool openFreeSpace();

private:

    /**
     * @brief Constroctor.
     * @note This is private since the file registar is a Singleton.
     */
    FileRegistar();

    /**
     * @brief Copy Constroctor.
     * @note This is private since copying file registar should only be handled here;
     *        otherwise, the file registar will not be a Singleton.
     */
    FileRegistar(const FileRegistar& other);

    /**
     * @brief Assignment Operation.
     * @note This is private since assiging file registar systems should only be handled here;
     *        otherwise, the file registar will not be a Singleton.
     */
    FileRegistar& operator =(FileRegistar&);

    /**
     * @brief Gives the maximum possible open files allowed by the operating system.\
     */
    constexpr Size maxAllowed() const;

    FilePtrs p_files;       // The files that are opens with this registar.
    LockedFiles p_locked;   // The files that are currently locked.

};

} // namespace rbs::exporting::file_system

#endif // FILEREGISTAR_H

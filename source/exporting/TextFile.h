//
//  TextFile.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef TEXTFILES_H
#define TEXTFILES_H

#include "file_system/FileRegistar.h"
#include "file_system/SingleFile.h"
#include <iomanip>
#include <sstream>

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

/**
 * @brief The TextFile class provides the means to handle exporting data to a text file.
 * @note The class is not default constructable.
 * @note The class is not copiable or moveable.
 */
class TextFile:public file_system::SingleFile
{
public:

    using CharCount = int;

    /**
     * @brief The constructor.
     * @param name The name of the file (the path to the file included).
     * @param extension The extention of the file without the dot(.).
     */
    explicit TextFile(const std::string &name, const std::string &extension = "txt");

    /**
     * @brief The constructor.
     * @param path The directory path of the file.
     * @param name The name of the file (the path to the file included).
     * @param extension The extention of the file without the dot(.).
     */
    explicit TextFile(const std::string &path,
                       const std::string &name,
                       const std::string &extension);

    /**
     * @brief Appends a c_string to the file.
     * @param c_str The c string.
     * @param flush WHether to flush content to the file or not.
     * @return The number of characters successfully added to the file.
     */
    CharCount append(const char * c_str, bool flush);

    /**
    * @brief Append a string to the text file.
    * @param text The string to append.
     * @param flush WHether to flush content to the file or not.
    * @return The number of characters successfully added to the file.
    */
    CharCount append(const std::string & text, bool flush);

    /**
     * @brief Appends a number to the text file.
     * @tparam N The number type.
     * @param number The number.
     * @param flush WHether to flush content to the file or not.
     * @param scientific Whether print the number with schientific notation or not.
     * @param precision The number of char after dot(.) to print.
     * @return The number of characters successfully added to the file.
     */
    template<typename N>
    CharCount appendNumber(const N number, const bool flush, const bool scientific = false, const int precision = -1){
        const auto type = (scientific) ? "e" : "f";
        const unsigned int defaultPrecision = (typeid(N) == typeid(double)) ? 12 : ((typeid (N) == typeid(float)) ? 6: 0);
        const unsigned int finalPercision = (precision < 0) ? defaultPrecision : precision;
        const auto str = "%." + std::to_string(finalPercision) + type;

        using file_system::FileRegistar;
        return FileRegistar::apply(fullName(true), FileRegistar::Mode::Append, [this, number, str](std::FILE* file){
            if (isZero(number))
                return std::fprintf(file, str.c_str(), static_cast<double>(0));
            return std::fprintf(file, str.c_str(), static_cast<double>(number));
        }, flush);
    }

    /**
     * @brief Appends a custom object to the text file.
     * @tparam DATA The custom type.
     * @param object The custom object.
     * @param flush WHether to flush content to the file or not.
     * @return The number of characters successfully added to the file.
     */
    template<typename DATA>
    CharCount appendCustom(const DATA object, const bool flush){
        std::ostringstream osContent{std::ostringstream::ate};
        osContent << object;
        return append(osContent.str().c_str(), flush);
    }

    /**
     * @brief Appends the content of the given file to this file.
     * @param filePath The complete path to the target file.
     * @return True, if successful, false otherwise.
     */
    bool appendFileContent(const TextFile& filePath);

    /**
     * @brief Gos to a new line.
     * @param flush WHether to flush content to the file or not.
     * @return True, if successful, false otherwise.
     */
    bool nextLine(bool flush);

private:

    /**
     * @brief Whether the value is close enoght to zero or not.
     * @param value The value to check.
     * @return True, if the value is close to zero false otherwise.
     */
    template<typename T>
    constexpr bool isZero(T value) {
        constexpr auto abs = [](const T & p) { return (p >= 0)? p : -p;};
        if (value == 0)
            return true;
        if (typeid(T) == typeid(double))
            return abs(value) <= 0.00000000001;
        if (typeid(T) == typeid(float))
            return abs(value) <= 0.00001f;
        return value == 0;
    }

};

} // namespace rbs::exporting

#endif // TEXTFILES_H

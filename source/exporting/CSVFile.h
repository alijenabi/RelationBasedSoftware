//
//  CSVFile.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef CSVFILE_H
#define CSVFILE_H

#include "TextFile.h"
#include <iomanip>
#include <sstream>

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

/**
 * @brief The CSVFile class provides the means to handle exporting data to a Comma-separated values (CSV) file.
 * @note The class is not default constructable.
 * @note The class is not copiable or moveable.
 */
class CSVFile: public TextFile
{
public:

    using Cell = unsigned long;
    using Column = unsigned long;
    using Row = unsigned long;
    using Seperator = std::string;

    /**
     * @brief The constructor.
     * @param name The name of the file (the path to the file included).
     * @param extension The extention of the file without the dot(.).
     */
    explicit CSVFile(const std::string &name,
                     const std::string &extension);

    /**
     * @brief The constructor.
     * @param path The directory path of the file.
     * @param name The name of the file.
     * @param extension The extention of the file without the dot(.).
     * @param seperator The Comma-separated values, seperator string.
     */
    explicit CSVFile(const std::string &path,
                     const std::string &name,
                     const std::string &extension);

    void append(const std::string & text) = delete;
    template<typename DATA>
    void append(const DATA& data) = delete;
    void nextLine() = delete;

    /**
     * @brief Gives the number of addes cells to the file.
     * @return The total number of cells added to the file.
     */
    Cell cellCount() const;

    /**
     * @brief Gives the current column index.
     */
    Column columnCount() const;

    /**
     * @brief Gives the maximum column number posible in the table.
     * @return The maximum column number posible in the table, zero if no limit.
     */
    Column maxColumn() const;

    /**
     * @brief Gives the maximum row number posible in the table.
     * @return The maximum row number posible in the table, zero if no limit.
     */
    Row maxRow() const;

    /**
     * @brief Gives the nubmer of rows inside the file.
     */
    Row rowIndex() const;

    /**
     * @brief Gives the seperator.
     */
    const Seperator& seperator() const;

    /**
     * @brief Appends a cell to the file.
     * @note If maxColumn is set this will call the next row at the end of each row.
     * @note Any white space or seperator will be removed before appending the cell to the file.
     * @param c_str The c string.
     * @param flush Whether to flush to file or not.
     * @return True if it is able to append, false otherwise
     */
    bool appendCell(const char * c_str, const bool flush);

    /**
     * @brief Appends a cell to the file.
     * @note If maxColumn is set this will call the next row at the end of each row.
     * @note Any white space or seperator will be removed before appending the cell to the file.
     * @param cellContent The cell content to append to the file.
     * @param flush Whether to flush to file or not.
     * @return True if it is able to append, false otherwise
     */
    bool appendCell(std::string cellContent, const bool flush);

    /**
     * @brief Appends a cell to the file.
     * @note If maxColumn is set this will call the next row at the end of each row.
     * @tparam N The type of the cell content.
     * @param number The cell content to append to the file.
     * @param flush WHether to flush to the file or not.
     * @param scientific Whether add the content with scientific notion or not.
     * @return True if it is able to append, false otherwise
     */
    template<typename N>
    bool appendCell(const N number, const bool flush, const bool scientific) {
        try {
            if (p_maxRow == 0 || p_rowCount < p_maxRow) {
                TextFile::appendNumber(number, flush, scientific, -1);
                handelRowEnding();
                return true;
            }
        } catch (...) {
        }
        return false;
    }

    /**
     * @brief Appends a row to the file.
     * @note If maxColumn is set this will call the next row at the end of each row.
     * @tparam ITRT The type of the row content which should be itratable.
     * @param rowContents The row to append to the file
     * @param flush Whether to flush the content to file or not..
     * @param scientific Whether add the content with scientific notion or not.
     * @return The number of successfuly added cells.
     */
    template<typename ITRT>
    unsigned long appendRow(const ITRT & rowContents, const bool flush, const bool scientific = true) {
        unsigned long successfull = 0;
        for (const auto content : rowContents)
            successfull += appendCell(content, flush, scientific);
        return successfull;
    }

    /**
     * @brief Gives direct accessthe to the maximum column number posible in the table.
     * @return The maximum column number posible in the table, zero if no limit.
     */
    Column & maxColumn();

    /**
     * @brief Gives direct accessthe to maximum row number posible in the table.
     * @return The maximum row number posible in the table, zero if no limit.
     */
    Row & maxRow();

    /**
     * @brief Starts a new line (new row).
     * @note If maxColumn is set the append member will call this!
     * @param flush Whether to flush the content to file or not.
     * @return Ture if successful, false otherwise.
     */
    bool nextRow(bool flush);

    /**
     * @brief Removes the content of the file.
     * @note The fill will be closed afterward.
     * @return True if successful, false otherwise.
     */
    bool cleanContent();

    /**
     * @brief Removes the file without move it to bin/trash directory.
     * @return True if successful, false otherwise.
     */
    bool remove();

    /**
     * @brief Sets the seperator of the csv file.
     * @note Use this immediately after the constructor.
     * @param seperator The new seperator.
     * @return The csv file with new seperator.
     * @throws If the file is not empty.
     */
    void setSeperator(const Seperator seperator);

protected:

    /**
     * @brief Erases all Occurrences of given substring from main string.
     */
    static void eraseAllSubStr(std::string & mainStr, const std::string & toErase);

    /**
     * @brief Removes all unacceptable characters from a string make it ready to append to the file.
     * @param string The raw string for removeing unacceptable characters.
     * @return Ready to use cell content.
     */
    void filterCellContnet(std::string & string) const;

    /**
     * @brief Appends a cell to the file directly (without checking the cell contnet).
     * @note If maxColumn is set this will call the next row at the end of each row.
     * @note Any white space or seperator will be removed before appending the cell to the file.
     * @param cellContent The cell content to append to the file.
     * @param flush Whether to flush the content to file or not.
     * @return True if it is able to append, false otherwise
     */
    bool directAppendToCell(const char * cellContent, bool flush) noexcept;

    /**
     * @brief Handel row ending
     * @details Decides whether to go to the next line (next row) close the row or add the separator.
     * @note this does not flush the content to file since the existence of the line end or cell separator without a new cell uses less
     */
    void handelRowEnding();

private:

    Cell p_cellCount;
    Column p_columnCount;
    Column p_maxColumns;
    Row p_maxRow;
    Row p_rowCount;
    Seperator p_seperator;

};

} // namespace rbs::exporting

#endif // CSVFILE_H

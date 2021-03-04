//
//  CSVFile.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "CSVFile.h"
#include <vector>

namespace rbs::exporting {

CSVFile::CSVFile(const std::string &name,
                 const std::string &extension)
    : TextFile(name, extension)
    , p_cellCount{0}
    , p_columnCount{0}
    , p_maxColumns{0}
    , p_maxRow{0}
    , p_rowCount{0}
    , p_seperator{","}
{
}

CSVFile::CSVFile(const std::string &path,
                 const std::string &name,
                 const std::string &extension)
    : TextFile(path, name, extension)
    , p_cellCount{0}
    , p_columnCount{0}
    , p_maxColumns{0}
    , p_maxRow{0}
    , p_rowCount{0}
    , p_seperator{","}
{
}

CSVFile::Cell CSVFile::cellCount() const
{
    return p_cellCount;
}

CSVFile::Column CSVFile::columnCount() const
{
    return p_columnCount;
}

CSVFile::Column CSVFile::maxColumn() const
{
    return p_maxColumns;
}

CSVFile::Row CSVFile::maxRow() const
{
    return p_maxRow;
}

CSVFile::Row CSVFile::rowIndex() const
{
    return p_rowCount;
}

const CSVFile::Seperator & CSVFile::seperator() const
{
    return p_seperator;
}

bool CSVFile::appendCell(const char *c_str, const bool flush)
{
    return appendCell(std::string(c_str), flush);
}

bool CSVFile::appendCell(std::string cellContent, const bool flush)
{
    filterCellContnet(cellContent);
    return directAppendToCell(cellContent.c_str(), flush);
}

CSVFile::Column &CSVFile::maxColumn()
{
    return p_maxColumns;
}

CSVFile::Row &CSVFile::maxRow()
{
    return p_maxRow;
}

bool CSVFile::nextRow(const bool flush)
{
    try {
        if (p_maxRow == 0 || p_rowCount < p_maxRow) {
            TextFile::nextLine(flush);
            p_rowCount++;
            return true;
        }
    } catch (...) {
    }
    return false;
}

bool CSVFile::cleanContent(){
    if (TextFile::cleanContent()) {
        p_cellCount = 0;
        p_columnCount = 0;
        p_rowCount = 0;
        return true;
    }
    return false;
}

bool CSVFile::remove()
{
    if (TextFile::remove()) {
        p_cellCount = 0;
        p_columnCount = 0;
        p_rowCount = 0;
        return true;
    }
    return false;
}

void CSVFile::setSeperator(const CSVFile::Seperator seperator) {
    if(!p_cellCount)
        p_seperator = seperator;
    else
        throw std::logic_error("The csv file cannot be changed in the middle of the file.");
}

void CSVFile::eraseAllSubStr(std::string &mainStr, const std::string &toErase)
{
    size_t pos = std::string::npos;
    while ((pos  = mainStr.find(toErase) )!= std::string::npos)
        mainStr.erase(pos, toErase.length());
}

void CSVFile::filterCellContnet(std::string &string) const {
    string.erase(std::remove_if(string.begin(), string.end(), [](char c){
        return c == '\n' || c == '\r' || c == '\t'|| c == '\f'|| c == '\v';}));
    eraseAllSubStr(string, p_seperator);
}

bool CSVFile::directAppendToCell(const char * cellContent, const bool flush) noexcept
{
    try {
        if (p_maxRow == 0 || p_rowCount < p_maxRow) {
            TextFile::append(cellContent, flush);
            handelRowEnding();
            return true;
        }
    } catch (...) {
    }
    return false;
}

void CSVFile::handelRowEnding() {
    p_cellCount++;
    p_columnCount++;
    if(p_maxColumns == 0 || p_columnCount < p_maxColumns){
        TextFile::append(p_seperator, false);
    } else if (p_maxColumns != 0 && p_columnCount >= p_maxColumns) {
        TextFile::nextLine(false);
        p_columnCount = 0;
        p_rowCount++;
    }
}

} // namespace exporting


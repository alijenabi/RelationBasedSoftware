//
//  VTKSection.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 11.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VTKSECTION_H
#define VTKSECTION_H

#include "../CSVFile.h"

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

/**
 * @brief The VTKSection class provides the means to handle diferent type of the vtk sections.
 * @details This class provides a header and body subsections, where the body is stored on
 *          a temporary CSV file (with " " separator and "tempvtk" extension) on the
 *          machine filesystem.
 * @note The storage of body data on a file reduces the overall memory cost.
 * @note The class is not default constructable.
 * @note The class is not copiable or moveable.
 */
class VTKSection
{
public:

    using Body = CSVFile;
    using CellNumber = unsigned long;

    /**
     * @brief Each vtk file has several sections.
     * @note Sections have an order, the order is the same as here.
     */
    enum class Type {
        Points,         // Contains the position of the points.
        Cells,          // Maps the cells to their points in the Point section.
        CellTypes,      // Contains the cell index and their types.
        Attribute       // Containt the information of the points or cell attributes (i.e., scalar, vector, tensor).
    };

    /**
     * @brief The constructor.
     * @param name The name of the parent VTK file (the path to the file included).
     * @param type The type of the section.
     */
    explicit VTKSection(const std::string &name, Type type);

    /**
     * @brief The constructor.
     * @param path The directory path of the file.
     * @param name The name of the parent VTK file.
     * @param type The type of the section.
     */
    explicit VTKSection(const std::string &path, const std::string &name, Type type);

    ~VTKSection() {}

    /**
     * @brief Gives the number of cells in each row of the given section required in CSV format.
     * @param type The type of the section.
     * @return The number of cells in each row of the given section.
     */
    static CellNumber tableCellPerRow(const Type type);

    /**
     * @brief Converts the section name to the std::string.
     * @param type The type of section.
     * @return The equivalent std::string of the section name.
     */
    static std::string to_string(const Type type);

    /**
     * @brief Gives the header of the section.
     */
    virtual std::string header() const;

    /**
     * @brief Gives the body of the section.
     */
    const Body & body() const;

    /**
     * @brief Gives the path of the file.
     */
    const std::string path() const;

    /**
     * @brief Gives the type of the section.
     */
    Type type() const;

    /**
     * @brief Removes the body (file) of the section without moving to bin.
     * @return True, if successful, false otherwise.
     */
    bool removeBody();

    friend std::ostream & operator << (std::ostream & out, const Type & type);

protected:

    using Body_ptr = std::shared_ptr<Body>;

    /**
     * @brief Gives direct access to the body of the section.
     */
    const Body_ptr & bodyPtr();

private:

    Body_ptr p_bodyPtr;
    const Type p_type;

};

} // namespace rbs::exporting

#endif // VTKSECTION_H

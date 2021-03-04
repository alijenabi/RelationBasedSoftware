//
//  VTKAttributeSection.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 15.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VTKATTRIBUTESECTION_H
#define VTKATTRIBUTESECTION_H

#include "VTKSection.h"
#include <vector>

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

/**
 * @brief The VTKPointSection class provides the means to handle exporting Points to the tempvtk file.
 */
class VTKAttributeSection : public VTKSection
{
public:

    using Size = unsigned long;

    /**
     * @brief The type of the attribute.
     */
    enum AttributeType {
        Scalar,         // Contains the scalar attributes of the Points or Cells (i.e., DataType)
        Vector,         // Contains the vector attributes of the Points or Cells (i.e., DataType)
        Tensor          // Contains the tensor attributes of the Points or Cells (i.e., DataType)
    };

    /**
     * @brief The source that the attribute belong to it (i.e., scalar, vector, tensor).
     */
    enum class SourceType {
        Point,  // The data presents a point atribute.
        Cell    // The data presents a cell atribute.
    };

    /**
     * @brief The constructor.
     * @note The attribute name will be filterd before use.
     * @note The attribute type can be altered anytime before adding the first attribute to the body.
     * @note The source type can be altered anytime.
     * @param name The name of the parent VTK file (the path to the file included).
     * @param attributeName The name of the attribute to be added at the header of the section.
     * @param attribute The type of the attribute that the body of the section will hold.
     * @param source The type of the attribute source.
     */
    explicit VTKAttributeSection(const std::string &name,
                                 const std::string & attributeName,
                                 const AttributeType attribute,
                                 const SourceType source);

    /**
     * @brief The constructor.
     * @note The attribute name will be filterd before use.
     * @note The attribute type can be altered anytime before adding the first attribute to the body.
     * @note The source type can be altered anytime.
     * @param path The directory path of the file.
     * @param attributeName The name of the attribute to be added at the header of the section.
     * @param attribute The type of the attribute that the body of the section will hold.
     * @param source The type of the attribute source.
     */
    explicit VTKAttributeSection(const std::string &path,
                                 const std::string &name,
                                 const std::string & attributeName,
                                 const AttributeType attribute,
                                 const SourceType source);

    ~VTKAttributeSection(){}

    /**
     * @brief Filter the attribute name to be ready for the header of the section.
     * @note The attribute name cannot contain white spaces and special characters.
     * @param name The suggested name for the attribute.
     * @return The clean and ready to use attribute name.
     */
    static const std::string filterAttributeName(const std::string & name);

    /**
     * @brief Gives the header of the points section.
     */
    virtual std::string header() const override;

    /**
     * @brief Gives the attribute name of the section.
     */
    const std::string & attributeName() const;

    /**
     * @brief Gives the attributes type of the section.
     */
    AttributeType attributeType() const;

    /**
     * @brief Gives the number of information inside the body.
     * @note The return value should be equal to the size of the point/cell section body.
     * @note The return value is not the number of cells added to the body.
     */
    Size size() const;

    /**
     * @brief Gives the source type of this attribute section.
     */
    SourceType source() const;

    /**
     * @brief Appendd values to the attribute section's body.
     * @note The attribute values will be added based on the attribute type.
     * @tparam The type of the information, the first call will set the attribute data type.
     * @param values The values of the attribute.
     * @throws If the size of the values is not sufficient to close (e.g., for vector attribute: size%3 != 0).
     * @throws If the data type is changed form the privious added data type.
     */
    template<typename T>
    void appendAttribute(const std::vector<T> & values) {
        setAttributeDataType<T>();
        if (values.size() % cellPerType(p_type) != 0)
            throw std::logic_error("The imported attribute values are out of size.");
        if (p_type == AttributeType::Tensor) {
            for(unsigned int itr = 0; itr + 8 < values.size(); itr += 9) {
                bodyPtr()->appendRow(values, false, p_dataType != DataType::Int);   // It will be flushed on the assembly time.
                bodyPtr()->nextRow(false);                                          // It will be flushed on the assembly time.
            }
        } else {
            bodyPtr()->appendRow(values, false, p_dataType != DataType::Int);       // It will be flushed on the assembly time.
        }
    }

    /**
     * @brief Sets a new attribute type to the section.
     * @param type The new attribute type.
     * @throws If the section body is not empty.
     */
    void setAttributeType(const AttributeType type);

    /**
     * @brief Sets the name of the attribute.
     * @param name the new name of the attribute.
     */
    void setName(const std::string & name);

    /**
     * @brief Sets the source of the section attribute.
     * @param source The new source.
     */
    void setSource(const SourceType source);

    friend std::ostream & operator << (std::ostream & out, const AttributeType & type);
    friend std::ostream & operator << (std::ostream & out, const SourceType & type);

protected:

    /**
     * @brief The data type of attributes in the body of the section.
     */
    enum class DataType {
        Int,
        Float,
        Double
    };

    /**
     * @brief Gives the number of cells in each row of the given attribute CSV format.
     * @param type The type of the section.
     */
    static CellNumber cellPerRow(const AttributeType type);

    /**
     * @brief Gives the number of cell per each attribute info.
     * @param type The type of the attribute.
     */
    static Size cellPerType(const AttributeType type);

    /**
     * @brief Converts the DataType to string
     * @param type The DataType instance.
     * @return The string representation of the DataType.
     */
    static const std::string to_string(DataType type);

    /**
     * @brief Sets the data type of this section.
     * @throws If the data type is changed form the privious added data type.
     */
    template<typename T>
    void setAttributeDataType() {
        const auto oldType = p_dataType;
        if(typeid(T) == typeid(double)) {
            p_dataType = DataType::Double;
        } else if (typeid(T) == typeid(float)) {
            p_dataType = DataType::Float;
        } else {
            p_dataType = DataType::Int;
        }

        if(static_cast<bool>(bodyPtr()->cellCount()) && oldType != p_dataType) {
            throw std::runtime_error("The body is not empty, the content type cannot be changed.");
        }
    }

private:

    std::string p_name;     // The name of the attribute.
    AttributeType p_type;   // The attribute type of the section.
    SourceType p_source;    // The attribute source's type.
    DataType p_dataType;    // The type of the data.

};

} // namespace rbs::exporting

#endif // VTKATTRIBUTESECTION_H

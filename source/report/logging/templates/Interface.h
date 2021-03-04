//
//  Interface.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 13.12.20.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>

/**
 * The templates interface provides the templates for logging.
 */
namespace rbs::report::logging::templates {

/**
 * @brief The logger demands a template (i.e., format) to define how the logging information should look like.
 * @note This interface provides the functionality that the logging template should provide.
 */
class Interface {
public:

    using Info = std::string;
    using FormatedText = std::string;

    /**
     * @brief The Tags define different style of format.
     */
    enum Tag {
        BlockHeader, BlockFooter,   // The block header and footer.
        InnerBlockHeader,           // The inner block header.
        InnerBlockParagraph,        // The inner block information.
        InnerBlockFooter,           // The inner block footer.
        H1, H2, H3, H4, H5, H6,     // Headers
        P,                          // Paragraph
        Space,                      // Empty Line.
    };

    virtual ~Interface(){}

    /**
     * @brief Clones the opject.
     * @param isCopy Whether copy the existing or create a new object.
     */
    virtual std::shared_ptr<Interface> clone(const bool isCopy) = 0;

    /**
     * @brief Gives the items that do not require information.
     * @param tag The tag.
     */
    virtual FormatedText item(const Tag tag) = 0;

    /**
     * @brief Formats the information based on the given tag.
     * @param tag The tag.
     * @param info The information.
     */
    virtual FormatedText item(const Tag tag, const Info& info) = 0;

    /**
     * @brief Gives the software footer.
     * @note Not defining this as a "const" member will allow more free template implementation downstream.
     * @param name The name of the software.
     * @param version The version of the software.
     */
    virtual FormatedText softwareFooter(const Info& name, const Info& version) = 0;

    /**
     * @brief Gives the software header.
     * @note Not defining this as a "const" member will allow more free template implementation downstream.
     * @param name The name of the software.
     * @param version The version of the software.
     */
    virtual FormatedText softwareHeader(const Info& name, const Info& version) = 0;

};

} // namespace rbs::report::logging::templates


#endif // INTERFACE_H

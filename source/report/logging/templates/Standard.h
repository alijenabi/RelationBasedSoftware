//
//  Standard.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 13.12.20.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef STANDARD_H
#define STANDARD_H

#include "Interface.h"
#include "../../date_time.h"
#include "../../text_editing.h"

/**
 * The templates interface provides the templates for logging.
 */
namespace rbs::report::logging::templates {

/**
 * @brief The standard logging template, formats the logging information for passing to the file or command line.
 */
class Standard: public Interface
{
public:

    /**
     * @brief The Standard constructor.
     */
    Standard();

    /**
     * @brief Clones the opject.
     * @param isCopy Whether copy the existing or create a new object.
     */
    virtual std::shared_ptr<Interface> clone(const bool isCopy) override;

    /**
     * @brief Gives the items that do not require information.
     * @param tag The tag.
     */
    virtual FormatedText item(const Tag tag) override;

    /**
     * @brief Formats the information based on the given tag.
     * @param tag The tag.
     * @param info The information.
     */
    virtual FormatedText item(const Tag tag, const Info& info) override;

    /**
     * @brief Gives the software footer.
     * @note Not defining this as a "const" member will allow more free template implementation downstream.
     * @param name The name of the software.
     * @param version The version of the software.
     */
    virtual FormatedText softwareFooter(const Info& name, const Info& version) override;

    /**
     * @brief Gives the software header.
     * @note Not defining this as a "const" member will allow more free template implementation downstream.
     * @param name The name of the software.
     * @param version The version of the software.
     */
    virtual FormatedText softwareHeader(const Info& name, const Info& version) override;

protected:

    using BlockID = size_t;
    using Time = std::shared_ptr<time_t>;

    /**
     * @brief Gives the current date in string format.
     */
    FormatedText currentDate() const;

    /**
     * @brief Gives the current time in string format.
     */
    FormatedText currentTime() const;

    /**
     * @brief Gives an empty line.
     */
    FormatedText emptyLine() const;

    /**
     * @brief Gives a formated paragraph.
     */
    FormatedText paragraph(const Info& info) const;

    /**
     * @brief Gives the software header and footer info block.
     */
    FormatedText softwareBlockInfo(const Info& name, const Info& version) const;

    /**
     * @brief Gives the software header and footer block seperator line.
     */
    FormatedText softwareBlockSeperator() const;

    /**
     * @brief Gives the formated header.
     * @param info The info.
     * @param header The number of header level.
     */
    FormatedText header(const Info& info, const size_t header);

    /**
     * @brief Gives the block footer.
     * @note Provides the time of the block at the end too.
     */
    FormatedText blockFooter();

    /**
     * @brief Gives the block beader.
     * @param title The title of the block.
     */
    FormatedText blockHeader(const Info& title);

    /**
     * @brief Gives the inner block footer.
     * @return The formated inner block for the open one, an empty string otherwise.
     */
    FormatedText innerBlockFooter();

    /**
     * @brief Gives the inner block header.
     * @warning Any title will be fit to 1/9 of the page width.
     * @throws If the page width dose not allow inner block (i.e., page width < 15 + fited title length).
     */
    FormatedText innerBlockHeader(const Info& title);

    /**
     * @brief Checks whether the block is open or not.
     * @returns A block footer if block is open, an empty string if its already closed.
     */
    FormatedText isBlockClosed();

    /**
     * @brief Checks whether the block is open or not.
     * @returns A block header if block is closed, an empty string if its already opened.
     */
    FormatedText isBlockOpened();

    /**
     * @brief Checks whether the inner block is open or not.
     * @returns An inner block footer if inner block is open, an empty string if its already closed.
     */
    FormatedText isInnerBlockClosed();

    /**
     * @brief Checks whether the inner block is open or not.
     * @returns An inner block header if inner block is closed, an empty string if its already opened.
     */
    FormatedText isInnerBlockOpened();

private:

    char p_chBlockHorizontal;   // The character to create the left side of the blocks.
    char p_chBlockVertical;     // The character to create a vertical line above the block header and at the end of the block.
    char p_chEmpty;             // The character to create empty space around the text
    char p_chFill;              // The charecter to fill the space on the header and footer.
    char p_chHeader;            // The character to show the level of header.
    char p_chItem;              // The character to show the items on the process of a block.
    char p_chParagraph;         // The character to start the paragraph.
    char p_chSoftware;          // The character of header/Footer spacer at the top and below of them.

    Time p_current;             // Denotes the start of the current block, nullprt if not inside a block.
    size_t p_innerBlockWidth;   // Denotes the width of the inner block, zero if not inside the inner block.
    size_t p_lastHeader;        // Denotes the level of the last printed header.
    size_t p_width;             // Denotes the width of the formated document.
    Time p_start;               // Denotes the time of printing the software header, nullptr if not printed yet.

};

} // namespace rbs::report::logging::templates

#endif // STANDARD_H

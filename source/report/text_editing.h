//
//  text_editing.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 13.12.20.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef TEXT_EDITING_H
#define TEXT_EDITING_H

#include <string>
#include <vector>

/**
 * The time namespace provides the means to edit text.
 */
namespace rbs::report::text_editing {

/**
 * @brief Whether a character is printable (i.e., human-readable) or not.
 */
bool isPrintable(const char c);

/**
 * @brief Whether a character is a white space or not.
 */
bool isWhite(char c);

/**
 * @brief Whether a character is alphabetic or not.
 */
bool isAlphabetic(const char c);

/**
 * @brief Whether a character is numeric or not.
 */
bool isNumberic(const char c);

/**
 * @brief Fits a text to spesific width.
 * @param text The text.
 * @param width The width.
 * @return A list of texts with smaller or equal lenght to width.
 */
std::vector<std::string> fitTo(const std::string text, const size_t width);

/**
 * @brief Removes the first word from string.
 * @param[in, out] str The string.
 * @return The removed word, or empty string if not found.
 */
std::string removeFirstWord(std::string& str);

/**
 * @brief Extracts the words inside a text.
 * @param text The text.
 * @return An list of words.
 */
std::vector<std::string> words(const std::string& text);


} // namespace rbs::report::text_editing

#endif // TEXT_EDITING_H

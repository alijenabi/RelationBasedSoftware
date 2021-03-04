//
//  text_editing.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 13.12.20.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "text_editing.h"

namespace rbs::report::text_editing {

bool isPrintable(const char c) {
    return c >= ' ';
}

bool isWhite(char c) {
    return c <= ' ';
}

bool isAlphabetic(const char c) {
    return ('a' < c && c < 'z') || ('A' < c && c < 'Z');
}

bool isNumberic(const char c) {
    return '0' < c && c < '9';
}

std::vector<std::string> fitTo(const std::string text, const size_t width) {
    if (width == 0)
        throw std::runtime_error("Cannot fill a text in a column with width of zero.");

    std::vector<std::string> result;
    const auto breakBack = [&result, &width]() {
        const auto begin = result.back().begin();
        const auto end = begin + width;
        result.insert(result.end() - 1, std::string(begin, end));

        const auto newBegin = result.back().begin();
        result.back().erase(newBegin, newBegin + width);
    };

    const auto ws = words(text);
    for (const auto& w : ws) {
        if (result.empty())
            result.push_back(w);
        else if (result.back().length() + 1 + w.length() < width)
            result.back() += " " + w;
        else
            result.push_back(w);

        while (result.back().length() > width)
            breakBack();
    }

    return result;
}

std::vector<std::string> words(const std::string &text) {
    std::vector<std::string> ans;
    auto t = text;
    ans.emplace_back(removeFirstWord(t));
    while(!t.empty()) {
        ans.emplace_back( removeFirstWord(t) );
    }
    if (ans.back().empty()) { ans.pop_back(); }
    return ans;
}

std::string removeFirstWord(std::string &str) {
    const auto wordBegin = std::find_if_not(str.begin(), str.end(), isWhite);
    const auto wordEnd = std::find_if(wordBegin, str.end(), isWhite);
    if (wordBegin != wordEnd) {
        const auto ans = std::string(wordBegin, wordEnd);
        str.erase(str.begin(), wordEnd + 1);
        return ans;
    }
    return "";
}

} // namespace rbs::report::text_editing

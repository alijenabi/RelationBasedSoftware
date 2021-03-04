//
//  Standard.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 13.12.20.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "Standard.h"

namespace rbs::report::logging::templates {

Standard::Standard()
    : p_chBlockHorizontal{'_'}
    , p_chBlockVertical{'|'}
    , p_chEmpty{' '}
    , p_chFill{' '}
    , p_chHeader{'-'}
    , p_chItem{'+'}
    , p_chParagraph{'*'}
    , p_chSoftware{'='}
    , p_current{nullptr}
    , p_innerBlockWidth{0}
    , p_lastHeader{0}
    , p_width{90}
    , p_start{nullptr}
{}

std::shared_ptr<Interface> Standard::clone(const bool isCopy) {
    if (isCopy)
        return std::make_shared<Standard>(*this);
    return std::make_shared<Standard>();
}

Interface::FormatedText Standard::item(const Interface::Tag tag) {
    return item(tag, "");
}

Interface::FormatedText Standard::item(const Interface::Tag tag, const Interface::Info &info) {
    const auto blockReady = [&]() { return isBlockOpened() + isInnerBlockClosed(); };

    switch (tag) {
    case BlockFooter:           return blockReady()         + isBlockClosed();
    case BlockHeader:           return isInnerBlockClosed() + isBlockClosed() + blockHeader(info);
    case InnerBlockFooter:      return blockReady();
    case InnerBlockHeader:      return blockReady() + innerBlockHeader(info);
    case H1:                    return blockReady() + header(info, 1);
    case H2:                    return blockReady() + header(info, 2);
    case H3:                    return blockReady() + header(info, 3);
    case H4:                    return blockReady() + header(info, 4);
    case H5:                    return blockReady() + header(info, 5);
    case H6:                    return blockReady() + header(info, 6);
    case P:                     return blockReady()         + paragraph(info);
    case InnerBlockParagraph:   return isInnerBlockOpened() + paragraph(info);
    case Space:                 return emptyLine();
    }
    throw std::runtime_error("Unknown log tag.");
}

Interface::FormatedText Standard::softwareFooter(const Interface::Info &name, const Interface::Info &version) {
    auto footer =  isInnerBlockClosed() + isBlockClosed()
                  + softwareBlockSeperator()
                  + emptyLine()
                  + softwareBlockInfo(name, version)
                  + emptyLine();

    if (p_start) {
        const auto duration = double(clock() - *p_start) / CLOCKS_PER_SEC;
        const auto formated = "Finished after " + date_time::duration::formated(duration, 3);
        footer += FormatedText(p_width - formated.length() - 2, p_chSoftware) + FormatedText(1, p_chBlockVertical) + FormatedText(1, p_chEmpty) + formated + "\n";
    } else  {
        footer += softwareBlockSeperator() + "\n";
    }
    return footer;
}

Interface::FormatedText Standard::softwareHeader(const Interface::Info &name, const Interface::Info &version) {
    p_start = std::make_shared<time_t>( clock() );
    return isInnerBlockClosed() + isBlockClosed()
           + softwareBlockSeperator()
           + emptyLine()
           + softwareBlockInfo(name, version)
           + emptyLine()
           + softwareBlockSeperator() + "\n";
}

Interface::FormatedText Standard::currentDate() const
{
    return date_time::formatedNow("Date: %d %B %G, %A", p_width - 10);
}

Interface::FormatedText Standard::currentTime() const
{
    return date_time::formatedNow("Time: %R:%S %p %z", p_width - 10);
}

Interface::FormatedText Standard::emptyLine() const {
    if (p_innerBlockWidth) // inside the inner block
        return FormatedText(1, p_chBlockVertical) + FormatedText(3, p_chEmpty) + FormatedText(1, p_chBlockVertical) + FormatedText(p_innerBlockWidth, p_chFill) + FormatedText(1, p_chBlockVertical) + "\n";
    if (p_current) // inside the block
        return FormatedText(1, p_chBlockVertical) + "\n";
    return FormatedText(p_width, p_chEmpty) + "\n";
}

Interface::FormatedText Standard::paragraph(const Interface::Info &info) const {
    FormatedText ans = "";
    bool isFirst = true;
    const auto lines = text_editing::fitTo(info, (p_innerBlockWidth)? p_innerBlockWidth - 5 : p_width - (5 + p_lastHeader));
    for ( const auto& line : lines ) {
        if (isFirst) {
            isFirst = false;
            if (p_innerBlockWidth) {
                ans += FormatedText(1, p_chBlockVertical) + FormatedText(3, p_chEmpty) + FormatedText(1, p_chBlockVertical)                                                             // pre
                       + FormatedText(2, p_chFill) + FormatedText(1, p_chParagraph) + FormatedText(1, p_chFill) + line + FormatedText(p_innerBlockWidth - line.length() - 5, p_chFill)  // content
                       + FormatedText(1, p_chFill) + FormatedText(1, p_chBlockVertical);                                                                                                // post
            } else {
                ans += FormatedText(1, p_chBlockVertical) + FormatedText(2 + p_lastHeader, p_chEmpty) + FormatedText(1, p_chParagraph) + FormatedText(1, p_chEmpty) + line;
            }
        } else {
            if (p_innerBlockWidth) {
                ans += FormatedText(1, p_chBlockVertical) + FormatedText(3, p_chEmpty) + FormatedText(1, p_chBlockVertical) // pre
                       + FormatedText(4, p_chFill) + line + FormatedText(p_innerBlockWidth - line.length() - 5, p_chFill)   // content
                       + FormatedText(1, p_chFill) + FormatedText(1, p_chBlockVertical);                                    // post
            } else {
                ans += FormatedText(1, p_chBlockVertical) + FormatedText(4 + p_lastHeader, p_chEmpty) + line;
            }
        }
        ans += "\n";
    }
    return ans;
}

Interface::FormatedText Standard::softwareBlockInfo(const Interface::Info &name, const Interface::Info &version) const {
    const auto date = currentDate();
    const auto time = currentTime();
    const auto fullVersion = "Version: " + version;
    const auto innerBlockMaxWidth = std::max({date.length(), time.length(), fullVersion.length(), name.length()});

    if ( innerBlockMaxWidth + 4 >= p_width )
        throw std::runtime_error("The software block width is larger than the loging width.");

    const auto distSize = 2;
    const auto preSize = (p_width - innerBlockMaxWidth) / 2 - distSize;
    const auto pre = FormatedText(preSize, p_chFill);
    const auto post = FormatedText(p_width - preSize - innerBlockMaxWidth - distSize * 2, p_chFill);
    const auto dist = FormatedText(distSize, p_chEmpty);

    const auto boxLine = [&](const FormatedText& text) {
        return pre + dist + text + FormatedText(innerBlockMaxWidth - text.length() + distSize, p_chEmpty) + post + "\n";
    };

    return boxLine(name)
           + boxLine(FormatedText(innerBlockMaxWidth, p_chEmpty))
           + boxLine(fullVersion)
           + boxLine(date)
           + boxLine(time);
}

Interface::FormatedText Standard::softwareBlockSeperator() const {
    const auto line = FormatedText(p_width, p_chSoftware) + "\n";
    return line;
}

Interface::FormatedText Standard::header(const Interface::Info &info, const size_t header) {
    p_innerBlockWidth = 0; // remove any open inner block.
    if (header > 6 || header == 0) { throw std::runtime_error("Header level is not supported."); }
    p_lastHeader = header;

    FormatedText ans = "";
    bool isFirst = true;
    const auto lines = text_editing::fitTo(info, p_width - (3 + header));
    for ( const auto& line : lines ) {
        if (isFirst) {
            isFirst = false;
            ans += FormatedText(1, p_chBlockVertical) + FormatedText(1, p_chItem) + FormatedText(header, p_chHeader) + FormatedText(1, p_chEmpty) + line;
        } else {
            ans += FormatedText(1, p_chBlockVertical) + FormatedText(2 + header, p_chEmpty) + line;
        }
        ans += "\n";
    }

    return ans;
}

Interface::FormatedText Standard::blockFooter() {
    if (p_current) {
        const auto duration = double(clock() - *p_current) / CLOCKS_PER_SEC;
        const auto timingText = FormatedText(1, p_chBlockVertical) + FormatedText(1, p_chEmpty) + date_time::duration::formated(duration, 2);

        p_current = nullptr;
        return FormatedText(1, p_chBlockVertical) + FormatedText(p_width - timingText.length() - 1, p_chBlockHorizontal) + timingText + "\n\n";
    }
    return FormatedText(1, p_chBlockVertical) + FormatedText(p_width - 1, p_chBlockHorizontal) + "\n\n";
}

Interface::FormatedText Standard::blockHeader(const Interface::Info &title) {
    p_current = std::make_shared<time_t>( clock() );
    const auto replace = [this](Info line) { std::replace(line.begin(), line.end(), ' ', p_chBlockHorizontal); return line; };

    FormatedText ans = "";
    bool isFirst = true;
    const auto lines = text_editing::fitTo(title, p_width / 2 - 1);
    for (const auto& line : lines ) {
        if (isFirst) {
            isFirst = false;
            ans += FormatedText(p_width - line.length(), p_chBlockHorizontal) + replace(line);
        } else {
            ans += FormatedText(1, p_chBlockVertical) + FormatedText(p_width - line.length() - 1, p_chEmpty) + line;
        }
        ans += "\n";
    }
    ans += FormatedText(1, p_chBlockVertical) + "\n";
    return ans;
}

Interface::FormatedText Standard::innerBlockFooter() {
    if(p_innerBlockWidth){
        const auto ans = FormatedText(1, p_chBlockVertical) + FormatedText(3, p_chEmpty) + FormatedText(1, p_chBlockVertical) + FormatedText(p_innerBlockWidth, p_chBlockHorizontal) + FormatedText(1, p_chBlockVertical) + "\n"
                         + FormatedText(1, p_chBlockVertical) + "\n";
        p_innerBlockWidth = 0;
        return ans;
    }
    return "";
}

Interface::FormatedText Standard::innerBlockHeader(const Interface::Info &title) {
    auto fitedTitle = title;
    const auto begin = fitedTitle.begin();
    if ( fitedTitle.end() - begin > static_cast<long>(p_width / 9) )
        fitedTitle.erase(begin, begin + (p_width / 9));

    if (p_width < 15 + fitedTitle.length())
        throw std::runtime_error("Cannot create an inner block. (not enough space)");

    p_innerBlockWidth = p_width - (fitedTitle.length() + 6) - 7;
    return FormatedText(1, p_chBlockVertical) + "\n" +
            FormatedText(1, p_chBlockVertical) + FormatedText(3, p_chHeader) + FormatedText(1, p_chItem) + FormatedText(p_innerBlockWidth ,p_chHeader) + FormatedText(1, p_chItem) + "-< " + fitedTitle + " >-\n" +
            emptyLine();
}

Interface::FormatedText Standard::isBlockClosed() {
    if (p_current) // If a block is already open, close it first.
        return blockFooter();
    return "";
}

Interface::FormatedText Standard::isBlockOpened() {
    if (!p_current)
        return blockHeader("Task");
    return "";
}

Interface::FormatedText Standard::isInnerBlockClosed() {
    if (p_innerBlockWidth) // there is a open inner block, close it.
        return innerBlockFooter();
    return "";
}

Interface::FormatedText Standard::isInnerBlockOpened() {
    if (!p_innerBlockWidth)
        return innerBlockHeader("Insight");
    return "";
}

} // namespace rbs::report::logging::templates

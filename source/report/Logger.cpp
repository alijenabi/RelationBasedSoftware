//
//  Logger.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 08.03.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Logger.h"

namespace rbs::report {

Logger &Logger::centre() {
    static Logger single;
    return single;
}

void Logger::initialize() {
    if (p_isLocked) {
        log(Broadcast::Exeption, "The logger is locked but initialization request recived by it again.");
        throw std::runtime_error("Initializer is already sent.");
    }

    p_isLocked = true;
}

void Logger::log(const Logger::Broadcast item, const Logger::Info &info) {
    const auto itemLevel = recieverLevel(item);
    if ( p_clLevels != RecieverLevel::Off && itemLevel <= p_clLevels ) {
        std::cout << softwareHeaderPrinted(p_clInfo) << format(item, info, p_clInfo);
    }

    const auto highestReciever = recieversHighestItemLevel();
    if ( highestReciever != RecieverLevel::Off && itemLevel <= highestReciever )
        std::for_each(p_recievers.begin(), p_recievers.end(), [&](auto& pair) {
            if ( pair.first != RecieverLevel::Off && itemLevel <= pair.first ) {
                pair.second.first.append(softwareHeaderPrinted(pair.second.second) + format(item, info, pair.second.second), true);
            }
        });
}

void Logger::setCommandLineLevel(const Logger::RecieverLevel level) {
    if (p_isLocked) {
        log(Broadcast::Exeption, "The commandline reciever level cannot be changed after initiation of the logger.");
        throw std::runtime_error("The logger recievers are locked.");
    }

    p_clLevels = level;
    log(Broadcast::Detail, "The broadcasted information with a higher or equal level to " + to_string(level) + " will be writing the command line.");
    p_isLocked = false;
}

void Logger::setFileLevel(const Logger::RecieverLevel level, const std::string &path, const std::string &fileName) {
    if (p_isLocked) {
        log(Broadcast::Exeption, "The file recievers level cannot be changed or created after initiation of the logger.");
        throw std::runtime_error("The logger recievers are locked.");
    }

    const auto newPath = path + fileName + ".log";
    const auto search = std::find_if(p_recievers.begin(), p_recievers.end(), [&level, &newPath](const auto& pair) {
        return pair.second.first.fullName(true).compare(newPath) == 0 && level != pair.first;
    });

    if (search != p_recievers.end()) {
        log(Broadcast::Exeption, "The Logger centre recived two or more reciever files with the same file path.");
        throw std::runtime_error("The reciever file path is already used.");
    }
        ReceiverInfo recieverInfo = { false, Broadcast::Block, 1, p_clInfo.tempPtr->clone(false)};
        FilePair filePair = std::pair{exporting::TextFile(path, fileName, "log"), recieverInfo};
        p_recievers.insert(std::pair{level, filePair});
        log(Broadcast::Detail, "The broadcasted information with a higher or equal level to " + to_string(level) + " will be writing to the " + newPath + ".");
        p_isLocked = false;
}

void Logger::setTemplate(const Logger::TemplatePtr &logTemplate) {
    if (p_isLocked) {
        log(Broadcast::Exeption, "The logger template cannot be altered after initiation of the logger.");
        throw std::runtime_error("The logger recievers are locked.");
    }

    log(Broadcast::Detail, "The logger template is set to all recievers.");

    p_clInfo.tempPtr = logTemplate;
    for(auto& reciever: p_recievers)
        reciever.second.second.tempPtr = logTemplate;

    p_isLocked = false;
}

Logger::FormatedInfo Logger::format(const Logger::Broadcast item, const Logger::Info &info, Logger::ReceiverInfo &recieverInfo) {
    switch (item) {
    case Broadcast::Exeption:
    case Broadcast::Error:
    case Broadcast::Warning:
        return formatInnerBlock(item, info, recieverInfo);
    case Broadcast::InnerBlock:
        recieverInfo.lastItem = item;
        return recieverInfo.tempPtr->item(Tag::InnerBlockHeader, info);
    case Broadcast::InnerBlockInfo:
        return formatInnerBlock(item, info, recieverInfo);
    case Broadcast::Block:
        recieverInfo.processLevel = 1;
        recieverInfo.lastItem = item;
        return recieverInfo.tempPtr->item(Tag::BlockHeader, info);
    case Broadcast::ProcessStart: {
        recieverInfo.lastItem = item;
        const auto level = std::min(recieverInfo.processLevel + 1, 6);
        recieverInfo.processLevel = level;
        if (info.compare("") == 0)
            return "";
        return formatProcess(recieverInfo.processLevel, info, recieverInfo.tempPtr);
    }
    case Broadcast::ProcessEnd:  {
        recieverInfo.lastItem = item;
        const auto level = std::max(recieverInfo.processLevel - 1, 1);
        recieverInfo.processLevel = level;
        if (info.compare("") == 0)
            return "";
        return formatProcess(level, info, recieverInfo.tempPtr);
    }
    case Broadcast::ProcessTiming:
    case Broadcast::Process:
        recieverInfo.lastItem = item;
        return formatProcess(recieverInfo.processLevel, info, recieverInfo.tempPtr);
    case Broadcast::Detail:
        recieverInfo.lastItem = item;
        return recieverInfo.tempPtr->item(Tag::P, info);
    }
}

Logger::RecieverLevel Logger::recieversHighestItemLevel() const {
    const auto getHighest = [this]() {
        auto highest = RecieverLevel::Off;
        for(const auto& reciever : p_recievers)
            if (reciever.first > highest)
                highest = reciever.first;
        return highest;
    };

    if (p_isLocked) {
        static bool isFirst = true;
        static auto highest = RecieverLevel::Off;
        if (isFirst) {
            highest = getHighest();
        }
        return highest;
    }

    return getHighest();
}

Logger::RecieverLevel Logger::recieverLevel(const Logger::Broadcast item) const {
    switch (item) {
    case Broadcast::Exeption:
    case Broadcast::Error:          return RecieverLevel::Error;
    case Broadcast::Warning:        return RecieverLevel::Warning;
    case Broadcast::Block:
    case Broadcast::InnerBlock:
    case Broadcast::InnerBlockInfo:
    case Broadcast::ProcessStart:
    case Broadcast::Process:
    case Broadcast::ProcessEnd:     return RecieverLevel::Process;
    case Broadcast::ProcessTiming:  return RecieverLevel::Timing;
    case Broadcast::Detail:         return RecieverLevel::Debug;
    }
    throw std::runtime_error("The Broadcast item is not recognized.");
}

Logger::FormatedInfo Logger::softwareHeaderPrinted(Logger::ReceiverInfo &receiverInfo) {
    if (!receiverInfo.hadHeader) {
        receiverInfo.hadHeader = true;
        return receiverInfo.tempPtr->softwareHeader(softwareName, softwareVersion);
    }
    return "";
}

std::string Logger::to_string(Logger::Broadcast item) const {
    switch (item) {
    case Broadcast::Error:          return "Error";
    case Broadcast::Exeption:       return "Exeption";
    case Broadcast::Warning:        return "Warning";
    case Broadcast::InnerBlock:     return "InnerBlock";
    case Broadcast::InnerBlockInfo: return "InnerBlockInfo";
    case Broadcast::Block:          return "Block";
    case Broadcast::ProcessStart:   return "ProcessStart";
    case Broadcast::Process:        return "Process";
    case Broadcast::ProcessEnd:     return "ProcessEnd";
    case Broadcast::ProcessTiming:  return "ProcessTiming";
    case Broadcast::Detail:         return "Detail";
    }
    throw std::runtime_error("The Broadcast item type is not recognized.");
}

std::string Logger::to_string(Logger::RecieverLevel level) const {
    switch (level) {
    case Off:       return "turned off";
    case Error:     return "Error";
    case Warning:   return "Warning";
    case Process:   return "Process";
    case Timing:    return "Timing";
    case Debug:     return "Debug";
    }
    throw std::runtime_error("The ReciverLevel is not recognized.");
}

Logger::Logger()
    : p_clLevels{ RecieverLevel::Process }
    , p_clInfo({ false, Broadcast::Block, 1, std::make_shared<logging::templates::Standard>() })
    , p_isLocked{ false }
    , p_recievers{ }
{
}

Logger::~Logger() {
    if ( p_clInfo.hadHeader )
        std::cout << p_clInfo.tempPtr->softwareFooter(softwareName, softwareVersion);

    for(auto& reciever: p_recievers) {
        if (reciever.second.second.hadHeader)
            reciever.second.first.append(reciever.second.second.tempPtr->softwareFooter(softwareName, softwareVersion), true);
    }
}

Logger::FormatedInfo Logger::formatInnerBlock(const Logger::Broadcast item, const Logger::Info &info, Logger::ReceiverInfo &receiverInfo) const {
    constexpr auto isInInnerBlock = [](const Broadcast item) {
        return item == Broadcast::Exeption || item == Broadcast::Error ||
                item == Broadcast::Warning  || item == Broadcast::InnerBlock || item == Broadcast::InnerBlockInfo;
    };

    if(!isInInnerBlock(item))
        throw std::runtime_error("The item is not an inner block item (i.g., Exeption, Error, Warning, InnerBlock, and InnerBlockInfo).");

    FormatedInfo ans = "";
    if ( !( item == Broadcast::InnerBlockInfo
            && (receiverInfo.lastItem == Broadcast::InnerBlockInfo || receiverInfo.lastItem == Broadcast::InnerBlock))) {
        if (item != receiverInfo.lastItem || item == Broadcast::InnerBlock) {
            if (isInInnerBlock(receiverInfo.lastItem)) {
                ans += receiverInfo.tempPtr->item(Tag::InnerBlockFooter);
            }
            ans += receiverInfo.tempPtr->item(Tag::InnerBlockHeader, to_string(item));
        }
    }

    if ( item == receiverInfo.lastItem && receiverInfo.lastItem != Broadcast::InnerBlock)
        ans += receiverInfo.tempPtr->item(Tag::Space);

    ans += receiverInfo.tempPtr->item(Tag::InnerBlockParagraph, info);
    receiverInfo.lastItem = item;
    return ans;
}

Logger::FormatedInfo Logger::formatProcess(const size_t level, const Logger::Info &info, const Logger::TemplatePtr &temp) const {
    switch (level) {
    case 1: return temp->item(Tag::H1, info);
    case 2: return temp->item(Tag::H2, info);
    case 3: return temp->item(Tag::H3, info);
    case 4: return temp->item(Tag::H4, info);
    case 5: return temp->item(Tag::H5, info);
    case 6: return temp->item(Tag::H6, info);
    }
    return temp->item(Tag::P, info);
}

} // namespace rbs::report

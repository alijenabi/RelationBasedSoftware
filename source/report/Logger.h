//
//  Logger.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 08.03.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef LOGGER_H
#define LOGGER_H

#include "../exporting/TextFile.h"
#include "logging/templates/Standard.h"
#include <iostream>
#include <map>
#include <time.h>

/**
 * The report namespace provides the means to report the process of the simulation to the user.
 */
namespace rbs::report {

/**
 * @brief Reports the state of the simulation.
 * @details The logger receives the broadcasting material and distributes them between those receivers that are listening.
 * @note Each process is required to broadcast its own stages, information, ...
 * @note The receivers can be std::cout (i.e., command line) or a text file or both.
 * @note The command line reciving the loged matterial of level Process by default.
 * @note The Logger is a singleton to secure a single broadcast throughout the software.
 */
class Logger
{
public:

    /**
     * @brief Denotes the receiver (i.e., Text File or std::cout) level on reading broadcasted items.
     */
    enum RecieverLevel {
        Off ,       // Recives is turn off
        Error,      // Recives Blocks, Top Processes, and Errors.
        Warning,    // Recives all Error items plus Warnings.
        Process,    // Recives all Warnings items plus all the broadcasted processes.
        Timing,     // Recives all Process items plus broadcasted ProsessTiming items.
        Debug,      // Recives all Timing items plus all the software setups.
    };

    /**
     * @brief The Broadcast enum denotes the type of infomration that can be broadcast by logger.
     * @note Starting a process inside a process creates a sub-process.
     */
    enum class Broadcast {
        Block,          // Broadcasting a new block.                                    =>  RecieverLevel::Process
        Exeption,       // Broadcasting an Exeption (the process will be intropted)     =>  RecieverLevel::Error
        Error,          // Broadcasting an Error.   (the process will be continued)     =>  RecieverLevel::Error
        Warning,        // Broadcasting a Warning.                                      =>  RecieverLevel::Warning
        InnerBlock,     // Broadcasting cosume inner block title.                       =>  RecieverLevel::Process
        InnerBlockInfo, // Broadcasting the information inside the inner block.         =>  RecieverLevel::Process
        ProcessStart,   // Broadcasting a process start.                                =>  RecieverLevel::Process
        Process,        // Broadcasting the continuation of a process.                  =>  RecieverLevel::Process
        ProcessTiming,  // Broadcasting the process timing.                             =>  RecieverLevel::Timing
        ProcessEnd,     // Broadcasting the end of a process.                           =>  RecieverLevel::Process
        Detail,         // Broadcasting detail on the current process or block if no process started yet. =>  RecieverLevel::Debug
    };

    using Info = std::string;
    using Tag = logging::templates::Standard::Tag;
    using TemplatePtr = std::shared_ptr<logging::templates::Interface>;

    static constexpr char * softwareName = (char *)"Relation-Based Simulator (RBS)";
    static constexpr char * softwareVersion = (char *)"1.0.0";

    /**
     * @brief Gives the logger centre (the single broadcast transmitter).
     */
    static Logger& centre();

    /**
     * @brief Initializes the logger.
     * @warning The initiation of the logger locks the receivers.
     * @note The software header will be sent to receivers in the initialization.
     */
    void initialize();

    /**
     * @brief Logs the information on a specific broadcast item.
     * @param item The broadcast item.
     * @param info The log information.
     */
    void log(const Broadcast item, const Info& info);

    /**
     * @brief Sets the command line reciveing level.
     * @param level The new level.
     */
    void setCommandLineLevel(const RecieverLevel level);

    /**
     * @brief Sets the file reciveing level.
     * @note Since the number of open files is limited, only one file per reciever level is posible.
     * @param level The new level.
     * @param path The path of the reciever folder.
     * @param fileName The name of the reciever file without extention.
     * @throws If the path and name is already signed to an other level.
     */
    void setFileLevel(const RecieverLevel level, const std::string& path, const std::string& fileName);

    /**
     * @brief Sets the log template of all the recivers.
     * @param logTemplate The log template.
     */
    void setTemplate(const TemplatePtr& logTemplate);

protected:

    /**
     * @brief The Receiver's specific Info.
     */
    struct ReceiverInfo {
        bool hadHeader = false;     // Whether the receiver printed the header or not.
        Broadcast lastItem;         // Denotes the last broadcasted item.
        int processLevel = 0;    // Denotes the receiver process level.
        TemplatePtr tempPtr;        // Stores a pointer to the template of the reciever.
    };

    using File = exporting::TextFile;
    using FormatedInfo = std::string;
    using FilePair = std::pair<File, ReceiverInfo>;
    using Recievers = std::map<RecieverLevel, FilePair>;

    /**
     * @brief Formats the information for logging.
     * @param item The broadcasted item.
     * @param info The log information.
     */
    FormatedInfo format(const Broadcast item, const Info& info, ReceiverInfo& recieverInfo);

    /**
     * @brief gets the recievers (files) highest item level.
     */
    RecieverLevel recieversHighestItemLevel() const;

    /**
     * @brief Converts the Broadcast item to its relative RecieverLevel.
     */
    RecieverLevel recieverLevel(const Broadcast item) const;

    /**
     * @brief Gives the receiver's software header if not printed yet.
     * @param receiverInfo The reciever info.
     * @return Software header if not printed yet, empty string if is printed already.
     */
    FormatedInfo softwareHeaderPrinted(ReceiverInfo& receiverInfo);

    /**
     * @brief Converts the recieverLevel to string.
     * @throws IF the recieverLevel is not recognized.
     */
    std::string to_string(Broadcast item) const;

    /**
     * @brief Converts the recieverLevel to string.
     * @throws IF the recieverLevel is not recognized.
     */
    std::string to_string(const RecieverLevel level) const;

private:

    /**
     * @brief Constructor.
     * @note This is private since the logger is a singleton.
     */
    Logger();

    /**
     * @brief Destructor.
     */
    ~Logger();

    /**
     * @brief Formats the Error items.
     * @param item An inner block item (i.g., Exeption, Error, Warning, and InnerBlock).
     * @throws If the item is not an inner block.
     */
    FormatedInfo formatInnerBlock(const Broadcast item, const Info& info, ReceiverInfo& receiverInfo) const;

    /**
     * @brief Formats the Process information.
     * @param level The heading level for the process.
     * @param info The process information.
     */
    FormatedInfo formatProcess(const size_t level, const Info& info, const TemplatePtr& temp) const;

    RecieverLevel p_clLevels;   // Levels to report on the command line
    ReceiverInfo p_clInfo;      // Stored the command line reciever info.
    bool p_isLocked;            // Denotes whether the loger recievers are locked or not.
    Recievers p_recievers;      // The targets to export the loging to them.

};

} // namespace rbs::report

#endif // LOGGER_H

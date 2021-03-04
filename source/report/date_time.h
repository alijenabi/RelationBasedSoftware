//
//  date_time.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 08.03.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <algorithm>
#include <map>
#include <string>
#include <time.h>

/**
 * The time namespace provides the means to present time to user interface.
 */
namespace rbs::report::date_time {

/**
 * @brief Gives the formated date time.
 * @param datetime The date time to format.
 * @param format The format of the result.
 * @param size The maximum size of the result.
 */
std::string formatedDateTime(const time_t& datetime, const std::string& format, const size_t size);

/**
 * @brief Gives the current (now) formated date time.
 * @param format The format of the result.
 * @param size The maximum size of the result.
 */
std::string formatedNow(const std::string& format, const size_t size);

/**
 * The duration namespace provides the means to presents time durations to user interface.
 */
namespace duration {

/**
 * @brief The time duration components
 */
enum Component {
    D, H, Min, Sec, Mil, Mic, Nan
};

using Duration = std::map<Component, size_t>;

/**
 * @brief Decomposes the time in seconds to Duration.
 * @param seconds The time in seconds.
 * @param level The maximum number of components to be incuded in the result.
 * @return A map of size level holding the biggest conponets of the time duration.
 */
 Duration decompose(double seconds, int level);

 /**
 * @brief Computes the formated time duration.
 * @param seconds The time in seconds.
 * @param level The maximum number of components to be incuded in the result.
 * @return A string presentation the biggest conponets of the time duration up to the level.
 */
std::string formated(double seconds, const size_t level);

/**
 * @brief Converts the time duration componets (i.e., Component) to string.
 */
std::string to_string(const Component component);

} // namespace duration

}

#endif // DATE_TIME_H

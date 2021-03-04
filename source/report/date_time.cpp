//
//  date_time.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 08.03.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "date_time.h"

namespace rbs::report::date_time {

std::string formatedDateTime(const time_t &datetime, const std::string &format, const size_t size) {
    auto timeinfo = localtime(&datetime);
    char buffer[size];
    strftime(buffer,size,format.c_str(),timeinfo);
    return buffer;
}

std::string formatedNow(const std::string &format, const size_t size) {
    const auto now = time(0);
    return formatedDateTime(now, format, size);
}

std::string duration::formated(double seconds, const size_t level) {
    std::string ans = "";
    const auto components = decompose(seconds, level);
    const auto outLevel = std::min(level, components.size());
    size_t itr = outLevel;
    for(const auto& comp : components) {
        if (!itr) break;

        const auto seperator = (outLevel > 2 && itr != 1)? ", " : "";
        const auto andSeperator = (itr == 2 && outLevel > 1)? " and " : "";
        const auto compName = to_string(comp.first) + ((comp.second > 1)? "s" : "");
        if (comp.second) {
            ans += std::to_string(comp.second) + " " + compName + seperator + andSeperator;
        }

        itr--;
    }
    return ans;
}

duration::Duration duration::decompose(double seconds, int level) {
    Duration ans;

    const auto get = [&seconds, &ans, &level](const Component comp, const size_t per) {
        const size_t count = seconds / per;
        if (count) {
            ans[comp] = count;
            seconds  -= count * per;
            level    -= (count) ? 1 : 0;
        }
        return level <= 0;
    };

    if( get(D, 86400) ) return ans;
    if( get(H,  3600) ) return ans;
    if( get(Min,  60) ) return ans;
    if( get(Sec,   1) ) return ans;

    seconds *= 1e9; // nanoseconds

    if( get(Mil, 1000000) ) return ans;
    if( get(Mic, 1000) )    return ans;
    if( get(Nan, 1) )       return ans;

    return ans;
}

std::string duration::to_string(const duration::Component component) {
    switch (component) {
    case D: return "day";
    case H: return "hour";
    case Min: return "minute";
    case Sec: return "second";
    case Mil: return "millisecond";
    case Mic: return "microsecond";
    case Nan: return "nanosecond";
    }
    throw std::runtime_error("Unknown duration component.");
}

} // namespace rbs::report::date_time

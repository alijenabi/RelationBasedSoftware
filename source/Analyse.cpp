//
//  Analyse.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 08.03.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Analyse.h"
#include "report/Logger.h"

namespace rbs {

Analyse &Analyse::current() {
    static Analyse single;
    return single;
}

const double &Analyse::time() {
    return current().p_time;
}

bool Analyse::canRun() const {
    return p_time < p_maxTime || p_timeIterator < p_maxTimeIterator;
}

size_t Analyse::iterator() const {
    return p_timeIterator;
}

void Analyse::next() {
    p_timeIterator++;
    p_time += p_timeSpan;
}

int Analyse::run() {
    using Logger = report::Logger;
    auto& logger = Logger::centre();
    logger.log(Logger::Broadcast::Block, "Starting the Analyses");
    while(canRun()) {
        const auto& itr = Analyse::current().iterator();
        const auto& time = Analyse::time();
        logger.log(Logger::Broadcast::Block, "Time Iteration #" + std::to_string(itr));
        logger.log(Logger::Broadcast::Process, "Analyse time: " + ( (time) ? report::date_time::duration::formated(time, 6) : "zero" ));
        if (p_handleExeptions) {
            try {
                for (auto& relation: p_relations)
                    relation->apply();
            } catch (std::exception e) {
                logger.log(Logger::Broadcast::Exeption, e.what());
                logger.log(Logger::Broadcast::Exeption, "Turn on the debug mode of the Analyses before debuging the program." );
                logger.log(Logger::Broadcast::Exeption, "Analyse::current().setDebugMode(true);" );
                return EXIT_FAILURE;
            } catch (...) {
                logger.log(Logger::Broadcast::Exeption, "An unknown exception (i.g., non std::exception) thrown.");
                logger.log(Logger::Broadcast::Exeption, "Turn on the debug mode of the Analyses before debuging the program." );
                logger.log(Logger::Broadcast::Exeption, "Analyse::current().setDebugMode(true);" );
                return EXIT_FAILURE;
            }
        } else {
            for (auto& relation: p_relations)
                relation->apply();
        }
        next();
    }
    return EXIT_SUCCESS;
}

void Analyse::setDebugMode(const bool on) {
    p_handleExeptions = !on;
}

void Analyse::setMaxIteration(const size_t iterator) {
    p_maxTimeIterator = iterator;
}

void Analyse::setMaxTime(const double time) {
    p_maxTime = time;
}

void Analyse::setTimeSpan(const double timeSpan) {
    p_timeSpan = timeSpan;
}

Analyse::Analyse()
    : p_handleExeptions{true}
    , p_maxTime{0}
    , p_maxTimeIterator{0}
    , p_relations{}
    , p_time{0}
    , p_timeIterator{0}
    , p_timeSpan{0}
{
}

} // namespace rbs

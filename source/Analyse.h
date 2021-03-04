//
//  Analyse.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 08.03.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef ANALYSE_H
#define ANALYSE_H

#include "relations/Applicable.h"
#include <memory>
#include <vector>

/**
 *  The global namespace for the Relation-Based Simulator (RBS).
 */
namespace rbs {

/**
 * @brief The Analyse is a singleton that allows secure and straightforward multi-processing.
 * @note One can neglect using this class if required. The purpose of this class is to simplify the interactions.
 * @note The Analyse::current() acts as a central message box.
 */
class Analyse
{
public:

    /**
     * @brief The current Analyses.
     * @note The Analysis is a singleton.
     * @note Each of the parts may assign with their series of internal or external relations (analyses).
     */
    static Analyse& current();

    /**
     * @brief Gives the simulation time.
     */
    static const double& time();

    /**
     * @brief Checks if the simulation can run or not.
     */
    bool canRun() const;

    /**
     * @brief Gives the current iteration.
     */
    size_t iterator() const;

    /**
     * @brief Append to the realtion array.
     */
    template<class AR>
    void appendRelation(const AR& relation) {
        p_relations.push_back(std::make_shared<AR>(relation));
    }

    /**
     * @brief Jumps to the next timestep.
     */
    void next();

    /**
     * @brief Runs the simulation.
     * @return Zero on success, and one at failure.
     */
    int run();

    /**
     * @brief Set the debug mode.
     * @param on If true, the analysis will avoid exceptions and closes the software,
     *           If false, the analysis will abrupt by raised exception, which can be explored while debugging.
     */
    void setDebugMode(const bool on);

    /**
     * @brief Sets the max number of iterations.
     */
    void setMaxIteration(const size_t iterator);

    /**
     * @brief Sets the max simulation time.
     */
    void setMaxTime(const double time);

    /**
     * @brief Sets the span between the time steps.
     */
    void setTimeSpan(const double timeSpan);

private:

    using Relation = std::shared_ptr<relations::Applicable>;
    using Relations = std::vector<Relation>;

    /**
     * @brief The constructor.
     * @note This is private since this class is a singleton.
     */
    Analyse();

    bool p_handleExeptions;         // Whether to handle the exception at runtime or not.
    double p_maxTime;               // The maximum simulation time.
    double p_maxTimeIterator;       // The maximum number of time iterations.
    Relations p_relations;          // The list of relations that has to be applied on each timestep.
    double p_time;                  // The current simulation time.
    size_t p_timeIterator;          // The current timestep number.
    double p_timeSpan;              // The distance between the timesteps.
};

} // namespace rbs

#endif // ANALYSE_H

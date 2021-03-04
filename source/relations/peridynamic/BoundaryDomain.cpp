//
//  BoundaryDomain.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "BoundaryDomain.h"
#include "../../configuration/Part.h"
#include "../../report/Logger.h"

namespace rbs::relations::peridynamic {

BoundaryDomain::BoundaryDomain(const BoundaryDomain::Conditioner &conditioner, BoundaryDomain::BoundaryPart &part)
    : Base(0, part, [](const SimulationTime&, BoundaryPart&) {} )
    , p_conditioner{conditioner}
    , p_time{ Analyse::time() }
{
    setFeeder( p_time );
    setRelationship([this](const SimulationTime& time, BoundaryPart& part) {
        using Logger = report::Logger;
        const auto start_t = clock();
        auto& logger = Logger::centre();
        logger.log(Logger::Broadcast::Process, "Applying boundary condition to the \"" + part.name() + "\" Part.");

        const auto& neighborhoods = part.neighborhoods();
        std::for_each(neighborhoods.begin(), neighborhoods.end(), [this, &time](const BoundaryPart::NeighborhoodPtr& neighborhood) {
            auto& centre = neighborhood->centre();
            p_conditioner(time, *centre);
        });

        const size_t problemSize = neighborhoods.size();
        const long dist = clock() - start_t;
        const auto duration = double(dist) / CLOCKS_PER_SEC;
        const auto durationText = report::date_time::duration::formated(duration, 3);
        if (problemSize) {
            logger.log(Logger::Broadcast::ProcessStart, std::to_string(problemSize) + " Node" + ((problemSize == 1)? " ": "s ") + " updated.");
            logger.log(Logger::Broadcast::ProcessStart, "");
            logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText)  + ".");
            logger.log(Logger::Broadcast::ProcessTiming, (dist) ? std::to_string( dist / problemSize ) + " CPU clock per Node." : "Less than a CPU clock per Node.");
            logger.log(Logger::Broadcast::ProcessTiming, report::date_time::duration::formated(duration / problemSize, 3) + " per Node.");
            logger.log(Logger::Broadcast::ProcessEnd, "");
            logger.log(Logger::Broadcast::ProcessEnd, "");
        } else {
            logger.log(Logger::Broadcast::Warning, "No Node found.");
            logger.log(Logger::Broadcast::ProcessStart, "");
            logger.log(Logger::Broadcast::ProcessTiming, "Done in " + durationText + ".");
            logger.log(Logger::Broadcast::ProcessEnd, "");
        }
    });
}

} // namespace rbs::relations::peridynamic

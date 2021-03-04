//
//  PDVelocityVerletAlgorithm.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//
//  @note PD prefix is added to avoid the name confilict by the linker.

#include "PDVelocityVerletAlgorithm.h"
#include "../../../Analyse.h"
#include "../../../configuration/Part.h"
#include <algorithm>

namespace rbs::relations::peridynamic::time_integration {

VelocityVerletAlgorithm::VelocityVerletAlgorithm(VelocityVerletAlgorithm::PDPart &part)
    : Base(0, part, [](const SimulationTime&, PDPart&) {} )
    , p_dt{0}
    , p_lastTimeStep{0}
    , p_time{ Analyse::time() }
{
    using Logger = report::Logger;
    auto& logger = Logger::centre();

    setFeeder( p_time );
    setRelationship([&](const SimulationTime& time, PDPart& part) {
        p_dt = time - p_lastTimeStep;
        p_lastTimeStep = time;

        using namespace space;
        vec3 a_t, v_t, x_t, v_t_2, force, a, v, x, bodyForce;
        const auto update = [&](const auto& begin, const auto& end) {
            const auto start_t = clock();
            logger.log(Logger::Broadcast::Process, "Applying Velocity Verlet algorithm to \"" + part.name() + "\" Part.");
            logger.log(Logger::Broadcast::ProcessStart, "");

            long problemSize = 0;
            std::for_each(begin, end, [&](const BoundaryPart::NeighborhoodPtr& neighborhood) {
                auto& centre = *neighborhood->centre();
                if (!centre.has(Property::Density))
                    throw std::runtime_error("The time integration cannot proceed without density at all of the Nodes.");
                if (!centre.has(Property::Volume))
                    throw std::runtime_error("The time integration cannot proceed without volume at all of the Nodes.");

                a_t = (centre.has(Property::Acceleration)) ? centre.at(Property::Acceleration).value<space::vec3>() : space::consts::o3D;
                v_t = (centre.has(Property::Velocity)) ? centre.at(Property::Velocity).value<space::vec3>() : space::consts::o3D;
                x_t = (centre.has(Property::Displacement)) ? centre.at(Property::Displacement).value<space::vec3>() : space::consts::o3D;

                v_t_2 = v_t + a_t * p_dt / 2;

                bodyForce = centre.has(Property::BodyForce) ? centre.at(Property::BodyForce).value<space::vec3>() : space::consts::o3D;
                force = (centre.has(Property::Force)) ? centre.at(Property::Force).value<space::vec3>() : space::consts::o3D;

                a = (force + bodyForce) / centre.at(Property::Density).value<double>();
                v = v_t_2 + a * p_dt / 2;
                x = x_t + v * p_dt + a * p_dt * p_dt / 2;

                if (centre.has(Property::Acceleration) || !a.isZero()) {
                    centre.at(Property::Acceleration) = a;
                    problemSize++;
                }

                if (centre.has(Property::Velocity) || !v.isZero()) {
                    centre.at(Property::Velocity) = v;
                    problemSize++;
                }

                if (centre.has(Property::Displacement) || !x.isZero()) {
                    centre.at(Property::Displacement) = x;
                    problemSize++;
                }
            });

            const auto dist = clock() - start_t;
            const auto duration = double(dist) / CLOCKS_PER_SEC;
            const auto durationText = report::date_time::duration::formated(duration, 2) ;

            if (problemSize) {
                logger.log(Logger::Broadcast::Process, std::to_string(problemSize) + " node's properties updated.");
                logger.log(Logger::Broadcast::ProcessStart, "");
                logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText) + ".");
                logger.log(Logger::Broadcast::ProcessTiming, std::to_string( dist / problemSize ) + " CPU Clock per neighborhood.");
                logger.log(Logger::Broadcast::ProcessTiming, "or " + report::date_time::duration::formated( duration / problemSize, 3) + " per node's property.");
                logger.log(Logger::Broadcast::ProcessEnd, "");
            } else {
                logger.log(Logger::Broadcast::Process, "No node's properties updated.");
                logger.log(Logger::Broadcast::ProcessStart, "");
                logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText) + ".");
                logger.log(Logger::Broadcast::ProcessEnd, "");
            }

            logger.log(Logger::Broadcast::ProcessEnd, "");
        };

        const auto start_t = clock();
        const auto& neighborhoods = part.neighborhoods();

        update(neighborhoods.begin(), neighborhoods.end());

        const auto problemSize = neighborhoods.size();
        const auto dist = clock() - start_t;
        const auto duration = double(dist) / CLOCKS_PER_SEC;
        const auto durationText = report::date_time::duration::formated(duration, 3) ;

        if (problemSize) {
            logger.log(Logger::Broadcast::Process, std::to_string(problemSize) + " neighborhood updated.");
            logger.log(Logger::Broadcast::ProcessStart, "");
            logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText) + ".");
            logger.log(Logger::Broadcast::ProcessTiming, std::to_string( dist / problemSize ) + " CPU Clock per neighborhood.");
            logger.log(Logger::Broadcast::ProcessTiming, "or " + report::date_time::duration::formated( duration / problemSize, 3) + " per neighborhood.");
            logger.log(Logger::Broadcast::ProcessEnd, "");
        } else  {
            logger.log(Logger::Broadcast::Process, "no neighborhood found.");
            logger.log(Logger::Broadcast::ProcessStart, "");
            logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText) + ".");
            logger.log(Logger::Broadcast::ProcessEnd, "");
        }
    });
}

} // namespace rbs::relations::peridynamic::time_integration

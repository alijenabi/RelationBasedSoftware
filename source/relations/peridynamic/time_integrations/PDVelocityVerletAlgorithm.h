//
//  PDVelocityVerletAlgorithm.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//
//  @note PD prefix is added to avoid the name confilict by the linker.

#ifndef VelocityVerletAlgorithm_H
#define VelocityVerletAlgorithm_H

#include "../Property.h"
#include "../../Relation.h"

/**
 * The configuration namespace provides the means to create configurations and use them.
 */
namespace rbs::configuration {

class Part;

}

/**
 * @brief The time integration namespace
 */
namespace rbs::relations::peridynamic::time_integration {

/**
 * @brief Provides easy to use relation for applying velocity verlet algorithm to a PD Part.
 */
class VelocityVerletAlgorithm : public Relation<double, configuration::Part>
{
public:

    using SimulationTime = double;
    using BoundaryPart = configuration::Part;
    using Base = Relation<SimulationTime, BoundaryPart>;
    using PDPart = configuration::Part;

    /**
     * @brief Constructs the VelocityVerletAlgorithm Relation.
     * @param part The part to apply the time-integration on its nodes.
     */
    explicit VelocityVerletAlgorithm(PDPart& part);

private:

    SimulationTime p_dt;                // The time step.
    SimulationTime p_lastTimeStep;      // The last time step time.
    const SimulationTime& p_time;       // The reference to global Analyses::Time.

};

} // namespace rbs::relations::peridynamic::time_integration

#endif // VelocityVerletAlgorithm_H

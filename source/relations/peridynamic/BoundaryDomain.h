//
//  BoundaryDomain.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef BOUNDARYDOMAIN_H
#define BOUNDARYDOMAIN_H

#include "../Relation.h"
#include "../../Analyse.h"

namespace rbs::configuration {

class Part;
class Node;

}

/**
 * Provides the specified version of the RBS objects for the Peridynamic simulation.
 */
namespace rbs::relations::peridynamic {

/**
 * @brief Provides easy to use relation for implementing boundary condition on peridynamic simulations.
 */
class BoundaryDomain : public Relation<double, configuration::Part>
{
public:
    using SimulationTime = double;
    using BoundaryPart = configuration::Part;
    using Base = Relation<SimulationTime, BoundaryPart>;

    using PDNode = configuration::Node;

    /**
     * @brief The conditioner will apply on each time step to apply the condition to the Part nodes.
     * @param 1st[in] The time of the simulation.
     * @param 2nd[in, out] The node to apply the condition on it.
     */
    using Conditioner = std::function<void(const SimulationTime&, PDNode&)>;

    /**
     * @brief The constructor.
     * @param conditioner The function to apply the condition to the nodes.
     * @param part The part of the Relation, The nodes of this part will be sent to the conditioner at each time step.
     */
    explicit BoundaryDomain(const Conditioner& conditioner, BoundaryPart& part);

protected:

private:

    Conditioner p_conditioner;          // The function that apply the boundary condition to the Nodes of the Part.
    const SimulationTime& p_time;       // The reference to global Analyses::Time.
};

} // namespace rbs::relations::peridynamic

#endif // BOUNDARYDOMAIN_H

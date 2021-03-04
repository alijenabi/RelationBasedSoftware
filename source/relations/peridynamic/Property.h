//
//  Property.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef PROPERTY_H
#define PROPERTY_H

/**
 * Provides the specified version of the RBS objects for the Peridynamic simulation.
 */
namespace rbs::relations::peridynamic {

/**
 * @brief The Property of the peridynmaic Node, Neighborhood.
 * @warning If adding a new property to this list, following should be updated.
 *          1: rbs::relations::peridynamic::Exporter::to_string()
 *          2: rbs::relations::peridynamic::Exporter::to_attType()
 * @note The Neighborhood properties will be stored at the centre Node of the Neighborhood.
 * @note This enum is merely useful to avoid mistakes in writing the properties to the node, and using it is not mandatory.
 */
enum Property {
    InitialPosition,    // Denotes the initial position of the Node in PD
    CurrentPosition,    // Denotes the current position (i.e., initial postion + displacement) of the Node in PD
    Displacement,       // Denotes the displacement of the Node in PD
    Velocity,           // Denotes the velocity of the Node in PD
    Acceleration,       // Denotes the acceleration of the Node in PD
    Force,              // Denotes the force calculated from the equation of motion on the neighborhood centre.
    BodyForce,          // Denotes the body force of the Node in PD
    Density,            // Denotes the dencity on the colocation point at the centre fo the Node.
    Volume,             // Denotes the volume of the Node in PD
    Damage,             // Denotes the number of broken bond that the PDNode is connected to them initialy.
    MaximumStretch,     // Denotes the maximum bond stretch on the cetner of the horizon.
    WeightedVolume,     // Denotes the weighted volume defined by ordinary state-based peridynamic.
    Dilatation,         // Denotes the dilatation defined by ordinary state-based peridynamic.
};

} // namespace rbs::relations::peridynamic

#endif // PROPERTY_H

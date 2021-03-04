//
//  Rotation.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabi on 22.12.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef ROTATION_H
#define ROTATION_H

#include "../Relation.h"
#include "../../Analyse.h"
#include "../../configuration/Part.h"
#include "../../space/Vector.h"

/**
 *  The solid namestpase provides the means for moving Parts.
 */
namespace rbs::relations::solid {

/**
 * @brief The pair of rotation centre and rotation vector.
 */
using RotationData = std::pair< space::Point<3>, space::Vector<3> >;

/**
 * @brief The Relation applying solid rotation to a Part.
 */
class Rotation: public Relation<std::pair< space::Point<3>, space::Vector<3> >, configuration::Part>
{
public:
    using Part = configuration::Part;
    using Angle = double;
    using Vector = space::Vector<3>;
    using Point = space::Point<3>;
    using Base = Relation<RotationData, Part>;
    using Relationship = std::function<void(const RotationData&, Part&)>;

    /**
     * @brief The constructor.
     * @param centre The rotation centre.
     * @param vector The vector of rotation.
     * @param part The rotating part.
     * @param rotation The rotation relationship (function).
     */
    explicit Rotation(const Point& centre, const Vector& vector, Part& part, Relationship rotation);

    /**
     * @brief Gives a solid rotation relation around the centre of part local coordinate system.
     * @param speed The angular speed.
     * @param vector The vector of rotation.
     * @param part The part to rotate.
     * @return The relation.
     */
    static Rotation ConstantSpeed(const Angle speed, const Vector& vector, Part& part);

protected:

    const RotationData p_data;

};

} // namespace rbs::relations::solid

#endif // ROTATION_H

//
//  Translation.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabi on 22.12.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef TRANSLATION_H
#define TRANSLATION_H


#include "../Relation.h"
#include "../../Analyse.h"
#include "../../configuration/Part.h"
#include "../../space/Vector.h"

/**
 *  The solid namestpase provides the means for moving Parts.
 */
namespace rbs::relations::solid {

/**
 * @brief The Relation applying solid move to a Part.
 */
class Translation: public Relation<space::Vector<3>, configuration::Part>
{
public:
    using Part = configuration::Part;
    using Vector = space::Vector<3>;
    using Base = Relation<Vector, Part>;
    using Relationship = std::function<void(const Vector&, Part&)>;

    /**
     * @brief The constructor.
     * @param vector The move vector.
     * @param part The moving part.
     * @param translation The translation relationship (function).
     */
    explicit Translation(const Vector& vector, Part& part, Relationship translation);

    /**
     * @brief Gives a solid translation relation for moving the part with constant acceleration.
     */
    static Translation ConstantAcceleration(const Vector& acceleration, Part& part);

    /**
     * @brief Gives a solid translation relation for moving the part with constant velocity.
     */
    static Translation ConstantVelocity(const Vector& velocity, Part& part);

    /**
     * @brief Gives a solid translation relation for moving the part with constant acceleration with initial velocity.
     */
    static Translation ConstantVelocityWithInitialVelocity(const Vector& acceleration, const Vector& velocity, Part& part);

    /**
     * @brief Gives a solid translation relation for moving the part with linear acceleration.
     */
    static Translation LinearAcceleration(const Vector& acceleration, Part& part);

    /**
     * @brief Gives a solid translation relation for moving the part with linear acceleration and initial acceleration.
     */
    static Translation LinearAccelerationWithInitialAcceleration(const Vector& acceleration, const Vector& initialAcceleration, Part& part);

    /**
     * @brief Gives a solid translation relation for moving the part with linear acceleration and initial acceleration.
     */
    static Translation LinearAccelerationWithInitialAccelerationAndInitialVelocity(const Vector& acceleration, const Vector& initialAcceleration, const Vector& velocity, Part& part);

};

} // namespace rbs::relations::solid

#endif // TRANSLATION_H

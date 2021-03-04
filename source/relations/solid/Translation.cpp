//
//  Translation.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabi on 22.12.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Translation.h"

namespace rbs::relations::solid {

Translation::Translation(const Translation::Vector &vector, Translation::Part &part, Translation::Relationship translation)
    : Base(vector, part, translation)
{}

Translation Translation::ConstantAcceleration(const Translation::Vector &acceleration, Translation::Part &part) {
    const auto initialOrigin = part.local().origin();
    return Translation(acceleration, part, [initialOrigin](const Vector& vector, Part& part) {
        const auto time = Analyse::current().time();
        part.local().origin().positionVector() = vector * pow(time, 2) / 2 + initialOrigin.positionVector();
    });
}

Translation Translation::ConstantVelocity(const Translation::Vector &velocity, Translation::Part &part) {
    const auto initialOrigin = part.local().origin();
    return Translation(velocity, part, [initialOrigin](const Vector& vector, Part& part) {
        const auto time = Analyse::current().time();
        part.local().origin().positionVector() = vector * time + initialOrigin.positionVector();
    });
}

Translation Translation::ConstantVelocityWithInitialVelocity(const Translation::Vector &acceleration, const Translation::Vector &velocity, Translation::Part &part) {
    const auto initialOrigin = part.local().origin();
    return Translation(acceleration, part, [initialOrigin, velocity](const Vector& vector, Part& part) {
        const auto time = Analyse::current().time();
        part.local().origin().positionVector() = vector * pow(time, 2) / 2 + velocity * time + initialOrigin.positionVector();
    });
}

Translation Translation::LinearAcceleration(const Translation::Vector &acceleration, Translation::Part &part) {
    const auto initialOrigin = part.local().origin();
    return Translation(acceleration, part, [initialOrigin](const Vector& vector, Part& part) {
        const auto time = Analyse::current().time();
        part.local().origin().positionVector() = vector * pow(time, 2) / 4 + initialOrigin.positionVector();
    });
}

Translation Translation::LinearAccelerationWithInitialAcceleration(const Translation::Vector &acceleration, const Translation::Vector &initialAcceleration, Translation::Part &part) {
    const auto initialOrigin = part.local().origin();
    return Translation(acceleration, part, [initialOrigin, initialAcceleration](const Vector& vector, Part& part) {
        const auto time = Analyse::current().time();
        part.local().origin().positionVector() = (vector + 2 * initialAcceleration) * pow(time, 2) / 4 + initialOrigin.positionVector();
    });
}

Translation Translation::LinearAccelerationWithInitialAccelerationAndInitialVelocity(const Translation::Vector &acceleration, const Translation::Vector &initialAcceleration, const Translation::Vector &velocity, Translation::Part &part) {
    const auto initialOrigin = part.local().origin();
    return Translation(acceleration, part, [initialOrigin, initialAcceleration, velocity](const Vector& vector, Part& part) {
        const auto time = Analyse::current().time();
        part.local().origin().positionVector() = (vector + 2 * initialAcceleration) * pow(time, 2) / 4 + velocity * time + initialOrigin.positionVector();
    });
}

} // namespace rbs::relations::solid

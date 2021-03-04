//
//  Relation.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabi on 22.12.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Rotation.h"

namespace rbs::relations::solid {

Rotation::Rotation(const Rotation::Point &centre, const Rotation::Vector &vector, Rotation::Part &part, Rotation::Relationship rotation)
    : Base({centre, vector}, part, rotation)
    , p_data{centre, vector}
{
    setFeeder(p_data);
}

Rotation Rotation::ConstantSpeed(const Rotation::Angle speed, const Rotation::Vector &vector, Rotation::Part &part) {
    const auto i = part.local().axesDirectionVector(0);
    const auto j = part.local().axesDirectionVector(1);
    const auto k = part.local().axesDirectionVector(2);
    return Rotation({0, 0, 0}, vector, part, [speed, i, j, k](const RotationData& pair, Part& part){
        const auto time = Analyse::current().time();
        part.local().axesDirectionVector(0) = Vector::rotate(i, pair.second, speed * time);
        part.local().axesDirectionVector(1) = Vector::rotate(j, pair.second, speed * time);
        part.local().axesDirectionVector(2) = Vector::rotate(k, pair.second, speed * time);
    });
}

} // namespace rbs::relations::solid

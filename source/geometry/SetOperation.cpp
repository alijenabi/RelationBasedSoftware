//
//  SetOperation.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 24.05.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "SetOperation.h"

/**
 * The geometry namespace provides the means to handle geometrical shapes in 3D space space.
 */
namespace rbs::geometry {

std::ostream &operator <<(std::ostream &out, const SetOperation &status) {
    switch (status) {
    case Union:
        out << "Union";
        break;
    case Intersection:
        out << "Intersection";
        break;
    case Difference:
        out << "Difference";
        break;
    case SymmetricDifference:
        out << "SymmetricDifference";
        break;
    default:
        throw std::runtime_error("The SetOperation is not recognized.");
    }
    return out;
}

} // namespace rbs::geometry

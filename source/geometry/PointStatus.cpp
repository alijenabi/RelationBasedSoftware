//
//  PointStatus.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 12.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "PointStatus.h"


/**
 * The geometry namespace provides the means to handle geometrical shapes in 3D space space.
 */
namespace rbs::geometry {

std::ostream &operator <<(std::ostream &out, const PointStatus &status) {
    switch (status) {
    case Inside:
        out << "Inside";
        break;
    case On:
        out << "On";
        break;
    case Outside:
        out << "Outside";
        break;
    default:
        throw std::runtime_error("The PointStatus is not recognized.");
    }
    return out;
}

} // namespace rbs::geometry

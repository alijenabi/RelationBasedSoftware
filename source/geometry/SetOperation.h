//
//  SetOperation.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 24.05.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef SETOPERATION_H
#define SETOPERATION_H

#include <ostream>

/**
 * The geometry namespace provides the means to handle geometrical shapes in 3D space space.
 */
namespace rbs::geometry {

/**
 * @brief The posible set operations.
 */
enum SetOperation{
    Union,
    Intersection,
    Difference,
    SymmetricDifference
};

std::ostream & operator << (std::ostream & out, const SetOperation & status);

} // namespace rbs::geometry

#endif // SETOPERATION_H

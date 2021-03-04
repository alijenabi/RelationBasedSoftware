//
//  PointStatus.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 12.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef POINTSTATUS_H
#define POINTSTATUS_H

#include <ostream>

/**
 * The geometry namespace provides the means to handle geometrical shapes in 3D space space.
 */
namespace rbs::geometry {

/**
 * @brief The status of the point when compared to any geometry.
 */
enum PointStatus {
    Inside,
    On,
    Outside
};

std::ostream & operator << (std::ostream & out, const PointStatus & status);

} // namespace rbs::geometry

#endif // POINTSTATUS_H

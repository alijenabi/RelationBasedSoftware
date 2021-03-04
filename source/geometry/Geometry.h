//
//  Geometry.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 24.05.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "PointStatus.h"
#include "../space/Point.h"
#include <vector>
#include <set>

/**
 * The geometry namespace provides the means to handle geometrical shapes in 3D space space.
 */
namespace rbs::geometry {

/**
 * @brief Any geometry requires to provide particular functionality in order to be able
 *        to use them inside the numeric simulator. This abstract class ensures the existence
 *        of those functionalities.
 */
class Geometry {
public:

    using Point3D = space::Point<3>;
    using PointPtr = std::shared_ptr<Point3D>;
    using Vector3D = space::Vector<3>;
    using Ray = std::pair<Point3D, Vector3D>;
    using Rays = std::vector<Ray>;

    virtual ~Geometry(){}

    /**
     * @brief Computes the distance between the geometry and a point in 3D space.
     * @note The distance is negative if the point is inside the shape, and it is in its minimum at the centre of the shape.
     * @param point The point in 3D space.
     */
    virtual double distanceTo(const Point3D& point) const = 0;

    /**
     * @brief Computes whether the geometry is equivalent to an empty space or not.
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Projects a point on the surface of the Geometry.
     * @param point The point to project.
     * @return The projected point on the sruface of the Geometry.
     */
    virtual PointPtr pointProjection(const Point3D& point) const = 0;

    /**
     * @brief Computes the status of the point in comparison with the Primary geometry.
     * @param point The point.
     */
    virtual PointStatus pointStatus(const Point3D & point) const = 0;

    /**
     * @brief Computes all the vertices (corners) of the geometry.
     */
    virtual std::vector<Point3D> vertices() const = 0;

    /**
     * @brief Moves the geometry.
     * @param vector The transfer vector.
     */
    virtual void move(const Vector3D& vector) = 0;

    /**
     * @brief Mirrors the geometry.
     * @param planePosition The mirror plane position vector.
     * @param planeNormal The mirror plane normal vector.
     */
    virtual void mirror(const Point3D& planePosition, const Vector3D& planeNormal) = 0;

    /**
     * @brief Rotates the geometry.
     * @param rotationCentre The centre of rotation.
     * @param rotationAxis The rotation axis' direction vector (dose not need to be unit vector).
     * @param angle The angle of rotation (rad) using rigth hand role.
     * @note Right hand thummb in direction of the rotation axis posetive angle is in direction of closing right hand.
     */
    virtual void rotate(const Point3D& rotationCentre, const Vector3D& rotationAxis, double angle) = 0;

    /**
     * @brief The operator std::string.
     */
    virtual operator std::string() const = 0;

};

} // namespace rbs::geometry

#endif // GEOMETRY_H

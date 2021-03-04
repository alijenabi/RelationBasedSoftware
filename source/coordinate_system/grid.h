//
//  grid.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 07.02.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef GRID_H
#define GRID_H

#include <set>

/**
 * The space namespace provides the means to handle matters related to the mathematical space.
 */
namespace rbs::space {

template<unsigned char SpaceDimension> class Index;
template<unsigned char SpaceDimension> class Point;
template<unsigned char SpaceDimension> class Vector;

} // namespace rbs::space

namespace rbs::coordinate_system {

class Axis;

} // namespace rbs::coordinate_system

/**
 * The coordinate_system namespace provides the means to work with different coordinate systems.
 * The grid namespace provides an easy way to create grids on coordinate systems.
 */
namespace rbs::coordinate_system::grid {

using Value = double;
using Values = std::set<Value>;

using Point = space::Point<3>;
using Vector = space::Vector<3>;

/**
 * @brief Converts a degree angle to radian.
 * @param deg The angle in degree.
 */
Value toRad(Value deg);

/**
 * @brief Converts a radian angle to degree.
 * @param rad The angle in radian.
 */
Value toDeg(Value rad);

/**
 * The generators namespace provides basic functions to generate grids.
 */
namespace generators {

using Index = size_t;

/**
 * @brief Meshes the axis in one direction.
 * @note Generates at least one grid.
 * @note If the end smaller than the start, then they will be switched.
 * @param start The start value for the grid mesh.
 * @param end The end value for the grid mesh.
 * @param distance The distance between the grids.
 * @throws If the distance is zero or negative.
 */
Values meshByLimit(Value start, Value end, Value distance);

/**
 * @brief Meshes the axis in one direction.
 * @note Generates at least one grid.
 * @note The distance vector can have negative components.
 * @param start The start value for the grid mesh.
 * @param distance The distance between the grids. (negative value: negative direction of the axis)
 * @param numberOfGrids The number of grids (i.e., meshes + 1).
 */
Values meshByNumber(Value start, Value distance, Index numberOfGrids);

/**
 * @brief Meshes the axis in two direction having the grid at the centre.
 * @note Generates at least one grid.
 * @param centre The centre (i.e., staring point)
 * @param distance The distance between the grids.
 * @param radius The radius of meshing.
 * @throws If the distance is zero or negative.
 * @throws If the radius is zero or negative.
 */
Values meshWithCentreedEdge(Value centre, Value distance, Value radius);

/**
 * @brief Meshes the axis in two directions having NO grid at the centre.
 * @note The first grids will be half distance away from the centre in both directions.
 * @param centre The centre (i.e., staring point)
 * @param distance The distance between the grids.
 * @param radius The radius of meshing.
 * @throws If the distance is zero or negative.
 * @throws If the radius is zero or negative.
 */
Values meshWithCentreedElement(Value centre, Value distance, Value radius);

} // namespace generators

namespace cartesian {

using Index = space::Index<3>;

/**
 * @brief Generates a uniform grid mesh within a specific range and a fixed mesh size.
 * @note The start and end are the values of the grids on the axes.
 * @note All the component of the start point may or maynot be smaller than end point.
 * @note Generates at least one grid in each direction.
 * @param start The start point for the grid mesh.
 * @param end The end point for the grid mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param[out] axes The axis of cartesian coordinate system (i.e., {x, y, z}).
 * @throws If any of the distance vector components is zero or negative.
 */
void uniformDirectional(const Point& start, const Point& end, const Vector& distanceVector, Axis* axes);

/**
 * @brief Generates a uniform grid mesh from a point by moving fixed steps in a direction.
 * @note This might not produce any grids on the axes if the numberOfGrids has zero conponent in those axes direction.
 * @param start The start point for the grid mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param numberOfGrids The number of grids (i.e., meshes + 1) in each axis direction.
 * @param[out] axes The axis of cartesian coordinate system (i.e., {x, y, z}).
 */
void uniformDirectional(const Point& start, const Vector& distanceVector, const Index& numberOfGrids, Axis* axes);

/**
 * @brief Generates a uniform grid mesh around the origin grid.
 * @note Always generates the origin grid.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param radiusVector The radius of meshing on each axis direction.
 * @param[out] axes The axis of cartesian coordinate system (i.e., {x, y, z}).
 * @throws If one of the distance vector components is zero or negative.
 * @throws If one of the radius vector components is zero or negative.
 */
void uniformOnOrigin(const Vector& distanceVector, const Vector& radiusVector, Axis* axes);

/**
 * @brief Generates a uniform grid mesh around a centre point.
 * @note Generates at least one grid on each axis to present the centre.
 * @param centre The centre point.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param radiusVector The radius of meshing on each axis direction.
 * @param[out] axes The axis of cartesian coordinate system (i.e., {x, y, z}).
 * @throws If one of the distance vector components is zero or negative.
 * @throws If one of the radius vector components is zero or negative.
 */
void uniformOnCentrePoint(const Point& centre, const Vector& distanceVector, const Vector& radiusVector, Axis* axes);

/**
 * @brief Generates a uniform grid mesh around the origin grid without including the origin itself.
 * @note The first grids will be half distance away from the origin in each direction.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param radiusVector The radius of meshing on each axis direction.
 * @param[out] axes The axis of cartesian coordinate system (i.e., {x, y, z}).
 * @throws If one of the distance vector components is zero or negative.
 * @throws If one of the radius vector components is zero or negative.
 */
void uniformAroundOrigin(const Vector& distanceVector, const Vector& radiusVector, Axis* axes);

/**
 * @brief Generates a uniform grid mesh around a centre point without including the point itself.
 * @note The first grids will be half distance away from the point in each direction.
 * @param centre The centre point.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param radiusVector The radius of meshing on each axis direction.
 * @param[out] axes The axis of cartesian coordinate system (i.e., {x, y, z}).
 * @throws If one of the distance vector components is zero or negative.
 * @throws If one of the radius vector components is zero or negative.
 */
void uniformAroundCentrePoint(const Point& centre, const Vector& distanceVector, const Vector& radiusVector, Axis* axes);

} // namespace cartesian

namespace cylindrical {

using Index = space::Index<3>;

/**
 * @brief Brings the point's azimuthal angle to the 0 to 2 * PI range.
 */
Point inRange(const Point& point);

/**
 * @brief Checks whther the azimuthal component of the distanceVector is within 0 to 2 * PI range.
 * @param distanceVector The distance between the grids in each axis direction.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 */
void checkDistanceVector(const Vector& distanceVector);

/**
 * @brief Checks whther the azimuthal component of the radiusVector is within 0 to 2 * PI range.
 * @param radiusVector The radius of meshing on each axis direction.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 */
void checkRadiusVector(const Vector& radiusVector);

/**
 * @brief Generates a uniform grid mesh within a specific range and a fixed mesh size.
 * @note The grids will be generated only within 0 to 2 * PI azimuthal angle.
 * @note The start and end are the values of the grids on the axes.
 * @note All the component of the start point may or maynot be smaller than end point.
 * @note Generates at least one grid in each direction.
 * @param start The start point for the grid mesh.
 * @param end The end point for the grid mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param[out] axes The axis of cylindrical coordinate system (i.e., {r, azimuthal angle, z}).
 * @throws If any of the distance vector components is zero or negative.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 */
void uniformDirectional(const Point& start, const Point& end, const Vector& distanceVector, Axis* axes);

/**
 * @brief Generates a uniform grid mesh from a point by moving fixed steps in a direction.
 * @note The grids will be generated only within 0 to 2 * PI azimuthal angle.
 * @note This might not produce any grids on the axes if the numberOfGrids has zero conponent in those axes direction.
 * @param start The start point for the grid mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param numberOfGrids The number of grids (i.e., meshes + 1) in each axis direction.
 * @param[out] axes The axis of cylindrical coordinate system (i.e., {r, azimuthal angle, z}).
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 */
void uniformDirectional(const Point& start, const Vector& distanceVector, const Index& numberOfGrids, Axis* axes);

/**
 * @brief Generates a uniform grid mesh around the origin grid.
 * @note Always generates the origin grid.
 * @note The bottom and top height of the mesh are interchangable.
 * @param radius The radius of the mesh.
 * @param bottomHeight The lowest height of the mesh.
 * @param topHeight The topest height of the mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param[out] axes The axis of cylindrical coordinate system (i.e., {r, azimuthal angle, z}).
 * @throws If one of the distance vector components is zero or negative.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 */
void uniformCylinder(Value radius, Value bottomHeight, Value topHeight, const Vector& distanceVector, Axis* axes);

/**
 * @brief Generates a uniform grid mesh around the origin grid.
 * @note The inner and radius of the mesh are interchangable.
 * @note The bottom and top height of the mesh are interchangable.
 * @param innerRadius The inner radius of the mesh.
 * @param outerRadius The outer radius of the mesh.
 * @param bottomHeight The lowest height of the mesh.
 * @param topHeight The topest height of the mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param[out] axes The axis of cylindrical coordinate system (i.e., {r, azimuthal angle, z}).
 * @throws If one of the distance vector components is zero or negative.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 */
void uniformRing(Value innerRadius, Value outerRadius, Value bottomHeight, Value topHeight, const Vector& distanceVector, Axis* axes);

} // namespace cylindrical

namespace spherical {

using Index = space::Index<3>;

/**
 * @brief Brings the point's azimuthal angle to the 0 to 2 * PI range and polar angle to 0 to PI range.
 */
Point inRange(const Point& point);

/**
 * @brief Checks whther the azimuthal and polar components of the distanceVector are in range (i.e., 0 to 2 * PI and 0 to PI respectively).
 * @param distanceVector The distance between the grids in each axis direction.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 * @throws If the distance vector polar components is bigger than PI.
 */
void checkDistanceVector(const Vector& distanceVector);

/**
 * @brief Checks whther the azimuthal and polar components of the radiusVector are in range (i.e., 0 to PI).
 * @param radiusVector The radius of meshing on each axis direction.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 * @throws If the distance vector polar components is bigger than PI.
 */
void checkRadiusVector(const Vector& radiusVector);

/**
 * @brief Generates a uniform grid mesh within a specific range and a fixed mesh size.
 * @note The grids will be generated only within 0 to 2 * PI azimuthal angle and 0 to PI polar angle.
 * @note The start and end are the values of the grids on the axes.
 * @note All the component of the start point may or maynot be smaller than end point.
 * @note Generates at least one grid in each direction.
 * @param start The start point for the grid mesh.
 * @param end The end point for the grid mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param[out] axes The axis of spherical coordinate system (i.e., {r, azimuthal angle, polar angle}).
 * @throws If any of the distance vector components is zero or negative.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 * @throws If the distance vector polar components is bigger than PI.
 */
void uniformDirectional(const Point& start, const Point& end, const Vector& distanceVector, Axis* axes);

/**
 * @brief Generates a uniform grid mesh from a point by moving fixed steps in a direction.
 * @note The grids will be generated only within 0 to 2 * PI azimuthal angle and 0 to PI polar angle.
 * @note This might not produce any grids on the axes if the numberOfGrids has zero conponent in those axes direction.
 * @param start The start point for the grid mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param numberOfGrids The number of grids (i.e., meshes + 1) in each axis direction.
 * @param[out] axes The axis of spherical coordinate system (i.e., {r, azimuthal angle, polar angle}).
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 * @throws If the distance vector polar components is bigger than PI.
 */
void uniformDirectional(const Point& start, const Vector& distanceVector, const Index& numberOfGrids, Axis* axes);

/**
 * @brief Generates a uniform grid mesh around the origin grid.
 * @note Always generates the origin grid.
 * @param radius The radius of the mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param[out] axes The axis of cylindrical coordinate system (i.e., {r, azimuthal angle, z}).
 * @throws If one of the distance vector components is zero or negative.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 */
void uniformSphere(Value radius, const Vector& distanceVector, Axis* axes);

/**
 * @brief Generates a uniform grid mesh around the origin grid.
 * @note The inner and radius of the mesh are interchangable.
 * @param innerRadius The inner radius of the mesh.
 * @param outerRadius The outer radius of the mesh.
 * @param distanceVector The distance between the grids in each axis direction.
 * @param[out] axes The axis of cylindrical coordinate system (i.e., {r, azimuthal angle, z}).
 * @throws If one of the distance vector components is zero or negative.
 * @throws If the distance vector azimuthal components is bigger than 2 * PI.
 */
void uniformBall(Value innerRadius, Value outerRadius, const Vector& distanceVector, Axis* axes);

} // namespace spherical

} // namespace rbs::coordinate_system::grid

#endif // GRID_H

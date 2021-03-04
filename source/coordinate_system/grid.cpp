//
//  grid.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 07.02.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "grid.h"
#include "Axis.h"
#include "../space/Index.h"
#include "../space/Point.h"

namespace rbs::coordinate_system::grid {

Value toRad(const Value deg) {
    return deg / 180 * M_PI;
}

Value toDeg(const Value rad) {
    return rad / M_PI * 180;
}

Values generators::meshByLimit(const coordinate_system::grid::Value start, const coordinate_system::grid::Value end, const coordinate_system::grid::Value distance){
    if (space::isZero(distance) || distance < 0)
        throw  std::logic_error("The distance between the grid should be positive.");
    if (end < start)
        return meshByLimit(end, start, distance);
    Values values;
    for (Value itr = start; itr < end || space::areEqual(itr, start) || space::areEqual(itr, end); itr += distance)
        values.insert(itr);
    return values;
}

Values generators::meshByNumber(const Value start, const Value distance, const Index numberOfGrids) {
    Values values;
    for (Index itr = 0; itr < numberOfGrids; itr ++)
        values.insert(start + itr * distance);
    return values;
}

Values generators::meshWithCentreedEdge(const Value centre, const Value distance, const Value radius) {
    if (space::isZero(radius) || radius < 0)
        throw  std::logic_error("The meshing radius should be positive.");

    Values values = meshByLimit(centre, centre + radius, distance);
    if (values.size() - 1 > 0) {
        const auto secondHalf = meshByNumber(centre - distance, -distance, values.size() - 1);
        values.insert(secondHalf.begin(), secondHalf.end());
    }
    return values;
}

Values generators::meshWithCentreedElement(const Value centre, const Value distance, const Value radius) {
    if (space::isZero(radius) || radius < 0)
        throw  std::logic_error("The meshing radius should be positive.");
    if (space::isZero(distance) || distance < 0)
        throw  std::logic_error("The meshing radius should be positive.");

    Values values;
    const auto halfDistance = distance / 2;
    const int numberOfGrids = static_cast<Index>((radius - halfDistance) / distance) + 1;
    if (numberOfGrids > 0 && radius >= halfDistance) {
        values = meshByNumber(centre + halfDistance, distance, numberOfGrids);
        const auto secondHalf = meshByNumber(centre - halfDistance, -distance, numberOfGrids);
        values.insert(secondHalf.begin(), secondHalf.end());
    }
    return values;
}

void cartesian::uniformDirectional(const Point &start, const Point &end, const Vector &distanceVector, Axis *axes){
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshByLimit(start.positionVector()[itr], end.positionVector()[itr], distanceVector[itr]);
}

void cartesian::uniformDirectional(const Point &start, const Vector &distanceVector, const cartesian::Index &numberOfGrids, Axis *axes){
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshByNumber(start.positionVector()[itr], distanceVector[itr], numberOfGrids[itr]);
}

void cartesian::uniformOnOrigin(const Vector &distanceVector, const Vector &radiusVector, Axis *axes){
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshWithCentreedEdge(0, distanceVector[itr], radiusVector[itr]);
}

void cartesian::uniformOnCentrePoint(const Point &centre, const Vector &distanceVector, const Vector &radiusVector, Axis *axes){
    const auto& posVec = centre.positionVector();
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshWithCentreedEdge(posVec[itr], distanceVector[itr], radiusVector[itr]);

}

void cartesian::uniformAroundOrigin(const Vector &distanceVector, const Vector &radiusVector, Axis *axes){
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshWithCentreedElement(0, distanceVector[itr], radiusVector[itr]);
}

void cartesian::uniformAroundCentrePoint(const Point &centre, const Vector &distanceVector, const Vector &radiusVector, Axis *axes){
    const auto& posVec = centre.positionVector();
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshWithCentreedElement(posVec[itr], distanceVector[itr], radiusVector[itr]);
}

Point cylindrical::inRange(const Point &point) {
    Vector ans = point.positionVector();
    if (ans[1] > 2 * M_PI)
        ans[1] -= static_cast<int>(ans[1] / (2 * M_PI)) * 2 * M_PI;
    if (ans[1] < 2 * M_PI)
        ans[1] += (static_cast<int>(ans[1] / (2 * M_PI))) * 2 * M_PI;
    return ans;
}

void cylindrical::checkDistanceVector(const Vector &distanceVector) {
    if (distanceVector[1] > 2 * M_PI)
        throw  std::logic_error("The distance vector's azimuthal angle should be smaler than 2 * PI.");
}

void cylindrical::checkRadiusVector(const Vector &radiusVector) {
    if (radiusVector[1] > 2 * M_PI)
        throw  std::logic_error("The radius vector's azimuthal angle should be smaler than PI.");
}

void cylindrical::uniformDirectional(const Point &start, const Point &end, const Vector &distanceVector, Axis *axes){
    checkDistanceVector(distanceVector);
    const auto sVec = inRange(start).positionVector();
    const auto eVec = inRange(end).positionVector();
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshByLimit(sVec[itr], eVec[itr], distanceVector[itr]);
}

void cylindrical::uniformDirectional(const Point &start, const Vector &distanceVector, const cylindrical::Index &numberOfGrids, Axis *axes){
    checkDistanceVector(distanceVector);
    const auto sVec = inRange(start).positionVector();
    const auto maxAzimuthalGrid =  static_cast<size_t>((2 * M_PI - sVec[1]) / distanceVector[1]);
    const auto gridNum = Index{numberOfGrids[0], std::min(numberOfGrids[1], maxAzimuthalGrid), numberOfGrids[2]};
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshByNumber(sVec[itr], distanceVector[itr], gridNum[itr]);
}

void cylindrical::uniformCylinder(const Value radius, const Value bottomHeight, const Value topHeight, const Vector& distanceVector, Axis* axes){
    uniformRing(0, radius, bottomHeight, topHeight, distanceVector, axes);
}

void cylindrical::uniformRing(const Value innerRadius, const Value outerRadius, const Value bottomHeight, const Value topHeight, const Vector& distanceVector, Axis* axes)
{
    uniformDirectional(Point{innerRadius, 0, bottomHeight }, Point{outerRadius, 2 * M_PI - distanceVector[1] / 4, topHeight }, distanceVector, axes);
}

Point spherical::inRange(const Point &point) {
    Vector ans = point.positionVector();

    // Correcting polar may change the azimuthal angle. Thus it comes first.
    if (ans[2] < 0) {
        ans[2] = -ans[2];
        ans[1] += M_PI;
    }
    if (M_PI < ans[2]) {
        if (2 * M_PI < ans[2]) {
            ans[1] -= static_cast<int>(ans[1] / (2 * M_PI)) * 2 * M_PI;
        }
        if (M_PI < ans[2] && ans[2] < 2 * M_PI) {
            ans[2] = 2 * M_PI - ans[2];
            ans[1] += M_PI;
        }
    }
    // Correcting azimuthal angle.
    if (2 * M_PI < ans[1])
        ans[1] -= static_cast<int>(ans[1] / (2 * M_PI)) * 2 * M_PI;
    if (ans[1] < 2 * M_PI)
        ans[1] += (static_cast<int>(ans[1] / (2 * M_PI))) * 2 * M_PI;

    return ans;
}

void spherical::checkDistanceVector(const Vector &distanceVector) {
    if (distanceVector[1] > 2 * M_PI)
        throw  std::logic_error("The distance vector's azimuthal angle should be smaler than 2 * PI.");
    if (distanceVector[2] > M_PI)
        throw  std::logic_error("The distance vector's polar angle should be smaler than PI.");
}

void spherical::checkRadiusVector(const Vector &radiusVector) {
    if (radiusVector[1] > 2 * M_PI)
        throw  std::logic_error("The radius vector's azimuthal angle should be smaler than PI.");
    if (radiusVector[2] > M_PI)
        throw  std::logic_error("The radius vector's polar angle should be smaler than PI.");
}

void spherical::uniformDirectional(const Point &start, const Point &end, const Vector &distanceVector, Axis *axes){
    checkDistanceVector(distanceVector);
    const auto sVec = inRange(start).positionVector();
    const auto eVec = inRange(end).positionVector();
    for (size_t itr = 0; itr < 3; itr++)
        axes[itr] = generators::meshByLimit(sVec[itr], eVec[itr], distanceVector[itr]);
}

void spherical::uniformDirectional(const Point &start, const Vector &distanceVector, const spherical::Index &numberOfGrids, Axis *axes){
    uniformDirectional(start, start + Vector{distanceVector[0] * numberOfGrids[0], distanceVector[1] * numberOfGrids[1], distanceVector[2] * numberOfGrids[2]}, distanceVector, axes);
}

void spherical::uniformSphere(const Value radius, const Vector &distanceVector, Axis *axes)
{
    uniformBall(0, radius, distanceVector, axes);
}

void spherical::uniformBall(const Value innerRadius, const Value outerRadius, const Vector &distanceVector, Axis *axes)
{
    uniformDirectional(Point{innerRadius, 0, distanceVector[2] / 2},
                       Point{outerRadius, 2 * M_PI - distanceVector[1] / 4, M_PI - distanceVector[2] / 4},
                       distanceVector,
                       axes);
}

} // namespace rbs::coordinate_system::grid


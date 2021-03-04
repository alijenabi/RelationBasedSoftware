//
//  convertors.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 01.02.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef CONVERTORS_H
#define CONVERTORS_H

#include "../space/Point.h"
#include <functional>

/**
 * The coordinate_system namespace provides the means to work with different coordinate systems.
 * The convertors namespace provides the forward and backward mapping for known coordinate systems (i.e. cartesian, cylindrical, and spherical).
 */
namespace rbs::coordinate_system::convertors {

using Point = space::Point<3>;
using Vector = space::Vector<3>;

/**
 * @brief Converting forward and backward between the coordinte systems requres mapping between two spaces (i.e. 3D vector stapce) regardless of the direction.
 * @param 1st The vector in parent/local coordinate system (in forward/backward mapping).
 * @param 2nd The origin of the local coordinate system in the parent coordinate system.
 * @param 3rd The i-vector of the local coordinate system in the parent coordinate system.
 * @param 4th The j-vector of the local coordinate system in the parent coordinate system.
 * @param 5th The k-vector of the local coordinate system in the parent coordinate system.
 * @return The vector in local/parent coordinate system (in forward/backward mapping).
 */
using Mapping = std::function<Point(const Point&, const Point&, const Vector&, const Vector&, const Vector&)>;

namespace cartesian {

/**
 * @brief Gives a forward mapping for converting cartesian parent points to cartesian child points.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cartesian).
 */
Mapping toCartesian();

/**
 * @brief Gives a backward mapping for converting cartesian child points to cartesian parrent points.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cartesian).
 */
Mapping toCartesianInverse();

/**
 * @brief Gives a forward mapping for converting cartesian parent points to cylindrical child points.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cartesian).
 */
Mapping toCylindrical();

/**
 * @brief Gives a backward mapping for converting cylindrical child points to cartesian parrent points.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cartesian).
 */
Mapping toCylindricalInverse();

/**
 * @brief Gives a forward mapping for converting cartesian parent points to spherical child points.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cartesian).
 */
Mapping toSpherical();

/**
 * @brief Gives a backward mapping for converting spherical child points to cartesian parrent points.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cartesian).
 */
Mapping toSphericalInverse();

} // namespace cylindrical

namespace cylindrical {

/**
 * @brief Computes the addition of two vectors in a cylindrical coordinate system.
 * @note The vectors have to have {r, theta, z} signature.
 * @note The addition of vectors in the cylindrical coordinate system will not achieve by adding the vectors' components.
 * @param v1 The first vector.
 * @param v2 The seciond vector.
 * @return The sum of the v1 and v2 using prallelogram rule.
 */
Vector add(const Vector& v1, const Vector& v2);

/**
 * @brief Computes the inverse vector.
 * @note The vectors have to have {r, theta, z} signature.
 * @note The vectors inversing in the cylindrical coordinate system will not achieve by multiplying a minus to the vectors' components.
 * @param vector The vector.
 */
Vector inverse(const Vector vector);

/**
 * @brief Computes the vector multiplication of a vector to a constant in a cylindrical coordinate system.
 * @note The vectors have to have {r, theta, z} signature.
 * @param factor The factor
 * @param vector The vector
 */
Vector multiply(const double factor, const Vector& vector);

/**
 * @brief Computes the projection lenght of a vector (in the cylindrical coordinate system) on other vector (in the cylindrical coordinate system).
 * @note The vectors have to have {r, theta, z} signature.
 * @note Computes the dot product of the vectors by converting them to the cartesian coordinate system and then computing the dot product.
 * @param projectee The vector to project.
 * @param base The vector to project projectee on it.
 */
double projectionLength(const Vector& projectee, const Vector& base);

/**
 * @brief Computes the unit vector in a cylindrical coordinate system.
 * @throws If the lenght of the vector is zero.
 */
Vector unit(const Vector& vector);

/**
 * @brief Gives a forward mapping for converting cylindrical parent points to cartesian child points.
 * @note The vectors have to have {r, theta, z} signature.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cylindrical).
 */
Mapping toCartesian();

/**
 * @brief Gives a backward mapping for converting cartesian child points to cylindrical parrent points.
 * @note The vectors have to have {r, theta, z} signature.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cylindrical).
 */
Mapping toCartesianInverse();

/**
 * @brief Gives a forward mapping for converting cylindrical parent points to cylindrical child points.
 * @note The vectors have to have {r, theta, z} signature.
 * @note The origin, i, j, k are in parrent coordinate system.
 */
Mapping toCylindrical();

/**
 * @brief Gives a backward mapping for converting cylindrical child points to cylindrical parrent points.
 * @note The vectors have to have {r, theta, z} signature.
 * @note The origin, i, j, k are in parrent coordinate system.
 */
Mapping toCylindricalInverse();

/**
 * @brief Gives a forward mapping for converting cylindrical parent points to spherical child points.
 * @note The vectors have to have {r, theta, z} signature.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cylindrical).
 */
Mapping toSpherical();

/**
 * @brief Gives a backward mapping for converting spherical child points to cylindrical parrent points.
 * @note The vectors have to have {r, theta, z} signature.
 * @note The origin, i, j, k are in parrent coordinate system (i.e. cylindrical).
 */
Mapping toSphericalInverse();

} // namespace cylindrical

namespace spherical {

/**
 * @brief Computes the addition of two vectors in a spherical coordinate system.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note The addition of vectors in the spherical coordinate system will not achieve by adding the vectors' components.
 * @param v1 The first vector.
 * @param v2 The seciond vector.
 * @return The sum of the v1 and v2 using prallelogram rule.
 */
Vector add(const Vector& v1, const Vector& v2);

/**
 * @brief Computes the inverse vector.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note The vectors inversing in the spherical coordinate system will not achieve by multiplying a minus to the vectors' components.
 * @param vector The vector.
 */
Vector inverse(const Vector vector);

/**
 * @brief Computes the projection lenght of a vector (in the spherical coordinate system) on other vector (in the spherical coordinate system).
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note Computes the dot product of the vectors by converting them to the cartesian coordinate system and then computing the dot product.
 * @param projectee The vector to project.
 * @param base The vector to project projectee on it.
 */
double projectionLength(const Vector& projectee, const Vector& base);

/**
 * @brief Computes the vector multiplication of a vector to a constant in a spherical coordinate system.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @param factor The factor
 * @param vector The vector
 */
Vector multiply(const double factor, const Vector& vector);

/**
 * @brief Computes the unit vector in a cylindrical coordinate system.
 */
Vector unit(const Vector& vector);

/**
 * @brief Gives a forward mapping for converting spherical parent points to cartesian child points.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note The origin, i, j, k are in parrent coordinate system (i.e. spherical).
 */
Mapping toCartesian();

/**
 * @brief Gives a backward mapping for converting cartesian child points to spherical parrent points.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note The origin, i, j, k are in parrent coordinate system (i.e. spherical).
 */
Mapping toCartesianInverse();

/**
 * @brief Gives a forward mapping for converting spherical parent points to cylindrical child points.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note The origin, i, j, k are in parrent coordinate system (i.e. spherical).
 */
Mapping toCylindrical();

/**
 * @brief Gives a backward mapping for converting cylindrical child points to spherical parrent points.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note The origin, i, j, k are in parrent coordinate system (i.e. spherical).
 */
Mapping toCylindricalInverse();

/**
 * @brief Gives a forward mapping for converting spherical parent points to spherical child points.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note The origin, i, j, k are in parrent coordinate system.
 */
Mapping toSpherical();

/**
 * @brief Gives a backward mapping for converting spherical child points to spherical parrent points.
 * @note The vectors have to have {r, theta, phi} signature. (theta == azimuthal angle, phi == polar angle)
 * @note The origin, i, j, k are in parrent coordinate system.
 */
Mapping toSphericalInverse();

} // namespace spherical

} // namespace rbs::coordinate_system::convertors

#endif // CONVERTORS_H

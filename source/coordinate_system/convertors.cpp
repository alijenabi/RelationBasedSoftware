//
//  convertors.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 01.02.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "convertors.h"

namespace rbs::coordinate_system::convertors {

namespace cartesian {

Mapping toCartesian() {
    return [](const Point& point, const Point& origin, const Vector& i, const Vector& j, const Vector& k) {
        // Gives the component of the vector in child coordinate system (CS) by mirroring the vector in child CS on the base vector (i.e. the unitvector of the CS axes).
        const auto component = [](const Vector& v, const Vector& base){
            return (v * base >= 0)? v.projectionLenghtOn(base) : -v.projectionLenghtOn(base);
        };
        // The position vector of the point in child CS.
        const auto difVector = point.positionVector() - origin.positionVector();
        return Point{
            component(difVector, i),
            component(difVector, j),
            component(difVector, k)
        };
    };
}

Mapping toCartesianInverse() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        const auto& vector = point.positionVector(); // The position vector of the point in child coordinate system.
        // Adding the axis vector to the origin will give the point in parent coordinate system.
        return origin + vector[0] * i + vector[1] * j + vector[2] * k;
    };
}

Mapping toCylindrical() {
    return [](const Point& point, const Point& origin, const Vector& i, const Vector& j, const Vector& k) {
        const auto difVector = point.positionVector() - origin.positionVector();                                 // The cartesian position vector of the point in child CS (i.e., cylindrical CS).
        const auto z = (difVector * k >= 0)? difVector.projectionLenghtOn(k) : -difVector.projectionLenghtOn(k); // the z component of point in child CS.
        const auto projection = difVector - z * k;                                                               // projection of difVector on r-theta plane.
        const auto r = projection.length();                                                                      // The r component of point in child CS.
        const auto azimuthal = (projection * j >= 0)? Vector::angleBetween(projection, i) : 2 * M_PI - Vector::angleBetween(projection, i); // The theta component of point in child CS.

        return Point{r, azimuthal, z}; // The point in child CS.
    };
}

Mapping toCylindricalInverse() {
    return [](const Point& point, const Point& origin, const Vector& i, const Vector& j, const Vector& k) {
        const auto& vector = point.positionVector(); // The position vector of the point in child (cylindrical) coordinate system.
        // Adding the axis vector to the origin will give the point in parent coordinate system.
        return origin + (Vector::rotate(vector[0] * i, Vector::crossProduct(i, j), vector[1]) + vector[2] * k);
    };
}

Mapping toSpherical() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        const auto difVector = point.positionVector() - origin.positionVector();                          // The cartesian position vector of the point in child CS (i.e., spherical CS).
        const auto r = difVector.length();                                                                // The r component of point in child CS.
        const auto polar = Vector::angleBetween(difVector, k);                                            // The \phi component of point in child CS.
        const auto projection = Vector{difVector[0], difVector[1], 0};                                    // projection of difVector on r-theta plane.
        const auto azimuthal = (projection * j > 0)? Vector::angleBetween(projection, i) : 2 * M_PI - Vector::angleBetween(projection, i); // The \theta component of point in child CS.

        return Point {r, azimuthal, polar }; // The point in child CS.
    };
}

Mapping toSphericalInverse() {
    return [](const Point& point, const Point& origin, const Vector& i, const Vector& j, const Vector& k) {
        const auto& vector = point.positionVector(); // The position vector of the point in child (spherical) coordinate system.
        const auto e = Vector::rotate(i, Vector::crossProduct(i, j), vector[1]).unit(); // The azimuthal unit vector in parent (cartesian) coordinate system.
        const auto unit = Vector::rotate(k, e.perpendicularUnitVectorInDirectionOF(Vector::crossProduct(k, e)), vector[2]).unit(); // The unit vector of the point's position vector in parent (cartesian) coordinate system.
        // Adding the position vector to the origin vector in parent (cartesian) coordinate system.
        return origin + vector[0] * unit;
    };
}

} // namespace cartesian

namespace cylindrical {

Vector add(const Vector &v1, const Vector &v2) {
    // Conveting the vectors' r and theta to cartesian x and y for both v1 and v2
    const auto x1 = v1[0] * cos(v1[1]);
    const auto x2 = v2[0] * cos(v2[1]);
    const auto y1 = v1[0] * sin(v1[1]);
    const auto y2 = v2[0] * sin(v2[1]);

    const auto x = x1 + x2;
    const auto y = y1 + y2;

    // Computing the azimuthal angle (i.e., theta) and r (i.e., length) for the result vector.
    const auto r = sqrt(pow(x, 2) + pow(y, 2));
    const auto theta = (space::isZero(r))? 0: asin( y / r);
    const auto azimuthal = (x < 0 && !space::isZero(x)) ? M_PI - theta : (theta < 0)? 2 * M_PI + theta: theta;

    return Vector{r, azimuthal, v1[2] + v2[2]}; // Creating the result vector.
}

Vector inverse(const Vector vector) {
    const auto azimuthal = (vector[1] > M_PI) ? vector[1] - M_PI : vector[1] + M_PI;
    return Vector{ vector[0], azimuthal, - vector[2] };
}

Vector multiply(const double factor, const Vector &vector) {
    if (factor < 0)
        return multiply(-factor, inverse(vector));
    return Vector{vector[0] * factor, vector[1], vector[2] * factor};
}

double projectionLength(const Vector &projectee, const Vector &base) {
    const auto deltaTheta = abs(projectee[1] - base[1]);                  // the azimuthal angle between the two vector.
    const auto u = unit(base);                                            // The unit vector of the base
    return (u[0] * projectee[0] * cos(deltaTheta) + u[2] * projectee[2]); // dot product.
}

Vector unit(const Vector &vector) {
    const auto length = sqrt(pow(vector[0], 2) + pow(vector[2], 2));
    if (!space::isZero(length) && length > 0)
        return multiply(1/length, vector);
    throw std::runtime_error("Vector of length " + std::to_string(length) + "cannot be normalized.");
}

Mapping toCartesian() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        // The position vector of the point in the cylindrical coordinate system with respect to the origin of the cartesian coordinate system.
        const auto& difVector = add( point.positionVector(), inverse(origin.positionVector()) );
        return Point{ projectionLength(difVector, i), projectionLength(difVector, j), projectionLength(difVector, k) };
    };
}

Mapping toCartesianInverse() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        const auto& vector = point.positionVector(); // The position vector of the point in the child (i.e., cartesian) coordinate system.
        // Converting the position vector cartesian components to parent cylindrical component by multiplying the axes to the component and then adding them.
        return add( origin.positionVector(), add( add( multiply(vector[0], i), multiply(vector[1], j)), multiply(vector[2], k)));
    };
}

Mapping toCylindrical() {
    return [](const Point& point, const Point& origin, const Vector& i, const Vector& j, const Vector& k) {
        const auto difVector = add( point.positionVector(), inverse(origin.positionVector()) ); // The position vector of the point in child CS.
        // Converting the difVector from parrent to child coordinate system.
        const Vector vector{ projectionLength(difVector, i), projectionLength(difVector, j), projectionLength(difVector, k) };
        // Converting the vector (in carteseian coordinate system) to the parent (i.e., shperical) coordinate system.
        const auto r = sqrt(pow(vector[0], 2) + pow(vector[1], 2));
        const auto theta = (space::isZero(r))? 0 : acos( vector[0] / r );
        const auto azimuthal = (vector[0] < 0)? 2 * M_PI - theta : theta;
        return Point{r, azimuthal, vector[2]};
    };
}

Mapping toCylindricalInverse() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        const auto& vector = point.positionVector(); // The position vector of the point in child coordinate system.
        // Computing the projection of vector on each axis.
        const auto vi = multiply(projectionLength(vector, i), i);
        const auto vj = multiply(projectionLength(vector, j), j);
        const auto vk = multiply(projectionLength(vector, k), k);
        // Adding the axis vector to the origin will give the point in parent coordinate system.
        return add(origin.positionVector(), add( add( vi, vj ), vk ));
    };
}

Mapping toSpherical() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        // The position vector of the point in the cylindrical coordinate system with respect to the origin of the spherical coordinate system.
        const auto& difVector = add( point.positionVector(), inverse(origin.positionVector()) );
        // Converting the difVector from parrent to cylindrical coordinate system.
        const Vector vector{ projectionLength(difVector, i), projectionLength(difVector, j), projectionLength(difVector, k) };
        // Converting the vector (in cylindrical coordinate system) to the parent (i.e., shperical) coordinate system.
        const auto r_cylindrical = sqrt(pow(vector[0], 2) + pow(vector[1], 2));
        const auto theta = (space::isZero(r_cylindrical))? 0 : acos( vector[0] / r_cylindrical );
        const auto azimuthal = (vector[0] < 0)? 2 * M_PI - theta : theta;
        const auto r = sqrt(pow(r_cylindrical, 2) + pow(vector[2], 2));
        const auto polar = (space::isZero(r))? 0 : (vector[2] < 0)? M_PI - asin(r_cylindrical / r) : asin(r_cylindrical / r);

        return Point{ r, azimuthal, polar };
    };
}

Mapping toSphericalInverse() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        const auto& posVector = point.positionVector(); // The position vector of the point in the child (i.e., spherical) coordinate system.
        // Converting the position vector spherical components to cylindrical component.
        const auto vector = Vector{ posVector[0] * sin(posVector[2]), posVector[1], posVector[0] * cos(posVector[2])};
        // Computing the projection of vector on each axis.
        const auto vi = multiply(projectionLength(vector, i), i);
        const auto vj = multiply(projectionLength(vector, j), j);
        const auto vk = multiply(projectionLength(vector, k), k);
        // Adding the axis vector to the origin will give the point in parent coordinate system.
        return add(origin.positionVector(), add( add( vi, vj ), vk ));
    };
}

} // namespace cylindrical

namespace spherical {

Vector add(const Vector &v1, const Vector &v2) {
    // Computing the r and z component related to the cylindrical coordinate system.
    const auto z1 = v1[0] * cos(v1[2]);
    const auto z2 = v2[0] * cos(v2[2]);
    const auto r1 = v1[0] * sin(v1[2]);
    const auto r2 = v2[0] * sin(v2[2]);

    // Conveting the vectors' r and theta to cartesian x and y for both v1 and v2
    const auto x1 = r1 * cos(v1[1]);
    const auto x2 = r2 * cos(v2[1]);
    const auto y1 = r1 * sin(v1[1]);
    const auto y2 = r2 * sin(v2[1]);

    const auto x = x1 + x2;
    const auto y = y1 + y2;
    const auto z = z1 + z2;

    // Computing the azimuthal angle (i.e., theta) and polar (i.e., phi) for the result vector.
    const auto r = sqrt(pow(x, 2) + pow(y, 2));
    const auto l = sqrt(pow(r, 2) + pow(z, 2));

    const auto theta = (space::isZero(r))? 0: asin( y / r);
    const auto azimuthal = (x < 0 && !space::isZero(x)) ? M_PI - theta : (theta < 0)? 2 * M_PI + theta: theta;
    const auto polar = (space::isZero(l)) ? 0 : M_PI_2 - asin(z/l);

    return Vector{l, azimuthal, polar}; // Creating the result vector.
}

Vector inverse(const Vector vector) {
    const auto azimuthal = (vector[1] > M_PI) ? vector[1] - M_PI : vector[1] + M_PI;
    const auto polar     = M_PI - vector[2];
    return Vector{ vector[0], azimuthal, polar };
}

Vector multiply(const double factor, const Vector &vector) {
    if (factor < 0)
        return multiply(-factor, inverse(vector));
    return Vector{ vector[0] * factor, vector[1], vector[2] };
}

double projectionLength(const Vector &projectee, const Vector &base) {
    const auto r = projectee[0];                                                // The lenght of the projectee
    return r * cos(base[1]) * sin(base[2]) * cos(projectee[1]) * sin(projectee[2])
           + r * sin(base[1]) * sin(base[2]) * sin(projectee[1]) * sin(projectee[2])
           + r * cos(base[2]) * cos(projectee[2]);                              // dot product.
}

Vector unit(const Vector &vector) {
    return Vector{1, vector[1], vector[2]};
}

Mapping toCartesian() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        // The position vector of the point in the spherical coordinate system with respect to the origin of the cartesian coordinate system.
        const auto& difVector = add(point.positionVector(), inverse(origin.positionVector()));
        return Point{ projectionLength(difVector, i), projectionLength(difVector, j), projectionLength(difVector, k) };
    };
}

Mapping toCartesianInverse() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        const auto& vector = point.positionVector(); // The position vector of the point in the child (i.e., cartesian) coordinate system.
        // Converting the position vector cartesian components to parent spherical component by multiplying the axes to the component and then adding them.
        return add(origin.positionVector(), add( add( multiply(vector[0], i), multiply(vector[1], j)), multiply(vector[2], k)));
    };
}

Mapping toCylindrical() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        // The position vector of the point in the spherical coordinate system with respect to the origin of the cylindrical coordinate system.
        const auto& difVector = add(point.positionVector(), inverse(origin.positionVector()));
        // Compute the projection lenght of the vector on each axis.
        const Vector vector{ projectionLength(difVector, i), projectionLength(difVector, j), projectionLength(difVector, k) };
        // Converting the vector (in carteseian coordinate system) to the parent (i.e., shperical) coordinate system.
        const auto r = sqrt(pow(vector[0], 2) + pow(vector[1], 2));
        const auto theta = (space::isZero(r))? 0 : acos( vector[0] / r );
        const auto azimuthal = (vector[0] < 0)? 2 * M_PI - theta : theta;
        return Point{r, azimuthal, vector[2]};
    };
}

Mapping toCylindricalInverse() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        const auto& posVector = point.positionVector(); // The position vector of the point in the child (i.e., cylindrical) coordinate system.
        // Converting the position vector cylindrical components to spherical component.
        const auto r = sqrt(pow(posVector[0], 2) + pow(posVector[2], 2));
        const auto polar = (space::isZero(r))? 0 : (posVector[0] > r)? M_PI_2 : asin(posVector[0] / r);
        const auto vector = Vector{r, posVector[1], polar};
        // Computing the projection of vector on each axis.
        const auto vi = multiply(projectionLength(vector, i), i);
        const auto vj = multiply(projectionLength(vector, j), j);
        const auto vk = multiply(projectionLength(vector, k), k);
        // Adding the axis vector to the origin will give the point in parent coordinate system.
        return add(origin.positionVector(), add( add( vi, vj ), vk ));
    };
}

Mapping toSpherical() {
    return [](const Point& point, const Point& origin, const Vector& i, const Vector& j, const Vector& k) {
        // The position vector of the point in the cylindrical coordinate system with respect to the origin of the spherical coordinate system.
        const auto& difVector = add( point.positionVector(), inverse(origin.positionVector()) );
        // Converting the difVector from parrent to cylindrical coordinate system.
        const Vector vector{ projectionLength(difVector, i), projectionLength(difVector, j), projectionLength(difVector, k) };
        // Converting the vector (in cylindrical coordinate system) to the parent (i.e., shperical) coordinate system.
        const auto r_cylindrical = sqrt(pow(vector[0], 2) + pow(vector[1], 2));
        const auto theta = (space::isZero(r_cylindrical))? 0 : acos( vector[0] / r_cylindrical );
        const auto azimuthal = (vector[0] < 0)? 2 * M_PI - theta : theta;
        const auto r = sqrt(pow(r_cylindrical, 2) + pow(vector[2], 2));
        const auto polar = (space::isZero(r))? 0 : (vector[2] < 0)? M_PI - asin(r_cylindrical / r) : asin(r_cylindrical / r);

        return Point{ r, azimuthal, polar };
    };
}

Mapping toSphericalInverse() {
    return [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
        const auto& vector = point.positionVector(); // The position vector of the point in child coordinate system.
        // Computing the projection of vector on each axis.
        const auto vi = multiply(projectionLength(vector, i), i);
        const auto vj = multiply(projectionLength(vector, j), j);
        const auto vk = multiply(projectionLength(vector, k), k);
        // Adding the axis vector to the origin will give the point in parent coordinate system.
        return add(origin.positionVector(), add( add( vi, vj ), vk ));
    };
}

} // namespace spherical

} // namespace rbs::coordinate_system::convertors

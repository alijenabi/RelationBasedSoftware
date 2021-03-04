//
//  VectorImp.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.01.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VECTORIMP_H
#define VECTORIMP_H

#ifndef VECTOR_H
#   include "Vector.h"
#endif // VECTOR_H

namespace rbs::space {

template<unsigned char SpaceDimension>
Vector<SpaceDimension>::Vector() : Space() {}

template<unsigned char SpaceDimension>
Vector<SpaceDimension>::Vector(const Vector &other) : Space(other) {}

template<unsigned char SpaceDimension>
Vector<SpaceDimension>::Vector(Vector &&other) : Space(other) {}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::operator =(Vector other){
    Space::operator =(other);
    return *this;
}

template<unsigned char SpaceDimension>
bool Vector<SpaceDimension>::areParallel(const Vector<SpaceDimension> &v1,
                                         const Vector<SpaceDimension> &v2) {
    const auto v1onv2 = v1.projectionOn(v2);
    return v1onv2 == v1 || v1onv2 == -v1;
}

template<unsigned char SpaceDimension>
bool Vector<SpaceDimension>::arePerpendicular(const Vector<SpaceDimension> &v1,
                                              const Vector<SpaceDimension> &v2) {
    return space::isZero(v1.projectionValueOn(v2));
}

template<unsigned char SpaceDimension>
Vector<3> Vector<SpaceDimension>::crossProduct(const Vector<SpaceDimension> &lhs,
                                               const Vector<SpaceDimension> &rhs){
    Vector<3> ans;
    if (lhs.dimension() > 3) {
        throw std::runtime_error("Vector cross product is only implemented up to 3D!");
    } else if (lhs.dimension() == 2){
        ans[0] = 0;
        ans[1] = 0;
        ans[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
        return ans;
    } else if (lhs.dimension() < 2){
        return ans;
    }
    ans[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
    ans[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
    ans[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
    return ans;
}

template<unsigned char SpaceDimension>
double Vector<SpaceDimension>::dotProduct(const Vector<SpaceDimension> &lhs,
                                          const Vector<SpaceDimension> &rhs) {
    return std::inner_product(lhs.begin(), lhs.end(), rhs.begin(), 0.0);
}

template<unsigned char SpaceDimension>
bool Vector<SpaceDimension>::haveSameDirections(const Vector<SpaceDimension> &v1,
                                                const Vector<SpaceDimension> &v2) {
    return v1.projectionLenghtOn(v2) > 0 && areParallel(v1, v2);
}

template<unsigned char SpaceDimension>
Vector<3> Vector<SpaceDimension>::normalTo(const Vector<SpaceDimension> &v1,
                                           const Vector<SpaceDimension> &v2) {
    return crossProduct(v1, v2).unit();
}

template<unsigned char SpaceDimension>
double Vector<SpaceDimension>::angleBetween(const Vector<SpaceDimension> &v1, const Vector<SpaceDimension> &v2) {
    const auto length1 = v1.length();
    const auto length2 = v2.length();
    const auto dot = (space::isZero(length1) || space::isZero(length2)) ? 1 : v1 * v2 / (length1 * length2);

    // Avoiding double precision error.
    if (dot > 1) {
        return 0;
    } else if (dot < -1) {
        return M_PI;
    } else if (space::isZero(dot)) {
        return M_PI_2;
    }
    return acos(dot);
}

template<unsigned char SpaceDimension>
Vector<2> Vector<SpaceDimension>::rotate(const Vector<3> &vector, double angle) {
    const auto cost = cos(angle);
    const auto sint = sin(angle);
    return { vector[0] * cost + vector[1] * sint, vector[0] * sint + vector[1] * cost };
}

template<unsigned char SpaceDimension>
Vector<3> Vector<SpaceDimension>::rotate(const Vector<3> &vector, const Vector<3> &rotationAxis, double angle) {
    if (areParallel(vector, rotationAxis))
        return vector;

    // Find the vector projection unit vector on a plane where its normal vector is rotation axis.
    const auto vectorOnRotationPlane = rotationAxis.perpendicularUnitVectorInDirectionOF(vector);

    // Find the angle of the projected unit vector to the vector itself.
    const auto length = vector.length();

    // Avoiding nan values due to double precision error.
    const auto dotRhs = vectorOnRotationPlane * vector / length;
    const auto vectorAngle = (dotRhs > 1)? 0 : ((dotRhs < -1)? M_PI : acos(dotRhs));

    // Rotate the projected unit vector around the rotation axes.
    const auto rotatedUnit = rotateAroundNormalAxes(vectorOnRotationPlane, rotationAxis, angle);

    // Find the rotation to return the rotated unit vector to its original direction.
    // Note the direction of the rotation axes for this operation is crossProduct(rotatedUnit, rotationAxis)
    const auto projectionOnRotationAxis = vector.projectionValueOn(rotationAxis);
    const auto returnAngle = (space::isZero(projectionOnRotationAxis) || projectionOnRotationAxis > 0 ) ? vectorAngle : -vectorAngle;

    // Rotate the normal rotated unit vector to the original direction and return its lenght to original lenght as well.
    return rotateAroundNormalAxes(rotatedUnit, crossProduct(rotatedUnit, rotationAxis), returnAngle).unit() * length;
}

template<unsigned char SpaceDimension>
bool Vector<SpaceDimension>::isZero() const {
    return std::all_of(Space::begin(), Space::end(),
                       [](const auto& value){
                            return space::isZero(value);
                        });
}

template<unsigned char SpaceDimension>
double Vector<SpaceDimension>::length() const {
    return sqrt(lengthSquared());
}

template<unsigned char SpaceDimension>
double Vector<SpaceDimension>::lengthSquared() const{
    return std::reduce(Space::begin(), Space::end(),
                       0.0,
                       [](const auto & init, const auto& value){ return init + std::pow(value, 2); });
}

template<unsigned char SpaceDimension>
std::string Vector<SpaceDimension>::name() const {
    return "Vector";
}

template<unsigned char SpaceDimension>
const Vector<SpaceDimension> Vector<SpaceDimension>::perpendicularUnitVectorInDirectionOF(const Vector &other) const{
    return Vector<SpaceDimension>::crossProduct(Vector<SpaceDimension>::crossProduct(*this, other), *this).unit();
}

template<unsigned char SpaceDimension>
double Vector<SpaceDimension>::projectionLenghtOn(const Vector &other) const {
    return abs(projectionValueOn(other));
}

template<unsigned char SpaceDimension>
double Vector<SpaceDimension>::projectionValueOn(const Vector &other) const {
    if (*this == other)
        return length();
    if (length()) {
        return (*this) * other/other.length();
    }
    return 0;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> Vector<SpaceDimension>::projectionOn(const Vector &other) const {
    return projectionValueOn(other) * other.unit();
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> Vector<SpaceDimension>::unit() const {
    const auto l = length();
    if (l > 0)
        return *this / l;
    throw std::runtime_error("Vector of length " + std::to_string(l) + "cannot be normalized.");
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::normalize() {
    const auto l = length();
    if (l > 0) {
        *this /= length();
        return *this;
    }
    throw std::runtime_error("Vector of length " + std::to_string(l) + "cannot be normalized.");
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::zero() {
    std::transform(Space::begin(), Space::end(), &Space::operator[](0), [](const auto &) {return 0;});
    return *this;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::operator+=(double number) {
    std::transform(Space::begin(), Space::end(), &Space::operator[](0),
                   [number](const auto & value) {return value + number;});
    return *this;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::operator+=(const Vector &other) {
    std::transform(Space::begin(), Space::end(), other.begin(), &Space::operator[](0),
                   [](const auto& a, const auto& b) {return a + b;});
    return *this;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::operator-=(double number) {
    std::transform(Space::begin(), Space::end(), &Space::operator[](0),
                   [number](const auto& value) {return value - number;});
    return *this;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::operator-=(const Vector &other) {
    std::transform(Space::begin(), Space::end(), other.begin(), &Space::operator[](0),
                   [](const auto& a, const auto& b) {return a - b;});
    return *this;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::operator*=(double factor) {
    std::transform(Space::begin(), Space::end(), &Space::operator[](0),
                   [factor](const auto& value) {return value * factor;});
    return *this;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::operator^=(double power) {
    std::transform(Space::begin(), Space::end(), &Space::operator[](0),
                   [power](const auto value) {return std::pow(value, power);});
    return *this;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Vector<SpaceDimension>::operator/=(double divisor) {
    std::transform(Space::begin(), Space::end(), &Space::operator[](0),
                   [divisor](const auto value) {return value / divisor;});
    return *this;
}

template<unsigned char SpaceDimension>
Vector<3> Vector<SpaceDimension>::rotateAroundNormalAxes(const Vector<3> &vector, const Vector<3> &axis, double angle) {
    if (!arePerpendicular(axis, vector))
        throw std::runtime_error("The rotation axis is not normal to the vector.");

    const auto i = vector.unit();
    const auto j = crossProduct(axis, i).unit();
    const auto x = vector.length();
    return x * cos(angle) * i + x * sin(angle) * j;
}

} // namespace rbs::space

#endif // VECTORIMP_H

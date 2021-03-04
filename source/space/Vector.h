//
//  Vector.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.01.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VECTOR_H
#define VECTOR_H

#include "Space.h"
#include <numeric>

/**
 * The space namespace provides the means to handle matters related to the mathematical space.
 */
namespace rbs::space {

/**
 * @brief The Vector class provides the means to manage vectors for all space sizes.
 * @note The linear algebra libraries provide more than necessary functionality, which
 *       comes with the cost of complexity and overhead for this module's purpose.
 * @details The linear algebra libraries bring simplicity to the implementation of numerical methods.
 *          This class provides the vectors, their operation, and the required computation.
 */
template<unsigned char SpaceDimension>
class Vector :public Space<double, SpaceDimension> {
public:

    using Component = double;

    /**
     * @brief The constructor.
     */
    Vector();

    /**
     * @brief The constructor.
     * @param values The initializer list.
     * @throws If the values has any size but the space dimension
     */
    Vector(std::initializer_list<double> values) : Space(values) {}

    /**
     * @brief The copy constructor.
     * @param other The other Vector to copy.
     */
    Vector(const Vector & other);

    /**
     * @brief The move constructor.
     * @param other The other Vector to move.
     */
    Vector(Vector && other);

    /**
     * @brief Default virtual destructor.
     */
    virtual ~Vector() = default;

    /**
     * @brief Move/copy assign operator.
     * @param other The other Vector to move or copy.
     * @return The new Vector.
     */
    Vector & operator =(Vector other);

    /**
     * @brief Computes the angle between two vectors.
     * @note The angle between any vector and zero vector in zero.
     * @param v1 The firest vector.
     * @param v2 The second vector.
     * @return Angle between 0 to PI.
     */
    static double angleBetween(const Vector& v1, const Vector& v2);

    /**
     * @brief Computes whether two vectors are parallel or not.
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @return True if the vectors are parallel, false otherwise.
     */
    static bool areParallel(const Vector& v1, const Vector& v2);

    /**
     * @brief Computes whether two vectors are perpendicular or not.
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @return True if the vectors are perpendicular, false otherwise.
     */
    static bool arePerpendicular(const Vector& v1, const Vector& v2);

    /**
     * @brief Computes the cross product of two vectors.
     * @param v1 The first vector.
     * @param v2 The second vector.
     */
    static Vector<3> crossProduct(const Vector& lhs, const Vector& rhs);

    /**
     * @brief Computes the dot product of two vectors.
     * @param v1 The first vector.
     * @param v2 The second vector.
     */
    static double dotProduct(const Vector& lhs, const Vector& rhs);

    /**
     * @brief Computes whether two vectors have a same direction.
     * @note If true, the vectors lenght might or mightnot be equal.
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @return True if the vectors are in the same direction, false otherwise.
     */
    static bool haveSameDirections(const Vector& v1, const Vector& v2);

    /**
     * @brief Computes the normal vector to two vectors.
     * @note Uses right hand side role to compute the normal.
     * @param v1 The first vector.
     * @param v2 The second vector.
     */
    static Vector<3> normalTo(const Vector& v1, const Vector& v2);

    /**
     * @brief Rotates a 2D vector around Z-axis with a specific angle.
     * @param vector The vector to rotate.
     * @param angle The angle of rotation.
     */
    static Vector<2> rotate(const Vector<3>& vector, double angle);

    /**
     * @brief Rotates a 3D vector around an other 3D vector with a specific angle.
     * @param vector The vector to rotate.
     * @param rotationAxis The rotation axis's direction vector (dose not need to be unit vector).
     * @param angle The angle of rotation (rad) using rigth hand role.
     */
    static Vector<3> rotate(const Vector<3>& vector, const Vector<3>& rotationAxis, double angle);

    /**
     * @brief Defines whether all the component of the vector are zero or not.
     */
    bool isZero() const;

    /**
     * @brief Computes the vector's length.
     */
    double length() const;

    /**
     * @brief Computes the vector's squared length.
     */
    double lengthSquared() const;

    /**
     * @brief Gives the name of the class.
     */
    virtual std::string name() const override;

    /**
     * @brief Finds a perpendicular vector to this vector on direction of other vector
     * @param other The direction vector.
     */
    const Vector perpendicularUnitVectorInDirectionOF(const Vector &other) const;

    /**
     * @brief Computed the lenght of this vector's projection on other vector.
     * @param other The other vector.
     */
    double projectionLenghtOn(const Vector& other) const;

    /**
     * @brief Computed the value of this vector's projection on other vector.
     * @note This will return negative values if the projection is in the opposite direction with the other vector.
     * @param other The other vector.
     */
    double projectionValueOn(const Vector& other) const;

    /**
     * @brief Computed the projection vector of this vector on other vector.
     * @param other The other vector.
     */
    Vector projectionOn(const Vector& other) const;

    /**
     * @brief Computes the unit vector.
     * @throws If the lenght of the vector is zero.
     */
    Vector unit() const;

    /**
     * @brief Normalizes this vector.
     * @return The result.
     * @throws If the lenght of the vector is zero.
     */
    Vector & normalize();

    /**
     * @brief Zeros the vetor and returns the result.
     * @return The result.
     */
    Vector & zero();

    Vector &operator+=(double number);
    Vector &operator+=(const Vector &other);
    Vector &operator-=(double number);
    Vector &operator-=(const Vector &other);
    Vector &operator*=(double factor);
    Vector &operator^=(double power);
    Vector &operator/=(double divisor);

    constexpr friend inline const Vector operator+(const Vector &vector) {
        return vector;
    }
    constexpr friend inline const Vector operator+(const Vector &v1, const Vector &v2) {
        Vector<SpaceDimension> ans;
        std::transform(v1.begin(), v1.end(), v2.begin(), &ans[0], [](const auto a, const auto b) {return a + b;});
        return ans;
    }
    constexpr friend inline const Vector operator+(const double &n, const Vector &v) {
        Vector<SpaceDimension> ans;
        std::transform(v.begin(), v.end(), &ans[0], [n](const auto value) {return n + value;});
        return ans;
    }
    constexpr friend inline const Vector operator+(const Vector &v, const double &n) {
        Vector<SpaceDimension> ans;
        std::transform(v.begin(), v.end(), &ans[0], [n](const auto value) {return value + n;});
        return ans;
    }

    constexpr friend inline const Vector operator-(const Vector<SpaceDimension> &vector) {
        Vector<SpaceDimension> ans;
        std::transform(vector.begin(), vector.end(), &ans[0], [](const double & a) {return -a;});
        return ans;
    }
    constexpr friend inline const Vector operator-(const Vector &v1, const Vector &v2) {
        Vector<SpaceDimension> ans;
        std::transform(v1.begin(), v1.end(), v2.begin(), &ans[0], [](const auto a, const auto b) {return a - b;});
        return ans;
    }
    constexpr friend inline const Vector operator-(const double &n, const Vector &v) {
        Vector<SpaceDimension> ans;
        std::transform(v.begin(), v.end(), &ans[0], [n](const auto value) {return n - value;});
        return ans;
    }
    constexpr friend inline const Vector operator-(const Vector &v, const double &n) {
        Vector<SpaceDimension> ans;
        std::transform(v.begin(), v.end(), &ans[0], [n](const auto value) {return value - n;});
        return ans;
    }

    friend inline const Vector<3> operator|(const Vector &v1, const Vector &v2) {
        return Vector<SpaceDimension>::crossProduct(v1, v2);
    }
    friend inline double operator*(const Vector &v1, const Vector &v2) {
        return Vector<SpaceDimension>::dotProduct(v1, v2);
    }

    constexpr friend inline const Vector operator*(const double &factor, const Vector &v) {
        Vector<SpaceDimension> ans;
        std::transform(v.begin(), v.end(), &ans[0], [factor](const auto value) {return factor * value;});
        return ans;
    }
    constexpr friend inline const Vector operator*(const Vector &v, const double &factor) {
        Vector<SpaceDimension> ans;
        std::transform(v.begin(), v.end(), &ans[0], [factor](const auto value) {return value * factor;});
        return ans;
    }
    constexpr friend inline const Vector operator^(const Vector &v, double power) {
        Vector<SpaceDimension> ans;
        std::transform(v.begin(), v.end(), &ans[0], [power](const auto value) {return std::pow(value, power);});
        return ans;
    }
    constexpr friend inline const Vector operator/(const Vector &v, double divisor) {
        Vector<SpaceDimension> ans;
        std::transform(v.begin(), v.end(), &ans[0], [divisor](const auto value) {return value / divisor;});
        return ans;
    }

protected:

    using Space = Space<double, SpaceDimension>;

private:

    /**
     * @brief Rotate a vector around a rotation vector that is normal to the vector.
     * @param vector The vector to rotate.
     * @param axis The normal rotation axis.
     * @param angle The angle of rotation in a direction of right hand role.
     * @throws If the angle of the rotation axis with the vecotr is not 90 degree.
     */
    static Vector<3> rotateAroundNormalAxes(const Vector<3>& vector, const Vector<3>& axis, double angle);

};

using vec3 = Vector<3>;
using vec2 = Vector<2>;
using vec1 = Vector<1>;
using scalar = vec1;

namespace consts {

    const Vector<1> o1D     = {0};
    const Vector<1> one1D   = {1};
    const Vector<1> i1D     = {1};

    const Vector<2> o2D     = { 0,  0};
    const Vector<2> one2D   = { 1,  1};
    const Vector<2> i2D     = { 1,  0};
    const Vector<2> j2D     = { 0,  1};
    const Vector<2> _2D1st  = { 1,  1};
    const Vector<2> _2D2nd  = { 1, -1};
    const Vector<2> _2D3rd  = {-1, -1};
    const Vector<2> _2D4th  = {-1,  1};

    const Vector<3> o3D     = { 0,  0,  0};
    const Vector<3> one3D   = { 1,  1,  1};
    const Vector<3> i3D     = { 1,  0,  0};
    const Vector<3> j3D     = { 0,  1,  0};
    const Vector<3> k3D     = { 0,  0,  1};
    const Vector<3> _3D1st  = { 1,  1,  1};
    const Vector<3> _3D2nd  = { 1, -1,  1};
    const Vector<3> _3D3rd  = {-1, -1,  1};
    const Vector<3> _3D4th  = {-1,  1,  1};
    const Vector<3> _3D5th  = { 1,  1, -1};
    const Vector<3> _3D6th  = { 1, -1, -1};
    const Vector<3> _3D7th  = {-1, -1, -1};
    const Vector<3> _3D8th  = {-1,  1, -1};

    const Vector<3> xyN     = {0, 0, 1};
    const Vector<3> yxN     = {0, 0, 1};
    const Vector<3> xzN     = {0, 1, 0};
    const Vector<3> zxN     = {0, 1, 0};
    const Vector<3> yzN     = {1, 0, 0};
    const Vector<3> zyN     = {1, 0, 0};

} // namespace consts

} // namespace rbs::space

#include "VectorImp.h"

#endif // VECTOR_H

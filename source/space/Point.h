//
//  Point.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.01.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef POINT_H
#define POINT_H

#include "Vector.h"

/**
 * The space namespace provides the means to handle matters related to the mathematical space.
 */
namespace rbs::space {

/**
 * @brief The Point class provides the means to manage points in any space.
 * @note Often the concept of point in space is presented with a vector (i.e., position vector
 *       of the point). The problem with this approach is that it makes it very easy to unintentionally
 *       perform vector arithmetic on points.
 */
template<unsigned char SpaceDimension>
class Point
{
public:

    using Index = unsigned char;

    /**
     * @brief The constructor.
     */
    Point();

    /**
     * @brief The constructor.
     * @param values The initializer list.
     * @throws If the values has any size but the space dimension.
     */
    Point(std::initializer_list<double> values);

    /**
     * @brief The copy constructor.
     * @param other The other Point to copy.
     */
    Point(const Point & other);

    /**
     * @brief The move constructor.
     * @param other The other Point to move.
     */
    Point(Point && other);

    /**
     * @brief The Vector copy constructor.
     * @param positionVector The point position vector.
     */
    Point(const Vector<SpaceDimension> & positionVector);

    /**
     * @brief The Vector move constructor.
     * @param positionVector The point position vector.
     */
    Point(Vector<SpaceDimension> && positionVector);

    /**
     * @brief Default virtual destructor.
     */
    virtual ~Point () = default;

    /**
     * @brief Move/copy assign operator.
     * @param other The other Point to move or copy.
     * @return The new Vector.
     */
    Point & operator =(Point other);

    /**
     * @brief Computes the centre of the point list.
     * @param points The list of points
     * @return The mid point of the point lists.
     */
    static Point centre(std::vector<Point> points);

    /**
     * @brief Computes the distance between two points.
     * @param p1 The first point.
     * @param p2 The second point.
     */
    static double distance(const Point & p1, const Point & p2);

    /**
     * @brief Computes the point located in the middle of the two other points.
     * @param p1 The first point.
     * @param p2 The second point.
     */
    static Point midPoint(const Point & p1, const Point & p2);

    /**
     * @brief The vector between two points.
     * @param origin The origin of the vector.
     * @param end The end point of the vector.
     */
    static Vector<SpaceDimension> vector(const Point & origin, const Point& end);

    /**
     * @brief Gives the dimension of the space.
     */
    constexpr unsigned char dimension() const;

    /**
     * @brief Gives the name of the class.
     */
    virtual std::string name() const;

    /**
     * @brief Gives read-only access to the point's position vector.
     */
    const Vector<SpaceDimension>& positionVector() const;

    /**
     * @brief Gives direct access to the point's position vector.
     * @return
     */
    Vector<SpaceDimension>& positionVector();

    /**
     * @brief Zeros the point's position vector.
     * @return The result.
     */
    Point& zero();

    Point &operator+=(const Vector<SpaceDimension> &other);
    Point &operator-=(const Vector<SpaceDimension> &other);

    constexpr friend inline bool operator==(const Point &p1, const Point &p2){
        if (!p1.p_postitionVector && !p1.p_postitionVector)
            return true;
        return p1.p_postitionVector && p1.p_postitionVector &&  *p1.p_postitionVector == *p2.p_postitionVector;
    }
    constexpr friend inline bool operator!=(const Point &p1, const Point &p2){
        return !operator==(p1, p2);
    }

    constexpr friend inline const Point operator+(const Point &point, const Vector<SpaceDimension> &vector) {
        Vector<SpaceDimension> ans = *point.p_postitionVector;
        ans += vector;
        return ans;
    }
    constexpr friend inline const Point operator-(const Point &point, const Vector<SpaceDimension> &vector) {
        Vector<SpaceDimension> ans = *point.p_postitionVector;
        ans -= vector;
        return ans;
    }

    operator std::string() const;

protected:

    using Location = Vector<SpaceDimension>;
    using LocationPtr = std::shared_ptr<Location>;

private:

    /**
     * @brief swaps the content of this object with an other objec of same type.
     * @param other The other instance.
     */
    void swap(Point & other);

    static constexpr auto ps_className = "Point";
    LocationPtr p_postitionVector;

};

} // namespace rbs::space

#ifndef POINTIMP_H
#   include "PointImp.h"
#endif // POINTIMP_H

#endif // POINT_H

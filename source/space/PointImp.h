//
//  PointImp.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.01.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef POINTIMP_H
#define POINTIMP_H

#ifndef POINT_H
#   include "Point.h"
#endif // POINT_H

namespace rbs::space {

template<unsigned char SpaceDimension>
Point<SpaceDimension>::Point()
    : p_postitionVector{std::make_shared<Location>()}
{}

template<unsigned char SpaceDimension>
Point<SpaceDimension>::Point(std::initializer_list<double> values)
    : p_postitionVector{std::make_shared<Location>(values)}
{}

template<unsigned char SpaceDimension>
Point<SpaceDimension>::Point(const Point &other)
{
    p_postitionVector = (other.p_postitionVector) ? std::make_shared<Vector<SpaceDimension> >(*other.p_postitionVector) : nullptr;
}

template<unsigned char SpaceDimension>
Point<SpaceDimension>::Point(Point &&other) {
    p_postitionVector = std::move(other.p_postitionVector);
}

template<unsigned char SpaceDimension>
Point<SpaceDimension>::Point(const Vector<SpaceDimension> &positionVector)
    : p_postitionVector{std::make_shared<Location>(positionVector)}
{}

template<unsigned char SpaceDimension>
Point<SpaceDimension>::Point(Vector<SpaceDimension> &&positionVector)
    : p_postitionVector{std::make_shared<Location>(positionVector)}
{}

template<unsigned char SpaceDimension>
Point<SpaceDimension> &Point<SpaceDimension>::operator =(Point other) {
    swap(other);
    return *this;
}

template<unsigned char SpaceDimension>
constexpr unsigned char Point<SpaceDimension>::dimension() const {
    return SpaceDimension;
}

template<unsigned char SpaceDimension>
std::string Point<SpaceDimension>::name() const {
    return ps_className;
}

template<unsigned char SpaceDimension>
Point<SpaceDimension>::operator std::string() const {
    if(p_postitionVector)
        return std::string(name() + "{position_vector: ") + std::string(*p_postitionVector) + "}";
    return std::string(name() + "{position_vector: null}");
}

template<unsigned char SpaceDimension>
Point<SpaceDimension> Point<SpaceDimension>::centre(std::vector<Point> points) {
    return std::reduce(points.begin(), points.end(), Point().zero(), [](const auto & acc, const auto & point) {
        return *acc.p_postitionVector + *point.p_postitionVector;
    }).positionVector() / points.size();
}

template<unsigned char SpaceDimension>
double Point<SpaceDimension>::distance(const Point<SpaceDimension> &p1,
                                       const Point<SpaceDimension> &p2) {
    return (vector(p2, p1)).length();
}

template<unsigned char SpaceDimension>
Point<SpaceDimension> Point<SpaceDimension>::midPoint(const Point<SpaceDimension> &p1,
                                                      const Point<SpaceDimension> &p2) {
    auto ans = p1;
    ans += vector(p1, p2) / 2;
    return ans;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> Point<SpaceDimension>::vector(const Point<SpaceDimension> &origin,
                                                     const Point<SpaceDimension> &end) {
    return end.positionVector() - origin.positionVector();
}

template<unsigned char SpaceDimension>
const Vector<SpaceDimension> &Point<SpaceDimension>::positionVector() const {
    return *p_postitionVector;
}

template<unsigned char SpaceDimension>
Vector<SpaceDimension> &Point<SpaceDimension>::positionVector() {
    return *p_postitionVector;
}

template<unsigned char SpaceDimension>
Point<SpaceDimension> &Point<SpaceDimension>::zero() {
    p_postitionVector->zero();
    return *this;
}

template<unsigned char SpaceDimension>
Point<SpaceDimension> &Point<SpaceDimension>::operator+=(const Vector<SpaceDimension> &other) {
    p_postitionVector->operator += (other);
    return *this;
}

template<unsigned char SpaceDimension>
Point<SpaceDimension> &Point<SpaceDimension>::operator-=(const Vector<SpaceDimension> &other) {
    p_postitionVector->operator -= (other);
    return *this;
}

template<unsigned char SpaceDimension>
void Point<SpaceDimension>::swap(Point &other) {
    std::swap(p_postitionVector, other.p_postitionVector);
}

template<unsigned char SpaceDimension>
std::ostream & operator << (std::ostream & out, const Point<SpaceDimension> & point) {
    return out << std::string(point);
}

} // namespace rbs::space

#endif // POINTIMP_H

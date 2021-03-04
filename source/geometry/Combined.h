//
//  Combined.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 24.05.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef COMBINED_H
#define COMBINED_H

#include "Primary.h"
#include "PointStatus.h"
#include "SetOperation.h"

/**
 * The geometry namespace provides the means to handle geometrical shapes in 3D space space.
 */
namespace rbs::geometry {

/**
 * @brief The Combined class defines any 3D geometry that can be described with combination of
 *        two other Combined; one Primary and one Combined; or two Primary geometies where can
 *        applies one of the set operations (SetOperation) to them.
 */
class Combined: public Geometry
{
public:

    using Thickness = double;
    using FirstGeoPtr = std::shared_ptr<Geometry>;
    using SecondGeoPtr = std::shared_ptr<Geometry>;

    /**
     * @brief The constructor.
     * @param first The first Geometry.
     * @param operation The operation between to two Geometries.
     * @param second The second Geometry.
     */
    Combined(const Primary& first, const SetOperation operation, const Primary& second);

    /**
     * @brief The constructor.
     * @param first The first Geometry.
     * @param operation The operation between to two Geometries.
     * @param second The second Geometry.
     */
    Combined(const Primary& first, const SetOperation operation, const Combined& second);

    /**
     * @brief The constructor.
     * @param first The first Geometry.
     * @param operation The operation between to two Geometries.
     * @param second The second Geometry.
     */
    Combined(const Combined& first, const SetOperation operation, const Primary& second);

    /**
     * @brief The constructor.
     * @param first The first Geometry.
     * @param operation The operation between to two Geometries.
     * @param second The second Geometry.
     */
    Combined(const Combined& first, const SetOperation operation, const Combined& second);

    /**
     * @brief The constructor.
     * @param first The first Geometry.
     * @param operation The operation between to two Geometries.
     * @param second The second Geometry.
     */
    Combined(const FirstGeoPtr& first, const SetOperation operation, const SecondGeoPtr& second);

    /**
     * @brief The copy constructor.
     * @param other The other Combined geomentry to copy.
     */
    Combined(const Combined & other);

    /**
     * @brief The copy constructor.
     * @param other The other Primary geomentry to copy.
     */
    Combined(const Primary & other);

    /**
     * @brief The move constructor.
     * @param other The other Combined to move.
     */
    Combined(Combined && other);

    /**
     * @brief The move constructor.
     * @param other The other Primary to move.
     */
    Combined(Primary && other);

    /**
     * @brief The deconstructor.
     */
    ~Combined();

    /**
     * @brief Move/copy assign operator.
     * @param other The other Combined to move or copy.
     * @return The new Primary.
     */
    Combined &operator =(Combined other);

    /**
     * @brief Move/copy assign operator.
     * @param other The other Primary to move or copy.
     * @return The new Primary.
     */
    Combined &operator =(Primary other);

    ///@ {
    /**
     * @details Static members providing constructor like experiance for creating each type of Combined geomenties.
     */

    ///@ {
    /**
     * @details Static members providing constructor like experiance for creating 0D Combined geomenties.
     */

    /**
     * @brief Generates an Empty Combined Shape.
     */
    static Combined Empty();

    ///@ }

    ///@ {
    /**
     * @details Static members providing constructor like experiance for creating coordinate system components as Combined geomenties.
     */

    /**
     * @brief Generates apoint at the origin of the coordinate system.
     */
    static Combined Origin();

    /**
     * @brief Generates a Combined that presents the whole of the 3D space.
     */
    static Combined Space();

    /**
     * @brief Gives the X-Axis Line
     */
    static Combined XAxisLine();

    /**
     * @brief Gives the XY-Plane
     */
    static Combined XYPlane();

    /**
     * @brief Gives the XZ-Plane
     */
    static Combined XZPlane();

    /**
     * @brief Gives the Y-Axis Line
     */
    static Combined YAxisLine();

    /**
     * @brief Gives the YX-Plane
     */
    static Combined YXPlane();

    /**
     * @brief Gives the YZ-Plane
     */
    static Combined YZPlane();

    /**
     * @brief Gives the Z-Axis Line
     */
    static Combined ZAxisLine();

    /**
     * @brief Gives the ZX-Plane
     */
    static Combined ZXPlane();

    /**
     * @brief Gives the YX-Plane
     */
    static Combined ZYPlane();

    ///@ }

    ///@ {
    /**
     * @details Static members providing constructor like experiance for creating 1D Combined geomenties.
     */

    /**
     * @brief Generates a Combined Line having a point and a parallel vector to the line.
     * @param point A point on the line.
     * @param vector A vector parallel to the line.
     */
    static Combined Line(const Point3D& point, const Vector3D& vector);

    /**
     * @brief Generates a Combined Line having two points.
     * @param point1 The first point.
     * @param point2 the second point.
     */
    static Combined Line(const Point3D& point1, const Point3D& point2);

    /**
     * @brief Generates a Combined Line having two points.
     * @param point1 The first point.
     * @param point2 the second point.
     * @throws If one of the points has not the Shape::Point.
     */
    static Combined Line(const Primary& point1, const Primary& point2);

    /**
     * @brief Generates a Combined geometry presenting a line ray.
     * @param point The point end point of the ray
     * @param directionVector The direction vector of the ray.
     */
    static Combined LineRay(const Point3D& point, const Vector3D& directionVector);

    /**
     * @brief Generates a Combined geometry presenting a line segment
     * @param p1 The first end of the line segment.
     * @param p2 The second end of the line segment.
     */
    static Combined LineSegment(const Point3D& p1, const Point3D& p2);

    /**
     * @brief Generates a Combined geometry presenting a line segment.
     * @param centre The middle point of the line segment.
     * @param radiusVector The vector form the centre to one of the line ends.
     */
    static Combined LineSegment(const Point3D& centre, const Vector3D& radiusVector);

    /**
     * @brief Generates a Combined Point.
     * @param values The component list.
     * @throws If the values has any size but 3.
     */
    static Combined Point(std::initializer_list<double> values);

    /**
     * @brief Generates a Combined Point having a 3D position vector.
     * @param positionVector The point position vector in 3D space.
     */
    static Combined Point(const Vector3D& positionVector);

    /**
     * @brief Generates a Combined Point located at a point in 3D space.
     * @param point A point locaion in 3D space.
     */
    static Combined Point(const Point3D& point);

    ///@ }

    ///@ {
    /**
     * @details Static members providing constructor like experiance for creating 2D Combined geomenties.
     */

    /**
     * @brief Generates a Circle.
     * @param centre The centre of the shape.
     * @param planeNormal The normal vector of the plane that shape located on it.
     * @param radiusVector The radius vector of the circle.
     */
    static Combined Circle(const Point3D& centre, const Vector3D& planeNormal, const Vector3D& radiusVector);

    /**
     * @brief Generates a EquilateralTriangle.
     * @param centre The centre of the shape.
     * @param planeNormal The normal vector of the plane that shape is located on it.
     * @param radiusVector The vector between one of the vertices and the centre.
     * @throws If the planeNormal is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined EquilateralTriangle(const Point3D& centre, const Vector3D& planeNormal,const Vector3D& radiusVector);

    /**
     * @brief Generates a EquilateralTriangle.
     * @param centre The centre of the shape.
     * @param vertex One of the shape vertices
     * @param vectorOnPlane Any vector lying on the plane of the Polygon.
     * @throws If the planeNormal calculated to be a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined EquilateralTriangle(const Point3D& centre, const Point3D& vertex, const Vector3D& vectorOnPlane);

    /**
     * @brief Generates a Paralelogram.
     * @param vertex One of the shape vertices.
     * @param vector1 The vector between the "vertex" and the adjacent vertex.
     * @param vector2 The vector between the "vertex" and other adjacent vertex.
     */
    static Combined Parallelogram(const Point3D& vertex, const Vector3D& vector1, const Vector3D& vector2);

    /**
     * @brief Generates a Combined Plane having a point and a normal vector.
     * @param point A point on the plane.
     * @param normalVector The normal vector of plane.
     */
    static Combined Plane(const Point3D& point, const Vector3D& normalVector);

    /**
     * @brief Generates a Combined Plane having three points on the plane.
     * @param point1 The first point.
     * @param point2 the second point.
     * @param point3 the third point.
     */
    static Combined Plane(const Point3D& point1, const Point3D& point2, const Point3D& point3);

    /**
     * @brief Generates a Combined Plane havin three Primary Points.
     * @param point1 The first point.
     * @param point2 the second point.
     * @param point3 the third point.
     * @throws If one of the points has not the Shape::Point.
     */
    static Combined Plane(const Primary& point1, const Primary& point2, const Primary& point3);

    /**
     * @brief Generates a Combined Plane havin a Primary Point and a Primary Line or two lines that intersect.
     * @param p1 The first primary.
     * @param p2 The second primary.
     * @throws If the shapes of the Primaries is not matching with Point-Line, or Line-Line.
     * @throws If p1 and p2 are lines but their intersection is not a point.
     */
    static Combined Plane(const Primary& p1, const Primary& p2);

    /**
     * @brief Generates a Rectangle
     * @note If the vector1 and vector2 are not perpendicular, the projection of the vector2 will be used insted.
     * @param vertex One of the shape vertices.
     * @param vector1 The vector between the "vertex" and the adjacent vertex.
     * @param vector2 The vector between the "vertex" and other adjacent vertex.
     */
    static Combined Rectangle(const Point3D& vertex, const Vector3D& vector1, const Vector3D& vector2);

    /**
     * @brief Generates a Rectangle
     * @note If the verteces do not create a normal corner, the projection of the vertex3 positionVector will be used insted.
     * @param vertex One of the shape vertices.
     * @param vertex2 The adjacent vertex to the vertex.
     * @param vertex3 The other adjacent vertex to the vertex.
     */
    static Combined Rectangle(const Point3D& vertex, const Point3D& vertex2, const Point3D& vertex3);

    /**
     * @brief Generates a Rhombuss
     * @note If the vector1 lenght will be considered to create the shape.
     * @param vertex One of the shape vertices.
     * @param vector1 The vector between the "vertex" and the adjacent vertex.
     * @param vector2 The vector between the "vertex" and other adjacent vertex.
     */
    static Combined Rhombus(const Point3D& vertex, const Vector3D& vector1, const Vector3D& vector2);

    /**
     * @brief Generates a Rhombuss
     * @note If the verteces do not create a rhombuss, the lenght between vertex and vertex2 will be used.
     * @note The vertex2-vertex lenght will be used to create the square
     * @param vertex One of the shape vertices.
     * @param vertex2 The adjacent vertex to the vertex.
     * @param vertex3 The other adjacent vertex to the vertex.
     */
    static Combined Rhombus(const Point3D& vertex, const Point3D& vertex2, const Point3D& vertex3);

    /**
     * @brief Generates a Regular Polygon.
     * @param centre The centre of the shape.
     * @param planeNormal The normal vector of the plane that shape is located on it.
     * @param radiusVector The vector from centre to one of the vertices.
     * @param numberOfVertices The number of vertices of the shape (min: 3)
     * @throws If the number of vertices is less than 3.
     * @throws If the planeNormal is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined RegularPolygon(const Point3D& centre,
                                   const Vector3D& planeNormal,
                                   const Vector3D& radiusVector,
                                   const unsigned long numberOfVertices);

    /**
     * @brief Generates a Regular Polygon.
     * @param centre The centre of the shape.
     * @param vertex One of the shape vertices
     * @param vectorOnPlane Any vector lying on the plane of the Polygon.
     * @param numberOfVertices The number of vertices of the shape (min: 3)
     * @throws If the number of vertices is less than 3.
     * @throws If the planeNormal calculated to be a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined RegularPolygon(const Point3D& centre,
                                   const Point3D& vertex,
                                   const Vector3D& vectorOnPlane,
                                   const unsigned long numberOfVertices);

    /**
     * @brief Generates a Square.
     * @param centre The centre of the shape.
     * @param planeNormal The normal vector of the plane that shape is located on it.
     * @param radiusVector The vector between one of the vertices and the centre.
     * @throws If the planeNormal is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined Square(const Point3D& centre, const Vector3D& planeNormal,const Vector3D& radiusVector);

    /**
     * @brief Generates a Square.
     * @param centre The centre of the shape.
     * @param vertex One of the shape vertices
     * @param vectorOnPlane Any vector lying on the plane of the Polygon.
     * @throws If the planeNormal calculated to be a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined Square(const Point3D& centre, const Point3D& vertex, const Vector3D& vectorOnPlane);

    /**
     * @brief Generates a Rectangle
     * @note If the verteces do not create a normal corner, the projection of the vertex3 positionVector will be used insted.
     * @note The vertex2-vertex lenght will be used to create the square
     * @param vertex One of the shape vertices.
     * @param vertex2 The adjacent vertex to the vertex.
     * @param vertex3 The other adjacent vertex to the vertex.
     */
    static Combined Square(const Point3D& vertex, const Point3D& vertex2, const Point3D& vertex3);

    /**
     * @brief Generates a Trapezoid.
     * @note The first vector will create the plane, thus if the second vector is not on the plane, its projection of the plane will be used.
     * @param vertex1 The first vertex of one of the parallel sides.
     * @param vertex2 The second vertex of the same parallel side.
     * @param vector1 The vector from p1 to the closest vertex of the other parralel side.
     * @param vector2 The vector from p2 to the closest vertex of the other parralel side.
     */
    static Combined Trapezoid(const Point3D& vertex1, const Point3D& vertex2, const Vector3D& vector1, const Vector3D& vector2);

    /**
     * @brief Generates a Trapezium
     * @param vertex One of the shape vertices.
     * @param vector1 The vector between the "vertex" and the adjacent vertex.
     * @param vector2 The vector between the "vertex" and other adjacent vertex.
     */
    static Combined Trapezium(const Point3D& vertex, const Vector3D& vector1, const Vector3D& vector2);

    /**
     * @brief Generates a Triangle.
     * @param point1 The first vertex.
     * @param point2 The second vertex.
     * @param point3 The third vertex.
     */
    static Combined Triangle(const Point3D& point1, const Point3D& point2, const Point3D& point3);

    ///@ }

    ///@ {
    /**
     * @details Static members providing constructor like experiance for creating 3D Combined geomenties.
     */

    /**
     * @brief Generates a Primary Bar having a point and a parallel vector of its centre line and the radius.
     * @param point A point on the centre line.F
     * @param vector A vector parallel to the bar.
     * @param radius The radius of the bar.
     * @throws If the radius is zero or minus.
     */
    static Combined Bar(const Point3D& point, const Vector3D& vector, double radius);

    /**
     * @brief Generates a Combined Bar having its centre line and the radius.
     * @param line The centre line of the bar.
     * @param radius The radius of the bar.
     * @throws If the radius is zero or minus.
     */
    static Combined Bar(const Primary& line, double radius);

    /**
     * @brief Generates a Cube.
     * @param sideCentre The centre of the one of the faces.
     * @param sideNormalVector The normal vector of the same face.
     * @param radiusVector The vector between the sidecenter and a vertex on the same face.
     * @throws If the radius vector is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined Cube(const Point3D& sideCentre,
                         const Vector3D& sideNormalVector,
                         const Vector3D& radiusVector);

    /**
     * @brief Generates a Cube.
     * @param vertex One of the shape verteces.
     * @param direction1 The direction vector between "vertex" and the first adjacent vertex.
     * @param direction2 The direction vector between "vertex" and the second adjacent vertex.
     * @param dimension The dimension of the cube.
     * @todo Draw the corresponding image on the documentation.
     */
    static Combined Cube(const Point3D& vertex,
                         const Vector3D& direction1,
                         const Vector3D& direction2,
                         const Vector3D& direction3,
                         const double dimension);

    /**
     * @brief Generates a Cuboid.
     * @param vertex One of the shape verteces.
     * @param vector1 The vector between the "vertex" and the first adjacent vertex.
     * @param vector2 The vector between the "vertex" and the second adjacent vertex.
     * @param vector3 The vector between the "vertex" and the third adjacent vertex.
     */
    static Combined Cuboid(const Point3D& vertex,
                           const Vector3D& vector1,
                           const Vector3D& vector2,
                           const Vector3D& vector3);

    /**
     * @brief Generates a Cuboid.
     * @param vertex One of the shape verteces.
     * @param adjacentVertex1 the first adjacent vertex.
     * @param adjacentVertex2 the second adjacent vertex.
     * @param adjacentVertex3 the third adjacent vertex.
     */
    static Combined Cuboid(const Point3D& vertex,
                           const Point3D& adjacentVertex1,
                           const Point3D& adjacentVertex2,
                           const Point3D& adjacentVertex3);

    /**
     * @brief Generates a Cylinder.
     * @param sideCentre1 The centre of the circle at the first side.
     * @param sideCentre2 The centre of the circle at the second side.
     * @param radius the radius of the shape.
     * @throws If the radius is zero or minus.
     * @throws If the side centers are located on eachother.
     */
    static Combined Cylinder(const Point3D& sideCentre1,
                             const Point3D& sideCentre2,
                             Thickness radius);

    /**
     * @brief Generates a EquilateralTrianglePrism.
     * @param sideCentre The centre of the one of the faces.
     * @param sideNormalVector The normal vector of the same face.
     * @param radiusVector The vector between the sidecenter and a vertex on the same face.
     * @param height The height of the prism.
     * @throws If the height is zero or minus.
     * @throws If the radius vector is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined EquilateralTrianglePrism(const Point3D& sideCentre,
                                             const Vector3D& sideNormalVector,
                                             const Vector3D& radiusVector,
                                             const double height);

    /**
     * @brief Generates a Parallelepiped (A combination of six Parallelograms).
     * @param vertex One of the shape verteces.
     * @param vector1 The vector between the "vertex" and the first adjacent vertex.
     * @param vector2 The vector between the "vertex" and the second adjacent vertex.
     * @param vector3 The vector between the "vertex" and the third adjacent vertex.
     */
    static Combined Parallelepiped(const Point3D& vertex,
                                   const Vector3D& vector1,
                                   const Vector3D& vector2,
                                   const Vector3D& vector3);

    /**
     * @brief Generates a Parallelepiped (A combination of six Parallelograms).
     * @param vertex One of the shape verteces.
     * @param adjacentVertex1 the first adjacent vertex.
     * @param adjacentVertex2 the second adjacent vertex.
     * @param adjacentVertex3 the third adjacent vertex.
     */
    static Combined Parallelepiped(const Point3D& vertex,
                                   const Point3D& adjacentVertex1,
                                   const Point3D& adjacentVertex2,
                                   const Point3D& adjacentVertex3);

    /**
     * @brief Generates a Combined Plate having a point, a normal vector and the thickness of the plate.
     * @param point A point on eather side of the plate.
     * @param normalVector The normal vector of the side having the point on it that point to the other side of the Plate.
     * @param thickness The thickness of the Plate.
     */
    static Combined Plate(const Point3D& point, const Vector3D& normalVector, Thickness thickness);

    /**
     * @brief Generates a Combined Plate having the base plane and the thickness of the plate.
     * @note The thickness is in the direction of base plane normal.
     * @param plane The base plane.
     * @param thickness The thickness of the plate.
     */
    static Combined Plate(const Primary& plane, Thickness thickness);

    /**
     * @brief Generates a Pyramid.
     * @param baseCentre The centre of the base.
     * @param baseNormalVector The normal vector of the pyramid base plane.
     * @param tip The tip of the Pyramid.
     * @param radiusVector The vector from one of the side centers to one of the vertices on the same side.
     * @param numberOfVertices The number of vertices of the shape (min: 3)
     * @throws If the radius vector is a zero vector.
     * @throws If the number of vertices is less than 3.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined Pyramid(const Point3D& baseCentre,
                            const Vector3D& baseNormalVector,
                            const Point3D& tip,
                            const Vector3D& radiusVector,
                            unsigned int numberOfVertices);

    /**
     * @brief Generates a RegularPrism (A Prism with two perpendicular RegularPolygon at each end).
     * @param sideCentre1 The centre of the circle at the first side.
     * @param sideCentre2 The centre of the circle at the second side.
     * @param radiusVector The vector from one of the side centers to one of the vertices on the same side.
     * @param numberOfVertices The number of vertices of the shape (min: 3)
     * @throws If the radius vector is a zero vector.
     * @throws If the number of vertices is less than 3.
     * @throws If the side centers are located on eachother.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined RegularPrism(const Point3D& sideCentre1, const Point3D& sideCentre2,
                                 const Vector3D& radiusVector,
                                 const unsigned long numberOfVertices);

    /**
     * @brief Generates a Regular Pyramid.
     * @param baseCentre The centre of the base.
     * @param heightVector The height vector of the pyramid.
     * @param radiusVector The vector from one of the side centers to one of the vertices on the same side.
     * @param numberOfVertices The number of vertices of the shape (min: 3)
     * @throws If the radius vector is a zero vector.
     * @throws If the number of vertices is less than 3.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined RegularPyramid(const Point3D& baseCentre,
                                   const Vector3D& heightVector,
                                   const Vector3D& radiusVector,
                                   unsigned int numberOfVertices);

    /**
     * @brief Generates a Regular Square Pyramid.
     * @param baseCenter The center of the base.
     * @param heightVector The height vector of the pyramid.
     * @param radiusVector The vector from one of the side centers to one of the vertices on the same side.
     * @throws If the radius vector is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined RegularSquarePyramid(const Point3D& baseCentre, const Vector3D heightVector, const Vector3D& radiusVector);

    /**
     * @brief Generates a Regular Tetrahedron.
     * @param baseCentre The centre of the base.
     * @param heightVector The height vector of the Tetrahedron.
     * @param radiusVector The vector from one of the side centers to one of the vertices on the same side.
     * @throws If the radius vector is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined RegularTetrahedron(const Point3D& baseCentre, const Vector3D heightVector, const Vector3D& radiusVector);

    /**
     * @brief Generates a Combined Sphere having the centre and a point located on the surface of the shpere.
     * @param centre The centre of the shpere.
     * @param aPointOnShpere A point located on the surface of the shpere
     * @throws If points are located on each other.
     */
    static Combined Sphere(const Point3D& centre, const Point3D& aPointOnShpere);

    /**
     * @brief Generates a Combined Sphere having the centre and the radius.
     * @param centre The centre of the shpere.
     * @param radius The radius of the shpere.
     * @throws If the radius is zero or minus.
     */
    static Combined Sphere(const Point3D& centre, Thickness radius);

    /**
     * @brief Generates a Square Pyramid.
     * @param baseCentre The centre of the base.
     * @param baseNormalVector The normal vector of the pyramid base plane.
     * @param tip The tip of the Pyramid.
     * @param radiusVector The vector from one of the side centers to one of the vertices on the same side.
     * @throws If the radius vector is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined SquarePyramid(const Point3D& baseCentre, const Vector3D baseNormalVector,
                                  const Point3D& tip, const Vector3D& radiusVector);

    /**
     * @brief Generates a TruncatedCylinder (A Cylinder cutted by one or two plane at the ends)
     * @note Ignoring (nullptr) any of the side vectors will replace them with a normal plane of the Cylinder.
     * @param sideCentre1 The centre of the circle at the first side.
     * @param sideCentre2 The centre of the circle at the second side.
     * @param radius the radius of the shape.
     * @param sideVector1 The cutting plane normal at the first side.
     * @param sideVector2 The cutting plane normal at the second side.
     * @throws If the radius is zero or minus.
     * @throws If the side centers are located on eachother.
     */
    static Combined TruncatedCylinder(const Point3D& sideCentre1, const Point3D& sideCentre2, Thickness radius,
                                      const std::shared_ptr<Vector3D>& sideVector1 = nullptr, const std::shared_ptr<Vector3D>& sideVector2 = nullptr);

    /**
     * @brief Generates a Tetrahedron.
     * @param baseCentre The centre of the base.
     * @param baseNormalVector The normal vector of the pyramid base plane.
     * @param tip The tip of the Pyramid.
     * @param radiusVector The vector from one of the side centers to one of the vertices on the same side.
     * @throws If the radius vector is a zero vector.
     * @throws If the projection of the radius vector on the shape plane is a zero vector.
     */
    static Combined Tetrahedron(const Point3D& baseCentre, const Vector3D baseNormalVector, const Point3D& tip, const Vector3D& radiusVector);

    ///@ }
    ///@ }

    /**
     * @brief Computes the distance between the geometry and a point in 3D space.
     * @note The distance is negative if the point is inside the shape, and it is in its minimum at the centre of the shape.
     * @param point The point in 3D space.
     */
    virtual double distanceTo(const Point3D& point) const override;

    /**
     * @brief Computes whether the geometry is equivalent to an empty space or not.
     * @note If true, you can be sure that the geometry is empty, but if false, there is a chance that the presented shape is empty.
     */
    virtual bool isEmpty() const override;

    /**
     * @brief Gets the light hand side of the geometry.
     */
    const Geometry& lhs() const;

    /**
     * @brief Gets the set operation between two geometry.
     */
    SetOperation operation() const;

    /**
     * @brief Projects a point on the surface of the Geometry.
     * @param point The point to project.
     * @return The projected point on the sruface of the Geometry.
     */
    virtual PointPtr pointProjection(const Point3D& point) const override;

    /**
     * @brief Computes the status of the point in comparison with the Primary geometry.
     * @param point The point.
     */
    virtual PointStatus pointStatus(const Point3D & point) const override;

    /**
     * @brief Gets the right hand side of the geometry.
     */
    const Geometry& rhs() const;

    /**
     * @brief Computes all the vertices (corners) of the geometry.
     */
    virtual std::vector<Point3D> vertices() const override;

    /**
     * @brief Moves the geometry.
     * @param vector The transfer vector.
     */
    virtual void move(const Vector3D& vector) override;

    /**
     * @brief Mirrors the geometry.
     * @param planePosition The mirror plane position vector.
     * @param planeNormal The mirror plane normal vector.
     */
    virtual void mirror(const Point3D& planePosition, const Vector3D& planeNormal) override;

    /**
     * @brief Rotates the geometry.
     * @param rotationCentre The centre of rotation.
     * @param rotationAxis The rotation axis' direction vector (dose not need to be unit vector).
     * @param angle The angle of rotation (rad) using rigth hand role.
     * @note Right hand thummb in direction of the rotation axis, posetive angle is in direction of closing right hand.
     */
    virtual void rotate(const Point3D& rotationCentre, const Vector3D& rotationAxis, double angle) override;

    /**
     * @brief Sets the light hand side of the geometry.
     * @param other The other geometry.
     */
    const Combined& setLhs(const Combined& other);

    /**
     * @brief Sets the light hand side of the geometry.
     * @param other The other geometry.
     */
    const Combined& setLhs(const Primary& other);

    /**
     * @brief Sets the set operation between two geometry.
     * @param operation The set operation.
     */
    const Combined& setOperation(const SetOperation operation);

    /**
     * @brief Sets the right hand side of the geometry.
     * @param other The other geometry.
     */
    const Combined& setRhs(const Combined& other);

    /**
     * @brief Sets the right hand side of the geometry.
     * @param other The other geometry.
     */
    const Combined& setRhs(const Primary& other);

    virtual operator std::string() const override;

protected:

    /**
     * @brief Creates a half 3D space excluding a point in that space.
     * @param plane The plane that the line is located on it.
     * @param linePoint1 The first point of the line.
     * @param linePoint2 The second point of the line.
     * @param excludingPoint The point that should be outside of the halfspace.
     * @throws If the plane is not a Primary::Plane.
     * @throws If the line has zero length.
     */
    static Primary halfSpaceExcludingPoint(const Primary& plane,
                                           const Point3D& linePoint1,
                                           const Point3D& linePoint2,
                                           const Point3D& excludingPoint);
    /**
     * @brief Creates a half 3D space excluding a point in that space.
     * @param plane The plane that the line is located on it.
     * @param linePoint1 The first point of the line.
     * @param linePoint2 The second point of the line.
     * @param includingPoint The point that should be inside of the halfspace.
     * @throws If the plane is not a Primary::Plane.
     * @throws If the line has zero length.
     */
    static Primary halfSpaceIncludingPoint(const Primary& plane,
                                           const Point3D& linePoint1,
                                           const Point3D& linePoint2,
                                           const Point3D& includingPoint);

    /**
     * @brief Computes the primary line-sphere intersection points.
     * @note The static intersection() member of Primary assumes the shapes to be filled, thus the intersection will be a empty.
     * @param line The Primary Line.
     * @param sphere The Primary Sphere.
     */
    static std::vector<Point3D> lineSphereIntersectionPoints(const Primary& line, const Primary& sphere);

    /**
     * @brief Computes the primary line-bar intersection points.
     * @note The static intersection() member of Primary assumes the shapes to be filled, thus the intersection will be a empty.
     * @param line The Primary Line.
     * @param bar The Primary Bar.
     */
    static std::vector<Point3D> lineBarIntersectionPoints(const Primary& line, const Primary& bar);

    /**
     * @brief Extract all the Primaries involving in creation this Combined geometry.
     */
    std::vector<Primary> allPrimaries() const;

private:

    /**
     * @brief Swaps the parameters of this geometry with other Combined geometry.
     * @param other The other Combined geometry.
     */
    void swap(Combined &other);

    FirstGeoPtr  p_first;       // The first geometry (lhs).
    SetOperation p_operation;   // The operation between two geometries.
    SecondGeoPtr p_second;      // The second geometry (rhs).

};

std::ostream &operator << (std::ostream & out, const Combined & geometry);

} // namespace rbs::geometry

#endif // COMBINED_H

//
//  Primary.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 12.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef PRIMARY_H
#define PRIMARY_H

#include "Geometry.h"
#include "../exporting/VTKFile.h"

/**
 * The geometry namespace provides the means to handle geometrical shapes in 3D space space.
 */
namespace rbs::geometry {

class Combined;

/**
 * @brief The Primary class defines any 3D geometry that can be described with a
 *        maximum of two vectors and one scalar. (i.e., empty space, point, line, plane,
 *        sphere, bar, and plate)
 */
class Primary: public Geometry
{
public:

    using Thickness = double;
    using VectorPtr = std::shared_ptr<Vector3D>;

    /**
     * @brief The name of the primary geometries.
     */
    enum class Shape {
        Empty, Point, Line, Plane, Sphere, Bar, Plate
    };

    /**
     * @brief The constructor.
     * @param name The name of the shape.
     * @param positionVector The position vector of the geometry.
     * @param vector The unit/normal vector of the geometry.
     * @param thickness The thickness of the shape.
     * @throws If the lenght of the vector is zero.
     */
    Primary(Shape name, const Point3D & positionVector, const Vector3D & vector, Thickness thickness);

    /**
     * @brief The constructor.
     * @note If any vector provided, it will be normalized.
     * @param name The name of the shape.
     * @param positionVector The position vector of the geometry.
     * @param vector The unit/normal vector of the geometry.
     * @param thickness The thickness of the shape.
     * @throws If the lenght of the vector is zero.
     */
    Primary(Shape name = Shape::Empty,
            const PointPtr & positionVector = nullptr,
            const VectorPtr & vector = nullptr,
            Thickness thickness = 0);

    /**
     * @brief The copy constructor.
     * @param other The other Primary geomentry to copy.
     */
    Primary(const Primary & other);

    /**
     * @brief The move constructor.
     * @param other The other Primary to move.
     */
    Primary(Primary && other);

    /**
     * @brief Move/copy assign operator.
     * @param other The other Primary to move or copy.
     * @return The new Primary.
     */
    Primary &operator =(Primary other);

    ///@ {
    /**
     * @details Static members providing constructor like experiance for creating each type of Primary geomenties.
     * @note The order of the members is not alphabetic. It follows the order of types in Shape enum.
     */

    /**
     * @brief Generates an Empty Primary Shape.
     */
    static Primary Empty();

    /**
     * @brief Generates a Primary Point.
     * @param values The component list.
     * @throws If the values has any size but 3.
     */
    static Primary Point(std::initializer_list<double> values);

    /**
     * @brief Generates a Primary Point having a 3D position vector.
     * @param positionVector The point position vector in 3D space.
     */
    static Primary Point(const Vector3D& positionVector);

    /**
     * @brief Generates a Primary Point located at a point in 3D space.
     * @param point A point locaion in 3D space.
     */
    static Primary Point(const Point3D& point);

    /**
     * @brief Generates apoint at the origin of the coordinate system.
     */
    static Primary Origin();

    /**
     * @brief Generates a Primary Line having a point and a parallel vector to the line.
     * @param point A point on the line.
     * @param vector A vector parallel to the line.
     */
    static Primary Line(const Point3D& point, const Vector3D& vector);

    /**
     * @brief Generates a Primary Line having two points.
     * @param point1 The first point.
     * @param point2 the second point.
     */
    static Primary Line(const Point3D& point1, const Point3D& point2);

    /**
     * @brief Generates a Primary Line having two points.
     * @param point1 The first point.
     * @param point2 the second point.
     * @throws If one of the points has not the Shape::Point.
     */
    static Primary Line(const Primary& point1, const Primary& point2);

    /**
     * @brief Gives the X-Axis Line
     */
    static Primary XAxisLine();

    /**
     * @brief Gives the Y-Axis Line
     */
    static Primary YAxisLine();

    /**
     * @brief Gives the Z-Axis Line
     */
    static Primary ZAxisLine();

    /**
     * @brief Generates a Primary Plane having a point and a normal vector.
     * @param point A point on the plane.
     * @param normalVector The normal vector of plane.
     */
    static Primary Plane(const Point3D& point, const Vector3D& normalVector);

    /**
     * @brief Generates a Primary Plane having three points on the plane.
     * @param point1 The first point.
     * @param point2 the second point.
     * @param point3 the third point.
     */
    static Primary Plane(const Point3D& point1, const Point3D& point2, const Point3D& point3);

    /**
     * @brief Generates a Primary Plane havin three Primary Points.
     * @param point1 The first point.
     * @param point2 the second point.
     * @param point3 the third point.
     * @throws If one of the points has not the Shape::Point.
     */
    static Primary Plane(const Primary& point1, const Primary& point2, const Primary& point3);

    /**
     * @brief Generates a Primary Plane havin a Primary Point and a Primary Line or two lines that intersect.
     * @param p1 The first primary.
     * @param p2 The second primary.
     * @throws If the shapes of the Primaries is not matching with Point-Line, or Line-Line.
     * @throws If p1 and p2 are lines but their intersection is not a point.
     */
    static Primary Plane(const Primary& p1, const Primary& p2);

    /**
     * @brief Gives the XY-Plane
     */
    static Primary XYPlane();

    /**
     * @brief Gives the YX-Plane
     */
    static Primary YXPlane();

    /**
     * @brief Gives the XZ-Plane
     */
    static Primary XZPlane();

    /**
     * @brief Gives the ZX-Plane
     */
    static Primary ZXPlane();

    /**
     * @brief Gives the YZ-Plane
     */
    static Primary YZPlane();

    /**
     * @brief Gives the YX-Plane
     */
    static Primary ZYPlane();

    /**
     * @brief Generates a Primary Sphere having the centre and the radius.
     * @param centre The centre of the shpere.
     * @param radius The radius of the shpere.
     * @throws If the radius is zero or minus.
     */
    static Primary Sphere(const Point3D& centre, double radius);

    /**
     * @brief Generates a Primary Sphere having the centre and a point located on the surface of the shpere.
     * @param centre The centre of the shpere.
     * @param aPointOnShpere A point located on the surface of the shpere
     * @throws If points are located on each other.
     */
    static Primary Sphere(const Point3D& centre, const Point3D& aPointOnShpere);

    /**
     * @brief Generates a Primary Sphere having the centre as Primary Point and the radius.
     * @param centre The centre of the shpere.
     * @param radius The radius of the shpere.
     * @throws If the radius is zero or minus.
     */
    static Primary Sphere(const Primary& point, double radius);

    /**
     * @brief Generates a Primary Sphere having the centre and a point located on the surface of the shpere.
     * @param centre The centre of the shpere.
     * @param aPointOnShpere A point located on the surface of the shpere
     * @throws If points are located on each other.
     */
    static Primary Sphere(const Primary& centre, const Primary& aPointOnShpere);

    /**
     * @brief Generates a Primary that presents the whole of the 3D space.
     */
    static Primary Space();

    /**
     * @brief Generates a Primary Bar having a point and a parallel vector of its centre line and the radius.
     * @param point A point on the centre line.
     * @param vector A vector parallel to the bar.
     * @param radius The radius of the bar.
     * @throws If the radius is zero or minus.
     */
    static Primary Bar(const Point3D& point, const Vector3D& vector, double radius);

    /**
     * @brief Generates a Primary Bar having its centre line and the radius.
     * @param line The centre line of the bar.
     * @param radius The radius of the bar.
     * @throws If the radius is zero or minus.
     */
    static Primary Bar(const Primary& line, double radius);

    /**
     * @brief Generates a Primary Plate having a point, a normal vector and the thickness of the plate.
     * @param point A point on eather side of the plate.
     * @param normalVector The normal vector of the side having the point on it that point to the other side of the Plate.
     * @param thickness The thickness of the Plate.
     */
    static Primary Plate(const Point3D& point, const Vector3D& normalVector, Thickness thickness);

    /**
     * @brief Generates a Primary Plate having the base plane and the thickness of the plate.
     * @note The thickness is in the direction of base plane normal.
     * @param plane The base plane.
     * @param thickness The thickness of the plate.
     */
    static Primary Plate(const Primary& plane, Thickness thickness);

    /**
     * @brief Generates a Primary plate contaning half of 3D space.
     * @param point A point on the age of the space.
     * @param direction The direction of the space that is filled by the half space.
     */
    static Primary HalfSpace(const Point3D& point, const Vector3D& direction) {
        return Plate(point, direction, std::numeric_limits<double>::max());
    }

    /**
     * @brief Generates a Primary geometry by offseting another Primary with an offset vector.
     * @param primary The Primary to offset.
     * @param vector The Offset vector.
     */
    static Primary Offset(const Primary& primary, const Vector3D& vector);

    /**
     * @brief Generates a Primary geometry by transfering another Primary to a new position.
     * @param primary The Primary to transfer.
     * @param position The new position.
     */
    static Primary Transfer(const Primary& primary, const Point3D& position);

    ///@ }

    /**
     * @brief Computes the distance between a Primary geometry and a point in 3D space.
     * @note The distance is negative if the point is inside the shape, and it is in its minimum at the centre of the shape.
     * @param primary The Primary geometry.
     * @param point The point in 3D space.
     */
    static double distance(const Primary& primary, const Point3D& point);

    /**
     * @brief Computes the distance between a Primary geometry and a point in 3D space.
     * @note The distance is negative if the point is inside the shape, and it is in its minimum at the centre of the shape.
     * @param point The point in 3D space.
     * @param primary The Primary geometry.
     */
    static double distance(const Point3D& point, const Primary& primary);

    /**
     * @brief Computes the distance between two Primary geometry.
     * @note Distance may become negative. For example, The distance of the point inside a sphere is equal to the distance of the point to the centre of the sphere minus its radius.
     * @param p1 The first Primary geometry.
     * @param p2 The second Primary geometry.
     */
    static double distance(const Primary& p1, const Primary& p2);

    /**
     * @brief Computes the intersection of two Primary geometry.
     * @note If there is no intersection, a empty space will be the intersection.
     * @throws If the intersection is not possible to be presented by a Primary geometry, a nullptr will be returned.
     * @param p1 The first Primary geometry.
     * @param p2 The second Primary geometry.
     * @return The intersection between two primary geometries or nullptr if its not fisible to present the intersection with Primary geometry.
     */
    static std::shared_ptr<Primary> intersection(const Primary& p1, const Primary& p2);

    /**
     * @brief Whether the point status matches with expectation or not.
     * @param point The point.
     * @param expectedStatus The expected point status in relation to the Primary geometry.
     * @param primary The geometry to check the point status aginst it.
     */
    static bool is(const Point3D & point, PointStatus expectedStatus, const Primary& primary);

    /**
     * @brief Projects a Primary geometry(projectee) on an other Primary geometry(surface).
     * @param projectee The Primary geometry projected to the other geomentry surface.
     * @param surface The Primary geometry that holds the surface of the projecction.
     * @return A Primary geometry (Empty Geometry if no projection), nullptr if the projection cannot be presented by aPrimary geometry.
     */
    static const std::shared_ptr<Primary> projection(const Primary& projectee, const Primary& surface);

    /**
     * @brief Computes the distance between the geometry and a point in 3D space.
     * @note The distance is negative if the point is inside the shape, and it is in its minimum at the centre of the shape.
     * @param point The point in 3D space.
     */
    virtual double distanceTo(const Point3D& point) const override;

    /**
     * @brief Computes whether the geometry is equivalent to an empty space or not.
     */
    virtual bool isEmpty() const override {
        return is(Shape::Empty);
    }

    /**
     * @brief Exports a representation of the Primary geometry's shape to a given VTK file.
     * @param file The VTK file.
     * @param min The position vector of a point of the simulation box with minimum components.
     * @param max The position vector of a point of the simulation box with maximum components.
     */
    void exportTo(exporting::VTKFile file, const Vector3D& min, const Vector3D& max) const;

    /**
     * @brief Exports a representation of the Primary geometry's shape to a given VTK file.
     * @param file The VTK file.
     * @param min The address of the position vector of a point of the simulation box with minimum components.
     * @param max The address of the position vector of a point of the simulation box with maximum components.
     */
    void exportTo(exporting::VTKFile file, const VectorPtr& min = nullptr, const VectorPtr& max = nullptr) const;

    /**
     * @brief Wheter the Primary has a thickness or not.
     */
    bool hasThickness() const;

    /**
     * @brief Whether the Primary presents a specific shape or not.
     * @param shape Any shape.
     */
    bool is(Shape shape) const;

    /**
     * @brief Projects a point on the surface of the Geometry.
     * @param point The point to project.
     * @return The projected point on the sruface of the Geometry, nullptr if empty.
     */
    virtual PointPtr pointProjection(const Point3D& point) const override;

    /**
     * @brief Computes the status of the point in comparison with the Primary geometry.
     * @param point The point.
     */
    virtual PointStatus pointStatus(const Point3D & point) const override;

    /**
     * @brief Gives the position point of the Primary.
     * @throws If the Primary does not possess a position.
     */
    const Point3D & position() const;

    /**
     * @brief Gives the position vector of the Primary.
     * @throws If the Primary does not possess a position.
     */
    const Vector3D & positionVector() const;

    /**
     * @brief Gives the thickness of the Primary.
     */
    Thickness thickness() const;

    /**
     * @brief Gives the unit vector of the Primary.
     * @throws If the Primary does not possess a unit vector.
     */
    const Vector3D & unitVector() const;

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
     * @note Right hand thummb in direction of the rotation axis posetive angle is in direction of closing right hand.
     */
    virtual void rotate(const Point3D& rotationCentre, const Vector3D& rotationAxis, double angle) override;

    /**
     * @brief Changes the location of the shape by changing its point.
     * @param point The point to move the shape to it.
     * @return The shape after applying the change.
     */
    Primary & setPosition(const Point3D & positionPoint);

    /**
     * @brief Changes the location of the shape by changing its point.
     * @param point The point to move the shape to it.
     * @return The shape after applying the change.
     * @throws If the point is not a Shape::Point.
     */
    Primary & setPosition(Primary& positionPoint);

    /**
     * @brief Changes the location of the shape by changing its point.
     * @param point The point to move the shape to it.
     * @return The shape after applying the change.
     */
    Primary & setPositionVector(const Vector3D & positionVector);

    /**
     * @brief Changes the Thickness of the shape.
     * @note This might change the type of the shape.
     * @param thickness The thickness of the
     * @return The shape after applying the change.
     * @throws If the thickness is negative.
     */
    Primary & setThickness(Thickness thickness);

    /**
     * @brief Sets the unit vector of the Primary.
     * @param vector The direction vector.
     * @throws If the vector is zero or Primary dose not required a unit vector.
     */
    Primary & setUnitVector(Vector3D vector);

    /**
     * @brief Computes whether the geometry is equivalent to a single point or not.
     * @param primary The Primary geometry.
     * @param point The Point in 3D space.
     */
    friend inline bool operator == (const Primary& primary, const Point3D & point);
    friend inline bool operator != (const Primary& primary, const Point3D & point);

    /**
     * @brief Computes whether the geometry is equivalent to a single point or not.
     * @param point The Point in 3D space.
     * @param primary The Primary geometry.
     */
    friend inline bool operator == (const Point3D & point, const Primary& primary);
    friend inline bool operator != (const Point3D & point, const Primary& primary);

    /**
     * @brief Computes whether two primary geometries are presenting the same space or not.
     * @note This is not pairwise equality between parameters.
     * @param p1 The first Primary geometry.
     * @param p2 The second Primary geometry.
     */
    friend inline bool operator == (const Primary& p1, const Primary& p2);
    friend inline bool operator != (const Primary& p1, const Primary& p2);

    friend inline Combined operator+(const Primary& lhs, const Primary& rhs);
    friend inline Combined operator-(const Primary& lhs, const Primary& rhs);
    friend inline Combined operator*(const Primary& lhs, const Primary& rhs);
    friend inline Combined operator^(const Primary& lhs, const Primary& rhs);

    virtual operator std::string() const override;

protected:

    /**
     * @brief Gives the intersection point's position vectors with the limits planes.
     * @note Dose not produce require any limits if the shape is Sphere or Point.
     * @param min The position vector of a simulation box's point wiht smallest components.
     * @param max The position vector of a simulation box's point wiht largest components.
     */
    std::vector<std::vector<Vector3D> > elemntsForExporting(const VectorPtr& min, const VectorPtr& max) const;

    /**
     * @brief Defines whether the name of the shape is compatible with its configuration or not.
     * @throws If the name of the shape is not known.
     */
    bool isNameCompatible() const;

    /**
     * @brief Corrects the shape type.
     * @note Change in the configuration of the shape (i.e., thickness and vector) may require type correction. correctType
     * @return The type of the shape after correction.
     */
    Shape correctShapeType();

    /**
     * @brief Swaps the content of this object with an other objec of same type.
     * @param other The other instance.
     */
    void swap(Primary& other);

private:

    /**
     * @brief Computes the intersection of two Primary geometry shaped as Plate.
     * @note If there is no intersection, a empty space will be the intersection.
     * @throws If the intersection is not possible to be presented by a Primary geometry, a nullptr will be returned.
     * @param p1 The first Primary geometry.
     * @param p2 The second Primary geometry.
     * @return The plate intersection between two primary geometries or nullptr if its not fisible to present the intersection with Primary geometry.
     */
    static std::shared_ptr<Primary> platePlateIntersection(const Primary& p1, const Primary& p2);

    PointPtr p_position;        // The point representing the geometry position in 3D space (if any)
    Shape p_shape;              // The shape that geometry presents in 3D space.
    Thickness p_thickness;      // The thickness of the geometry (min zero)
    VectorPtr p_vector;         // The vector of the geometry (if any)

};



inline bool operator ==(const Primary &primary, const Primary::Point3D &point) {
    return primary.is(Primary::Shape::Point)  && primary.p_position->positionVector() == point.positionVector();
}

inline bool operator !=(const Primary &primary, const Primary::Point3D &point) {
    return !operator==(primary, point);
}

inline bool operator ==(const Primary::Point3D &point, const Primary &primary) {
    return operator==(primary, point);
}

inline bool operator !=(const Primary::Point3D &point, const Primary &primary) {
    return !operator==(primary, point);
}

inline bool operator ==(const Primary &p1, const Primary &p2) {
    if (p1.p_shape == p2.p_shape) {
        auto sameVectorStatus = false;
        if (!p1.p_vector && !p1.p_vector) {
            sameVectorStatus = true;
        } else if (p1.p_vector && p2.p_vector && Primary::Vector3D::areParallel(*p1.p_vector, *p2.p_vector)) {
            sameVectorStatus = true;
        }
        if (sameVectorStatus) {
            if (space::isZero(p1.p_thickness)) {
                return Primary::is(*p2.p_position, On, p1);
            } else if (space::areEqual(p1.p_thickness, p2.p_thickness)) {
                if(p1.is(Primary::Shape::Sphere)) {
                    return *p1.p_position == *p2.p_position;
                } else if (p1.is(Primary::Shape::Bar)) {
                    // base line : Line(*p1.p_position, *p1.p_vector))
                    return Primary::is(*p2.p_position, On, Primary::Line(*p1.p_position, *p1.p_vector));
                } else if (p1.is(Primary::Shape::Plate)) {
                    if (Primary::Vector3D::haveSameDirections(*p1.p_vector, *p2.p_vector)) {
                        // base plane : Plane(*p1.p_position, *p1.p_vector))
                        return Primary::is(*p2.p_position, On, Primary::Plane(*p1.p_position, *p1.p_vector));
                    } else {
                        // secondary plane : Plane(*p1.p_position + *p1.p_vector * p1.p_thickness, *p1.p_vector)
                        return Primary::is(*p2.p_position, On, Primary::Plane(*p1.p_position + *p1.p_vector * p1.p_thickness, *p2.p_vector));
                    }
                }
            }
        }
    }
    return false;
}

inline bool operator !=(const Primary &p1, const Primary &p2) {
    return !operator==(p1, p2);
}

std::ostream &operator << (std::ostream & out, const Primary & geometry);

} // namespace rbs::geometry

#endif // PRIMARY_H

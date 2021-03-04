//
//  Combined.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 24.05.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Combined.h"

namespace rbs::geometry {

Combined::Combined(const Primary &first, const SetOperation operation, const Primary &second)
    : Combined(std::make_shared<Primary>(first), operation, std::make_shared<Primary>(second))
{
}

Combined::Combined(const Primary &first, const SetOperation operation, const Combined &second)
    : Combined(std::make_shared<Primary>(first), operation, std::make_shared<Combined>(second))
{
}

Combined::Combined(const Combined &first, const SetOperation operation, const Primary &second)
    : Combined(std::make_shared<Combined>(first), operation, std::make_shared<Primary>(second))
{
}

Combined::Combined(const Combined &first, const SetOperation operation, const Combined &second)
    : Combined(std::make_shared<Combined>(first), operation, std::make_shared<Combined>(second))
{
}

Combined::Combined(const Combined::FirstGeoPtr &first, const SetOperation operation, const Combined::SecondGeoPtr &second)
    : p_first{first}
    , p_operation{operation}
    , p_second{second}
{}

Combined::Combined(const Combined &other)
    : p_operation{other.p_operation}
{
    const auto primary = [](const auto& geometry) { return dynamic_cast<Primary *>(geometry.get()); };
    const auto combined = [](const auto& geometry) { return dynamic_cast<Combined *>(geometry.get()); };
    const auto copyPtr = [&primary, &combined](const std::shared_ptr<Geometry> & from, std::shared_ptr<Geometry>& to) {
        if (from) {
            const auto p = primary(from);
            if (p) {
                to = std::make_shared<Primary>(*p);
            } else {
                const auto c = combined(from);
                if (c)
                    to = std::make_shared<Combined>(*c);
            }
        } else {
            to = nullptr;
        }
    };

    copyPtr(other.p_first, p_first);
    copyPtr(other.p_second, p_second);
}

Combined::Combined(const Primary &other)
    : p_first{std::make_shared<Primary>(other)}
    , p_operation{Union}
    , p_second{nullptr}
{}

Combined::Combined(Combined &&other) {
    p_first = std::move(other.p_first);
    p_operation = std::move(other.p_operation);
    p_second = std::move(other.p_second);
}

Combined::Combined(Primary &&other)
    : p_first{std::make_shared<Primary>(other)}
    , p_operation{Union}
    , p_second{nullptr}
{}

Combined::~Combined()
{}

Combined &Combined::operator =(Primary other) {
    p_first = std::make_shared<Primary>(other);
    p_operation = Union;
    p_second = nullptr;
    return *this;
}

Combined &Combined::operator =(Combined other) {
    swap(other);
    return *this;
}

Combined Combined::Empty() {
    return Primary::Empty();
}

Combined Combined::Origin(){
    return Primary::Origin();
}

Combined Combined::Space() {
    return Primary::Space();
}

Combined Combined::XAxisLine() {
    return Primary::XAxisLine();
}

Combined Combined::XYPlane() {
    return Primary::XYPlane();
}

Combined Combined::XZPlane() {
    return Primary::XZPlane();
}

Combined Combined::YAxisLine() {
    return Primary::XAxisLine();
}

Combined Combined::YXPlane() {
    return Primary::YXPlane();
}

Combined Combined::YZPlane() {
    return Primary::YZPlane();
}

Combined Combined::ZAxisLine() {
    return Primary::XAxisLine();
}

Combined Combined::ZXPlane() {
    return Primary::ZXPlane();
}

Combined Combined::ZYPlane() {
    return Primary::ZYPlane();
}

Combined Combined::Line(const Geometry::Point3D &point, const Geometry::Vector3D &vector){
    return Primary::Line(point, vector);
}

Combined Combined::Line(const Geometry::Point3D &point1, const Geometry::Point3D &point2){
    return Primary::Line(point1, point2);
}

Combined Combined::Line(const Primary &point1, const Primary &point2){
    return Primary::Line(point1, point2);
}

Combined Combined::LineRay(const Geometry::Point3D &point, const Geometry::Vector3D &directionVector){
    return Combined(Primary::Line(point,directionVector),
                    SetOperation::Difference,
                    Primary::HalfSpace(point, -directionVector));
}

Combined Combined::LineSegment(const Geometry::Point3D &p1, const Geometry::Point3D &p2) {
    const auto directionVector = p2.positionVector() - p1.positionVector();
    return Combined(Primary::Line(p1,directionVector),
                    SetOperation::Difference,
                    Combined(Primary::HalfSpace(p1, -directionVector),
                             SetOperation::Union,
                             Primary::HalfSpace(p2, directionVector))
                    );
}

Combined Combined::LineSegment(const Geometry::Point3D &centre, const Geometry::Vector3D &radiusVector) {
    return LineSegment(centre + radiusVector, centre - radiusVector);
}

Combined Combined::Point(std::initializer_list<double> values) {
    return Primary::Point(values);
}

Combined Combined::Point(const Geometry::Vector3D &positionVector)
{
    return Primary::Point(positionVector);
}

Combined Combined::Point(const Geometry::Point3D &point) {
    return Primary::Point(point);
}

Combined Combined::Circle(const Geometry::Point3D &centre, const Geometry::Vector3D &planeNormal, const Geometry::Vector3D &radiusVector) {
    const auto radiusVectorLyingOnPlane = radiusVector.projectionOn(planeNormal.perpendicularUnitVectorInDirectionOF(radiusVector));
    return Combined(Primary::Sphere(centre, radiusVectorLyingOnPlane.length()),
                    SetOperation::Intersection,
                    Primary::Plane(centre, planeNormal));
}

Combined Combined::EquilateralTriangle(const Geometry::Point3D &centre, const Geometry::Vector3D &planeNormal, const Geometry::Vector3D &radiusVector) {
    return RegularPolygon(centre, planeNormal, radiusVector, 3);
}

Combined Combined::EquilateralTriangle(const Geometry::Point3D &centre, const Geometry::Point3D &vertex, const Geometry::Vector3D &vectorOnPlane) {
    return RegularPolygon(centre, vertex, vectorOnPlane, 3);
}

Combined Combined::Parallelogram(const Geometry::Point3D &vertex, const Geometry::Vector3D &vector1, const Geometry::Vector3D &vector2) {
    const auto planeNormal = Vector3D::crossProduct(vector1, vector2);
    if (planeNormal == space::consts::o3D) {
        throw std::runtime_error("Cannot find the normal vector of the plane.");
    }

    const auto plane = Primary::Plane(vertex, planeNormal);
    const auto vertex2 = vertex + vector1;
    const auto vertex3 = vertex + vector2;
    const auto h1 = vector2.projectionOn(Vector3D::crossProduct(planeNormal, vector1).unit()).length();
    const auto h2 = vector1.projectionOn(Vector3D::crossProduct(planeNormal, vector2).unit()).length();

    auto hlf1 = halfSpaceIncludingPoint(plane, vertex, vertex2, vertex3);
    hlf1.setThickness(h1);

    auto hlf2 = halfSpaceIncludingPoint(plane, vertex, vertex3, vertex2);
    hlf2.setThickness(h2);

    return Combined(plane,
                    SetOperation::Intersection,
                    Combined(hlf1, SetOperation::Intersection, hlf2));
}

Combined Combined::Plane(const Geometry::Point3D &point, const Geometry::Vector3D &normalVector) {
    return Primary::Plane(point, normalVector);
}

Combined Combined::Plane(const Geometry::Point3D &point1, const Geometry::Point3D &point2, const Geometry::Point3D &point3) {
    return Primary::Plane(point1, point2, point3);
}

Combined Combined::Plane(const Primary &point1, const Primary &point2, const Primary &point3) {
    return Primary::Plane(point1, point2, point3);
}

Combined Combined::Plane(const Primary &p1, const Primary &p2) {
    return Primary::Plane(p1, p2);
}

Combined Combined::Rectangle(const Geometry::Point3D &vertex, const Geometry::Vector3D &vector1, const Geometry::Vector3D &vector2) {
    const auto heightVector = vector2.projectionOn(vector1.perpendicularUnitVectorInDirectionOF(vector2));
    return Parallelogram(vertex, vector1, heightVector);
}

Combined Combined::Rectangle(const Geometry::Point3D &vertex, const Geometry::Point3D &vertex2, const Geometry::Point3D &vertex3) {
    return Rectangle(vertex, vertex2.positionVector() - vertex.positionVector(), vertex3.positionVector() - vertex.positionVector());
}

Combined Combined::Rhombus(const Geometry::Point3D &vertex, const Geometry::Vector3D &vector1, const Geometry::Vector3D &vector2) {
    return Parallelogram(vertex, vector1, vector2.unit() * vector1.length());
}

Combined Combined::Rhombus(const Geometry::Point3D &vertex, const Geometry::Point3D &vertex2, const Geometry::Point3D &vertex3) {
    return Rhombus(vertex,
                    vertex2.positionVector() - vertex.positionVector(),
                    vertex3.positionVector() - vertex.positionVector());
}

Combined Combined::RegularPolygon(const Geometry::Point3D &centre, const Geometry::Vector3D &planeNormal, const Geometry::Vector3D &radiusVector, const unsigned long numberOfVertices) {
    if (numberOfVertices < 3)
        throw std::out_of_range("Cannot have a RegularPolygon with less than three vertexes.");

    if (planeNormal.isZero())
        throw std::runtime_error("The normal vector cannot be a zero vector.");

    const auto plane = Primary::Plane(centre, planeNormal);
    const auto radiusVectorOnPlane = radiusVector.projectionOn(planeNormal.perpendicularUnitVectorInDirectionOF(radiusVector));
    if(radiusVectorOnPlane.isZero())
        throw std::runtime_error("The radius vector on the plane of the shape cannot be a zero vector.");

    std::vector<Point3D> vertexes;
    const auto angle = 2 * M_PI / numberOfVertices;
    const auto maxAngle = 2 * M_PI - angle / 2;
    for(double angleItr = 0; angleItr < maxAngle; angleItr += angle) {
        vertexes.emplace_back(centre + Vector3D::rotate(radiusVectorOnPlane, planeNormal, angleItr));
        vertexes.back() = plane.pointProjection(vertexes.back())->positionVector();
    }

    std::vector<Primary> plates;
    for(size_t itr = 0; itr + 1 < vertexes.size(); itr++) {
        plates.emplace_back(halfSpaceExcludingPoint(plane, vertexes[itr], vertexes[itr+1], centre));
    }
    plates.emplace_back(halfSpaceExcludingPoint(plane, vertexes[0], vertexes.back(), centre));

    return Combined(plane, SetOperation::Difference,
                    std::reduce(plates.begin(), plates.end(), Combined(Primary::Empty()),
                                [](const auto & init, const auto& value) {
                                    return Combined( init, Union, value);
    }));
}

Combined Combined::RegularPolygon(const Geometry::Point3D &centre, const Geometry::Point3D &vertex, const Geometry::Vector3D &vectorOnPlane, const unsigned long numberOfVertices) {
    const auto radiusVector = vertex.positionVector() - centre.positionVector();
    const auto planeNormal = Vector3D::crossProduct(radiusVector, vectorOnPlane);
    return RegularPolygon(centre, planeNormal, radiusVector, numberOfVertices);
}

Combined Combined::Square(const Geometry::Point3D &centre, const Geometry::Vector3D &planeNormal, const Geometry::Vector3D &radiusVector) {
    return RegularPolygon(centre, planeNormal, radiusVector, 4);
}

Combined Combined::Square(const Geometry::Point3D &centre, const Geometry::Point3D &vertex, const Geometry::Vector3D &vectorOnPlane) {
    return RegularPolygon(centre, vertex, vectorOnPlane, 4);
}

Combined Combined::Square(const Geometry::Point3D &vertex, const Geometry::Point3D &vertex2, const Geometry::Point3D &vertex3) {
    const auto vector1 = vertex2.positionVector() - vertex.positionVector();
    auto vector2 = vertex3.positionVector() - vertex.positionVector();
    vector2 = vector2.projectionOn(vector1.perpendicularUnitVectorInDirectionOF(vector2)).unit() * vector1.length();
    return Rectangle(vertex, vector1, vector2);
}

Combined Combined::Trapezoid(const Geometry::Point3D &vertex1, const Geometry::Point3D &vertex2, const Geometry::Vector3D &vector1, const Geometry::Vector3D &vector2) {
    const auto plane = Primary::Plane(vertex1, vertex2, vertex1 + vector1);
    const auto vector2OnPlane = vector2.projectionOn(plane.unitVector().perpendicularUnitVectorInDirectionOF(vector2));
    const auto parallelogram = Combined::Parallelogram(vertex1, vertex2.positionVector() - vertex1.positionVector(), vector1);
    const auto cutter = halfSpaceExcludingPoint(plane, vertex2, vertex2 + vector2OnPlane, vertex1);
    return Combined(parallelogram, Difference, cutter);
}

Combined Combined::Trapezium(const Geometry::Point3D &vertex, const Geometry::Vector3D &vector1, const Geometry::Vector3D &vector2) {
    const auto v = -vector2.projectionOn(vector1);
    const auto heightVector = vector2.projectionOn(vector1.perpendicularUnitVectorInDirectionOF(vector2));
    const auto plane = Primary::Plane(vertex, Vector3D::crossProduct(vector1, heightVector));
    const auto vertex2 = vertex + vector1;
    const auto vertex4 = vertex2 + v + heightVector;
    const auto halfSpace = halfSpaceExcludingPoint(plane, vertex2, vertex4, vertex);
    return Combined(Parallelogram(vertex, vector1, vector2), Difference, halfSpace);
}

Combined Combined::Triangle(const Geometry::Point3D &point1, const Geometry::Point3D &point2, const Geometry::Point3D &point3) {
    const auto centre = (point1.positionVector() + point2.positionVector() + point3.positionVector()) / 3;
    const auto plane = Primary::Plane(centre, Vector3D::crossProduct(point1.positionVector() - centre, point2.positionVector() - centre));
    const auto halfSpaceFor = [&plane](const Point3D& p1, const Point3D& p2, const Point3D& otherPoint) {
        const auto edgeCentre = (p1.positionVector() + p2.positionVector()) / 2;
        auto halfSpace = Primary::HalfSpace(edgeCentre, Vector3D::crossProduct(p2.positionVector() - p1.positionVector(), plane.unitVector()));
        if (Primary::is(otherPoint, PointStatus::Inside, halfSpace)) {
            halfSpace.setUnitVector(-halfSpace.unitVector());
        }
        return halfSpace;
    };

    return Combined( plane, Difference,
                    Combined( Combined( halfSpaceFor(point1, point2, point3), Union, halfSpaceFor(point1, point3, point2) ),
                             Union, halfSpaceFor(point2, point3, point1) ));
}

Combined Combined::Bar(const Geometry::Point3D &point, const Geometry::Vector3D &vector, double radius) {
    return Primary::Bar(point, vector, radius);
}

Combined Combined::Bar(const Primary &line, double radius) {
    return Primary::Bar(line, radius);
}

Combined Combined::Cube(const Geometry::Point3D &sideCentre, const Geometry::Vector3D &sideNormalVector, const Geometry::Vector3D &radiusVector) {
    const auto dim = radiusVector.length() * sqrt(2);
    return RegularPrism(sideCentre, sideCentre + sideNormalVector * dim, radiusVector, 4);
}

Combined Combined::Cube(const Geometry::Point3D &vertex, const Geometry::Vector3D &direction1, const Geometry::Vector3D &direction2, const Geometry::Vector3D &direction3, const double dimension) {
    const auto vector1 = direction1.unit() * dimension;
    const auto vector2 = direction1.perpendicularUnitVectorInDirectionOF(direction2) * dimension;
    const auto vector3_1 = direction1.perpendicularUnitVectorInDirectionOF(direction3);
    const auto vector3 = direction2.perpendicularUnitVectorInDirectionOF(vector3_1) * dimension;
    return Cuboid(vertex, vector1, vector2, vector3);
}

Combined Combined::Cuboid(const Geometry::Point3D &vertex, const Geometry::Vector3D &vector1, const Geometry::Vector3D &vector2, const Geometry::Vector3D &vector3) {
    const auto vec2Normal = vector2.projectionOn(vector1.perpendicularUnitVectorInDirectionOF(vector2));
    const auto vec3Normal1 = vector3.projectionOn(vector1.perpendicularUnitVectorInDirectionOF(vector3));
    const auto vec3Normal = vec3Normal1.projectionOn(vec2Normal.perpendicularUnitVectorInDirectionOF(vec3Normal1));
    return Parallelepiped(vertex, vector1, vec2Normal, vec3Normal);
}

Combined Combined::Cuboid(const Geometry::Point3D &vertex, const Geometry::Point3D &adjacentVertex1, const Geometry::Point3D &adjacentVertex2, const Geometry::Point3D &adjacentVertex3) {
    const auto vector = [&vertex](const Point3D& adjacentVertex) {
        return adjacentVertex.positionVector() - vertex.positionVector();
    };

    return Cuboid(vertex, vector(adjacentVertex1), vector(adjacentVertex2), vector(adjacentVertex3));
}

Combined Combined::Cylinder(const Geometry::Point3D &sideCentre1, const Geometry::Point3D &sideCentre2, Combined::Thickness radius) {
    const auto heightVector = sideCentre2.positionVector() - sideCentre1.positionVector();
    if (heightVector.isZero()) {
        throw std::runtime_error("Cannot create a cylinder with zero height.");
    }
    const auto bar = Primary::Bar(sideCentre1, heightVector, radius);
    const auto plate = Primary::Plate(sideCentre1, heightVector, heightVector.length());
    return Combined(bar, SetOperation::Intersection, plate);
}

Combined Combined::EquilateralTrianglePrism(const Geometry::Point3D &sideCentre, const Geometry::Vector3D &sideNormalVector, const Geometry::Vector3D &radiusVector, const double height) {
    if (space::isZero(height) || height < 0)
        throw std::out_of_range("The height of the prism cannot be zero or minus.");

    return RegularPrism(sideCentre, sideCentre + sideNormalVector * height, radiusVector, 3);
}

Combined Combined::Parallelepiped(const Geometry::Point3D &vertex, const Geometry::Vector3D &vector1, const Geometry::Vector3D &vector2, const Geometry::Vector3D &vector3) {
    const auto plate = [&vertex](const Vector3D& v1, const Vector3D& v2, const Vector3D& h) {
        auto plate = Primary::HalfSpace(vertex, Vector3D::crossProduct(v1, v2));
        if (!Primary::is(vertex + h, Inside, plate)) {
            plate.setUnitVector(-plate.unitVector());
        }
        plate.setThickness(h.projectionLenghtOn(plate.unitVector()));
        return plate;
    };

    return Combined(Combined(plate(vector1, vector2, vector3),
                             SetOperation::Intersection,
                             plate(vector2, vector3, vector1)),
                    SetOperation::Intersection,
                    plate(vector1, vector3, vector2));
}

Combined Combined::Parallelepiped(const Geometry::Point3D &vertex, const Geometry::Point3D &adjacentVertex1, const Geometry::Point3D &adjacentVertex2, const Geometry::Point3D &adjacentVertex3) {
    const auto vector = [&vertex](const Point3D& adjacentVertex) {
        return adjacentVertex.positionVector() - vertex.positionVector();
    };

    return Parallelepiped(vertex, vector(adjacentVertex1), vector(adjacentVertex2), vector(adjacentVertex3));
}

Combined Combined::Plate(const Geometry::Point3D &point, const Geometry::Vector3D &normalVector, Combined::Thickness thickness) {
    return Primary::Plate(point, normalVector, thickness);
}

Combined Combined::Plate(const Primary &plane, Combined::Thickness thickness) {
    return Primary::Plate(plane, thickness);
}

Combined Combined::Pyramid(const Geometry::Point3D &baseCentre, const Geometry::Vector3D &baseNormalVector, const Geometry::Point3D &tip, const Geometry::Vector3D &radiusVector, unsigned int numberOfVertices) {
    if (numberOfVertices < 3) {
        throw std::out_of_range("Cannot have a pyramid with base vertexes less than three.");
    }

    if (baseNormalVector.isZero())
        throw std::runtime_error("Cannot create a RegularPrism with zero height.");

    const auto halfSpace = [&baseCentre, &tip](const Point3D& p1, const Point3D& p2) {
        auto ans = Primary::Plate(Primary::Plane(p1, p2, tip), std::numeric_limits<double>::max());
        if (ans.pointStatus(baseCentre) != PointStatus::Outside) {
            ans.setUnitVector( -ans.unitVector());
        }
        return ans;
    };

    const auto plane = Primary::Plane(baseCentre, baseNormalVector);
    const auto radiusVectorOnPlane = radiusVector.projectionOn(baseNormalVector.perpendicularUnitVectorInDirectionOF(radiusVector));
    if(radiusVectorOnPlane.isZero())
        throw std::runtime_error("The radius vector on the plane of the shape cannot be a zero vector.");

    std::vector<Point3D> vertexes;
    const auto angle = 2 * M_PI / numberOfVertices;
    const auto maxAngle = 2 * M_PI - angle / 2;
    for(double angleItr = 0; angleItr < maxAngle; angleItr += angle){
        vertexes.emplace_back(baseCentre + Vector3D::rotate(radiusVectorOnPlane, baseNormalVector, angleItr));
        vertexes.back() = plane.pointProjection(vertexes.back())->positionVector();
    }

    std::vector<Primary> cuttingPlates;
    for(size_t itr = 0; itr + 1 < vertexes.size(); itr++) {
        cuttingPlates.emplace_back(halfSpace(vertexes[itr], vertexes[itr+1]));
    }
    cuttingPlates.emplace_back(halfSpace(vertexes[0], vertexes.back()));

    // adding the base half space.
    cuttingPlates.emplace_back(Primary::Plate(Primary::Plane(vertexes[0], vertexes[1], vertexes[2]), std::numeric_limits<double>::max()));
    if (cuttingPlates.back().pointStatus(tip) != PointStatus::Outside) {
        cuttingPlates.back().setUnitVector( -cuttingPlates.back().unitVector());
    }

    return Combined(Primary::Space(), SetOperation::Difference,
                    std::reduce(cuttingPlates.begin(), cuttingPlates.end(), Combined(Primary::Empty()),
                                [](const auto & init, const auto& value) {
                                    return Combined( init, Union, value);
    }));
}

Combined Combined::RegularPrism(const Geometry::Point3D &sideCentre1, const Geometry::Point3D &sideCentre2, const Geometry::Vector3D &radiusVector, const unsigned long numberOfVertices) {
    if (numberOfVertices < 3)
        throw std::out_of_range("Cannot have a RegularPrism with less than three vertexes.");

    const auto heightVector = sideCentre2.positionVector() - sideCentre1.positionVector();
    if (heightVector.isZero())
        throw std::runtime_error("Cannot create a RegularPrism with zero height.");

    const auto plane = Primary::Plane(sideCentre1, heightVector);
    const auto radiusVectorOnPlane = radiusVector.projectionOn(heightVector.perpendicularUnitVectorInDirectionOF(radiusVector));
    if(radiusVectorOnPlane.isZero())
        throw std::runtime_error("The radius vector on the plane of the shape cannot be a zero vector.");

    std::vector<Point3D> vertexes;
    const auto angle = 2 * M_PI / numberOfVertices;
    const auto maxAngle = 2 * M_PI - angle / 2;
    for(double angleItr = 0; angleItr < maxAngle; angleItr += angle){
        vertexes.emplace_back(sideCentre1 + Vector3D::rotate(radiusVectorOnPlane, heightVector, angleItr));
        vertexes.back() = plane.pointProjection(vertexes.back())->positionVector();
    }

    std::vector<Primary> cuttingPlates;
    for(size_t itr = 0; itr + 1 < vertexes.size(); itr++) {
        cuttingPlates.emplace_back(halfSpaceExcludingPoint(plane, vertexes[itr], vertexes[itr+1], sideCentre1));
    }
    cuttingPlates.emplace_back(halfSpaceExcludingPoint(plane, vertexes[0], vertexes.back(), sideCentre1));

    const auto plate = Primary::Plate(sideCentre1, heightVector, heightVector.length());
    return Combined(plate, SetOperation::Difference,
                    std::reduce(cuttingPlates.begin(), cuttingPlates.end(), Combined(Primary::Empty()),
                                [](const auto & init, const auto& value) {
                                    return Combined( init, Union, value);
    }));
}

Combined Combined::RegularPyramid(const Geometry::Point3D &baseCentre, const Geometry::Vector3D &heightVector, const Geometry::Vector3D &radiusVector, unsigned int numberOfVertices) {
    return Pyramid(baseCentre, heightVector, baseCentre + heightVector, radiusVector, numberOfVertices);
}

Combined Combined::RegularSquarePyramid(const Geometry::Point3D &baseCentre, const Geometry::Vector3D heightVector, const Geometry::Vector3D &radiusVector) {
    return RegularPyramid(baseCentre, heightVector, radiusVector, 4);
}

Combined Combined::RegularTetrahedron(const Geometry::Point3D &baseCentre, const Geometry::Vector3D heightVector, const Geometry::Vector3D &radiusVector) {
    return RegularPyramid(baseCentre, heightVector, radiusVector, 3);
}

Combined Combined::Sphere(const Geometry::Point3D &centre, const Geometry::Point3D &aPointOnShpere) {
    return Primary::Sphere(centre, aPointOnShpere);
}

Combined Combined::Sphere(const Geometry::Point3D &centre, Combined::Thickness radius){
    return Primary::Sphere(centre, radius);
}

Combined Combined::SquarePyramid(const Geometry::Point3D &baseCentre, const Geometry::Vector3D baseNormalVector, const Geometry::Point3D &tip, const Geometry::Vector3D &radiusVector) {
    return Pyramid(baseCentre, baseNormalVector, tip, radiusVector, 4);
}

Combined Combined::TruncatedCylinder(const Geometry::Point3D &sideCentre1, const Geometry::Point3D &sideCentre2, Combined::Thickness radius, const std::shared_ptr<Geometry::Vector3D> &sideVector1, const std::shared_ptr<Geometry::Vector3D> &sideVector2) {
    const auto heightVector = sideCentre2.positionVector() - sideCentre1.positionVector();
    if (heightVector.isZero()) {
        throw std::runtime_error("Cannot create a cylinder with zero height.");
    }
    const auto bar = Primary::Bar(sideCentre1, heightVector, radius);
    const auto halfSpace = [](const Point3D& sideCentre,
            const Vector3D& sideVector,
            const Point3D& excludingPoint){
        auto ans = Primary::HalfSpace(sideCentre, sideVector);
        if (ans.pointStatus(excludingPoint) == PointStatus::Inside) {
            ans.setUnitVector(-ans.unitVector());
        }
        return ans;
    };

    if (sideVector1 && sideVector2) {
        const auto cutters = Combined(halfSpace(sideCentre1, *sideVector1, sideCentre2),
                                      SetOperation::Union,
                                      halfSpace(sideCentre2, *sideVector2, sideCentre1));
        return Combined(bar, SetOperation::Difference, cutters);
    } else if (sideVector1) {
        const auto cutters = Combined(halfSpace(sideCentre1, *sideVector1, sideCentre2),
                                      SetOperation::Union,
                                      halfSpace(sideCentre2, bar.unitVector(), sideCentre1));
        return Combined(bar, SetOperation::Difference, cutters);
    } else if (sideVector2) {
        const auto cutters = Combined(halfSpace(sideCentre1, bar.unitVector(), sideCentre2),
                                      SetOperation::Union,
                                      halfSpace(sideCentre2, *sideVector2, sideCentre1));
        return Combined(bar, SetOperation::Difference, cutters);
    }

    const auto cutters = Combined(halfSpace(sideCentre1, bar.unitVector(), sideCentre2),
                                  SetOperation::Union,
                                  halfSpace(sideCentre2, bar.unitVector(), sideCentre1));
    return Combined(bar, SetOperation::Difference, cutters);
}

Combined Combined::Tetrahedron(const Geometry::Point3D &baseCentre, const Geometry::Vector3D baseNormalVector, const Geometry::Point3D &tip, const Geometry::Vector3D &radiusVector) {
    return Pyramid(baseCentre, baseNormalVector, tip, radiusVector, 3);
}

double Combined::distanceTo(const Geometry::Point3D &point) const{
    const auto projection = pointProjection(point);
    if (projection) {
        return (projection->positionVector() - point.positionVector()).length();
    }
    return std::numeric_limits<double>::infinity(); // The distance of any point to an empty space is infinity.
}

bool Combined::isEmpty() const {
    const auto areEqual = [](const std::shared_ptr<Geometry>& first, const std::shared_ptr<Geometry>& second) {
        if (first == second) {
            return true;
        }

        const auto p1 = dynamic_cast<Primary *>(first.get());
        const auto p2 = dynamic_cast<Primary *>(second.get());
        if (p1 && p2) {
            return *p1 == *p2;
        }
        return false;
    };

    switch (p_operation) {
    case SetOperation::Union: {
        if(!p_first && !p_second) {
            return true;
        } else if (!p_first && p_second && p_second->isEmpty()) {
            return true;
        } else if (!p_second && p_first && p_first->isEmpty()) {
            return true;
        } else if (p_first && p_first->isEmpty() && p_second && p_second->isEmpty()) {
            return true;
        }
        break;
    }
    case SetOperation::Difference: {
        if (!p_first || p_first->isEmpty()) {
            return true;
        }  else if (p_first && p_second) {
            return areEqual(p_first, p_second);
        }
        break;
    }
    case SetOperation::Intersection: {
        if (!p_first || p_first->isEmpty()) {
            return true;
        } else if (p_first && p_second) {
            const auto p1 = dynamic_cast<Primary *>(p_first.get());
            const auto p2 = dynamic_cast<Primary *>(p_second.get());
            if (p1 && p2) {
                const auto intersect = Primary::intersection(*p1, *p2);
                if (!intersect || intersect->isEmpty()) {
                    return true;
                }
            }
        }
    }
    case SetOperation::SymmetricDifference: {
        if(!p_first && !p_second) {
            return true;
        } else if (!p_first && p_second && p_second->isEmpty()) {
            return true;
        } else if (!p_second && p_first && p_first->isEmpty()) {
            return true;
        } else if (p_first && p_first->isEmpty() && p_second && p_second->isEmpty()) {
            return true;
        } else if (p_first && p_second) {
            return areEqual(p_first, p_second);
        }
        break;
    }
    default:
        throw std::runtime_error("The SetOperation is not recognized.");
    }
    return false;
}

const Geometry &Combined::lhs() const {
    if(p_first)
        return *p_first;
    throw std::runtime_error("There is no left hand side geomotry set.");
}

SetOperation Combined::operation() const {
    return p_operation;
}

Geometry::PointPtr Combined::pointProjection(const Geometry::Point3D &point) const {
    const auto distVec = [](const Point3D& p1, const Point3D& p2) {
        return p1.positionVector() - p2.positionVector();
    };

    std::vector<PointPtr> projections;

    // Get all the projection points on Primaries
    const auto primaries = allPrimaries();
    std::transform(primaries.begin(), primaries.end(), projections.begin(), [&point](const auto& p) {
        return p.pointProjection(point);
    });
    // Get all the vertices
    const auto allvertices = vertices();
    std::transform(allvertices.begin(), allvertices.end(), projections.end(), [](const auto& p) {
        return std::make_shared<Point3D>(p);
    });

    // Remove all the points that are not a point or are not On the shape!
    projections.erase(std::remove_if(projections.begin(), projections.end(), [&](const auto & p) {
        return !p || pointStatus(*p) != On;
    }),
                      projections.end());

    // remove all dublicates
    projections.erase(std::unique(projections.begin(), projections.end()), projections.end());

    std::sort(projections.begin(), projections.end(), [&](const auto& p1, const auto& p2) {
        return distVec(*p1, point).lengthSquared() < distVec(*p2, point).lengthSquared();
    });

    if (projections.empty()) {
        return nullptr;
    }
    return projections[0];
}

PointStatus Combined::pointStatus(const Geometry::Point3D &point) const {
    if (p_first && p_second) {
        const auto lhsStatus = p_first->pointStatus(point);
        const auto rhsStatus = p_second->pointStatus(point);
        switch(p_operation) {
        case Union: {
            if (lhsStatus == Inside || rhsStatus == Inside) {
                return Inside;
            } else if (lhsStatus == On || rhsStatus == On) {
                return On;
            }
            return Outside;
        }
        case Intersection: {
            if (lhsStatus == Inside && rhsStatus == Inside) {
                return Inside;
            } else if ((lhsStatus == On && rhsStatus != Outside)
                       || (lhsStatus != Outside && rhsStatus == On)) {
                return On;
            }
            return Outside;
        }
        case Difference: {
            if (lhsStatus == Inside && rhsStatus == Outside) {
                return Inside;
            } else if ((lhsStatus == On && rhsStatus == Outside)
                       || (lhsStatus != Outside && rhsStatus == On)) {
                return On;
            }
            return Outside;
        }
        case SymmetricDifference: {
            if ((lhsStatus == Inside && rhsStatus == Outside)
                    || (lhsStatus == Outside && rhsStatus == Inside)) {
                return Inside;
            } else if (lhsStatus == On || rhsStatus == On) {
                return On;
            }
            return Outside;
        }
        default:
            throw std::runtime_error("The SetOperation is not recognized.");
        }
    } else if (p_first) {
        return p_first->pointStatus(point);
    } else if (p_second) {
        return p_second->pointStatus(point);
    }
    return Outside; // Everything is outside of a empty geometry.
}

const Geometry &Combined::rhs() const {
    if(p_second)
        return *p_second;
    throw std::runtime_error("There is no right hand side geomotry set.");
}

std::vector<Geometry::Point3D> Combined::vertices() const {
    std::vector<Point3D> ans;
    auto primaries = allPrimaries();

    const auto newGeometry = [&primaries](const Primary& p) {
        return std::find(primaries.begin(), primaries.end(), p) == primaries.end();
    };

    const auto newVertex = [&ans](const Point3D& p) {
        return std::find(ans.begin(), ans.end(), p) == ans.end();
    };

    for(size_t itr = 0; itr < primaries.size(); itr++) {
        if (primaries[itr].is(Primary::Shape::Plate)) {
            if (primaries[itr].thickness() != std::numeric_limits<double>::max()) {
                primaries.emplace_back(Primary::Plane(primaries[itr].position() + primaries[itr].unitVector() * primaries[itr].thickness(), primaries[itr].unitVector()));
            }
            primaries[itr] = Primary::Plane(primaries[itr].position(), primaries[itr].unitVector());
        }
    }

    size_t p1itr, p2itr;
    for(p1itr = 0; p1itr < primaries.size(); p1itr++) {
        const auto p1 = primaries[p1itr];
        for(p2itr = 0; p2itr < primaries.size(); p2itr++) {
            const auto p2 = primaries[p2itr];
            if (p1 != p2 && !p1.is(Primary::Shape::Point) && !p2.is(Primary::Shape::Point)) {
                auto intersect = Primary::intersection(p1, p2);
                if (intersect && !intersect->isEmpty() && newGeometry(*intersect)) {
                    primaries.emplace_back(*intersect);
                } else if (p1.is(Primary::Shape::Line)) {
                    if (p2.is(Primary::Shape::Sphere)) {
                        const auto points = lineSphereIntersectionPoints(p1, p2);
                        for(const auto& point : points) {
                            primaries.emplace_back(Primary::Point(point));
                        }
                    } else if (p2.is(Primary::Shape::Bar)) {
                        const auto points = lineBarIntersectionPoints(p1, p2);
                        for(const auto& point : points) {
                            primaries.emplace_back(Primary::Point(point));
                        }
                    }
                } else if (p2.is(Primary::Shape::Line)) {
                    if (p1.is(Primary::Shape::Sphere)) {
                        const auto points = lineSphereIntersectionPoints(p2, p1);
                        for(const auto& point : points) {
                            primaries.emplace_back(Primary::Point(point));
                        }
                    } else if (p1.is(Primary::Shape::Bar)) {
                        const auto points = lineBarIntersectionPoints(p2, p1);
                        for(const auto& point : points) {
                            primaries.emplace_back(Primary::Point(point));
                        }
                    }
                }
            }
        }
    }

    for(const auto & p: primaries) {
        if (p.is(Primary::Shape::Point) && pointStatus(p.position()) == On && newVertex(p.position())) {
            ans.emplace_back(p.position());
        }
    }

    return ans;
}

void Combined::move(const Geometry::Vector3D &vector) {
    if (p_first)
        p_first->move(vector);
    if (p_second)
        p_second->move(vector);
}

void Combined::mirror(const Geometry::Point3D &planePosition, const Geometry::Vector3D &planeNormal) {
    if (p_first)
        p_first->mirror(planePosition, planeNormal);
    if (p_second)
        p_second->mirror(planePosition, planeNormal);
}

void Combined::rotate(const Geometry::Point3D &rotationCentre, const Geometry::Vector3D &rotationAxis, double angle) {
    if (p_first)
        p_first->rotate(rotationCentre, rotationAxis, angle);
    if (p_second)
        p_second->rotate(rotationCentre, rotationAxis, angle);
}

const Combined &Combined::setLhs(const Combined &other) {
    p_first = std::make_shared<Combined>(other);
    return *this;
}

const Combined &Combined::setLhs(const Primary &other) {
    p_first = std::make_shared<Primary>(other);
    return *this;
}

const Combined &Combined::setOperation(const SetOperation operation) {
    p_operation = operation;
    return *this;
}

const Combined &Combined::setRhs(const Combined &other) {
    p_second = std::make_shared<Combined>(other);
    return *this;
}

const Combined &Combined::setRhs(const Primary &other) {
    p_second = std::make_shared<Primary>(other);
    return *this;
}

Primary Combined::halfSpaceExcludingPoint(const Primary &plane, const Geometry::Point3D &linePoint1, const Geometry::Point3D &linePoint2, const Geometry::Point3D &excludingPoint) {
    if (!plane.is(Primary::Shape::Plane)) {
        throw std::runtime_error("The plane is not a Primary::Plane.");
    }
    const auto lineVector = linePoint1.positionVector() - linePoint2.positionVector();
    if (space::isZero(lineVector.lengthSquared())) {
        throw std::runtime_error("The ends of the line are located on top of each other.");
    }
    if (Primary::Line(linePoint1, lineVector).pointStatus(excludingPoint) == PointStatus::On) {
        throw std::runtime_error("The excluded point cannot be located on the line.");
    }

    const auto lineCentre = (linePoint1.positionVector() + linePoint2.positionVector()) / 2;
    auto halfSpace = Primary::HalfSpace(lineCentre, Vector3D::crossProduct(lineVector, plane.unitVector()));

    if (Primary::is(excludingPoint, PointStatus::Inside, halfSpace)) {
        halfSpace.setUnitVector(-halfSpace.unitVector());
    }
    return halfSpace;
}

Primary Combined::halfSpaceIncludingPoint(const Primary &plane, const Geometry::Point3D &linePoint1, const Geometry::Point3D &linePoint2, const Geometry::Point3D &includingPoint) {
    if (!plane.is(Primary::Shape::Plane)) {
        throw std::runtime_error("The plane is not a Primary::Plane.");
    }
    const auto lineVector = linePoint1.positionVector() - linePoint2.positionVector();
    if (space::isZero(lineVector.lengthSquared())) {
        throw std::runtime_error("The ends of the line are located on top of each other.");
    }
    if (Primary::Line(linePoint1, lineVector).pointStatus(includingPoint) == PointStatus::On) {
        throw std::runtime_error("The excluded point cannot be located on the line.");
    }

    const auto lineCentre = (linePoint1.positionVector() + linePoint2.positionVector()) / 2;
    auto halfSpace = Primary::HalfSpace(lineCentre, Vector3D::crossProduct(lineVector, plane.unitVector()));

    if (Primary::is(includingPoint, PointStatus::Outside, halfSpace)) {
        halfSpace.setUnitVector(-halfSpace.unitVector());
    }
    return halfSpace;
}

std::vector<Geometry::Point3D> Combined::lineSphereIntersectionPoints(const Primary &line, const Primary &sphere) {
    if (!line.is(Primary::Shape::Line) || !sphere.is(Primary::Shape::Sphere)) {
        throw std::runtime_error("Geometry shapes are not matched.");
    }
    std::vector<Point3D> ans;
    const auto centre = Primary::Point(sphere.position());
    const auto project = Primary::projection(centre, line);
    if (project && project->is(Primary::Shape::Point)) {
        const auto dist = Primary::distance(*project, sphere);
        if( space::isZero(dist) ) {
            ans.emplace_back(project->position());
        } else if (dist < 0) {
            const auto len = sqrt(pow(sphere.thickness(), 2) - pow(sphere.thickness() + dist, 2));
            ans.emplace_back(project->position() + len * line.unitVector());
            ans.emplace_back(project->position() - len * line.unitVector());
        }
    }
    return ans;
}

std::vector<Geometry::Point3D> Combined::lineBarIntersectionPoints(const Primary &line, const Primary &bar) {
    if (!line.is(Primary::Shape::Line) || !bar.is(Primary::Shape::Bar)) {
        throw std::runtime_error("Geometry shapes are not matched.");
    }
    std::vector<Point3D> ans;
    if (Vector3D::areParallel(line.unitVector(), bar.unitVector())) {
        return ans;
    }

    const auto normalPlane = Primary::Plane(line.position(), bar.unitVector());
    const auto barCentreOnNormalPlane = normalPlane.pointProjection(bar.position());
    const auto lineOnNormalPlane = Primary::projection(line, normalPlane);
    if (!barCentreOnNormalPlane) {
        throw std::runtime_error("Cannot find the bar centre on the normal Plane.");
    }
    if (!lineOnNormalPlane || lineOnNormalPlane->is(Primary::Shape::Line)) {
        throw std::runtime_error("Cannot find the line projection on the normal plane.");
    }
    // Up to here we have a plane that cuts the bar with normal angle.
    //                    the centre of the bar intersection (i.e., circle) on the normal plane.
    //                    the projection of the line on the normal plain.
    // Find the points of the circle-projected-line intersection.
    std::vector<Point3D> circlePoints = lineSphereIntersectionPoints(*lineOnNormalPlane,
                                                                     Primary::Sphere(*barCentreOnNormalPlane,
                                                                                     bar.thickness()));
    for(const auto& point : circlePoints) {
        const auto plane = Primary::Plane(point, point.positionVector() - barCentreOnNormalPlane->positionVector());
        const auto intersect = Primary::intersection(plane, line);
        if (intersect && intersect->is(Primary::Shape::Point)) {
            ans.emplace_back(intersect->position());
        }
    }

    return ans;
}

std::vector<Primary> Combined::allPrimaries() const {
    std::vector<Primary> ans;

    const auto append = [&ans](const std::shared_ptr<Geometry>& ptr) {
        if (ptr) {
            Primary * p_ptr = dynamic_cast<Primary *>(ptr.get());
            if (p_ptr) {
                ans.emplace_back(*p_ptr);
                return;
            }

            Combined * c_ptr = dynamic_cast<Combined *>(ptr.get());
            if (c_ptr) {
                const auto primaries = c_ptr->allPrimaries();
                ans.insert(ans.end(), primaries.begin(), primaries.end());
            }
        }
    };

    append(p_first);
    append(p_second);

    return ans;
}

void Combined::swap(Combined &other) {
    std::swap(p_first, other.p_first);
    std::swap(p_operation, other.p_operation);
    std::swap(p_second, other.p_second);
}

geometry::Combined::operator std::string() const{
    static size_t tab = 0;

    std::string ans;
    const auto applyTabs = [&ans](const size_t tab) {
        for(size_t t = 0; t < tab; t++)
            ans+= "  ";
    };

    ans += "CombinedGeometry:\n";

    applyTabs(tab);
    ans += "{\n";

    tab++;
    applyTabs(tab);
    ans += "first: ";
    if (p_first) {
        ans += *p_first;
    } else {
        ans += "NULL";
    }
    tab--;
    ans += "\n";

    applyTabs(tab + 1);
    ans += "operation: ";
    switch (p_operation) {
    case Union:
        ans += "Union (+)";
        break;
    case Intersection:
        ans += "Intersection (^)";
        break;
    case Difference:
        ans += "Difference (-)";
        break;
    case SymmetricDifference:
        ans += "SymmetricDifference (*)";
        break;
    default:
        throw std::runtime_error("The SetOperation is not recognized.");
    }
    ans += "\n";

    tab++;
    applyTabs(tab);
    ans += "second: ";
    if (p_second) {
        ans += *p_second;
    } else {
        ans += "NULL";
    }
    tab--;
    ans += "\n";

    applyTabs(tab);
    ans += "}";
    if (tab == 0)
        ans +="\n";

    return ans;
}

std::ostream &operator << (std::ostream & out, const Combined & geometry) {
    return out << std::string(geometry);
}

} // namespace rbs::geometry

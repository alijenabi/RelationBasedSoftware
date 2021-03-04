//
//  Primary.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 12.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Primary.h"

namespace rbs::geometry {

Primary::Primary(Primary::Shape name, const Primary::Point3D &positionVector, const Primary::Vector3D &vector, Primary::Thickness thickness)
    : Primary(name, std::make_shared<Point3D>(positionVector), std::make_shared<Vector3D>(vector), thickness)
{}

Primary::Primary(Primary::Shape name, const Primary::PointPtr &positionVector, const Primary::VectorPtr &vector, Primary::Thickness thickness)
    : p_position{positionVector}
    , p_shape{name}
    , p_thickness{thickness}
    , p_vector{vector}
{
    correctShapeType();
    if(p_vector)
        p_vector->normalize();
}

Primary::Primary(const Primary &other)
    : p_shape{other.p_shape}
    , p_thickness{other.p_thickness}
{
    p_position = (other.p_position) ? std::make_shared<Point3D>(*other.p_position) : nullptr;
    p_vector = (other.p_vector) ? std::make_shared<Vector3D>(*other.p_vector) : nullptr;
}

Primary::Primary(Primary &&other) {
    p_position = std::move(other.p_position);
    p_shape = std::move(other.p_shape);
    p_thickness = std::move(other.p_thickness);
    p_vector = std::move(other.p_vector);
}

Primary &Primary::operator =(Primary other) {
    swap(other);
    return *this;
}

Primary Primary::Empty() {
    return Primary();
}

Primary Primary::Point(std::initializer_list<double> values) {
    return Point(Point3D(values));
}

Primary Primary::Point(const Primary::Vector3D& positionVector) {
    return Point(Point3D(positionVector));
}

Primary Primary::Point(const Primary::Point3D& point) {
    return Primary(Shape::Point, std::make_shared<Point3D>(point));
}

Primary Primary::Origin() {
    return Point({0, 0, 0});
}

Primary Primary::Line(const Primary::Point3D& point, const Primary::Vector3D& vector) {
    return Primary(Shape::Line, point, vector, 0);
}

Primary Primary::Line(const Primary::Point3D& point1, const Primary::Point3D& point2) {
    return Line(point1, point2.positionVector() - point1.positionVector());
}

Primary Primary::Line(const Primary& point1, const Primary& point2) {
    if (point1.p_shape != Shape::Point || point2.p_shape != Shape::Point)
        throw std::runtime_error("The Primary configuration is not a point.");
    return Line(*point1.p_position, *point2.p_position);
}

Primary Primary::XAxisLine() {
    return Line(Point3D({0, 0, 0}), space::consts::i3D);
}

Primary Primary::YAxisLine() {
    return Line(Point3D({0, 0, 0}), space::consts::j3D);
}

Primary Primary::ZAxisLine() {
    return Line(Point3D({0, 0, 0}), space::consts::k3D);
}

Primary Primary::Plane(const Primary::Point3D& point, const Primary::Vector3D& normalVector) {
    return Primary(Shape::Plane, point, normalVector, 0);
}

Primary Primary::Plane(const Primary::Point3D& point1, const Primary::Point3D& point2, const Primary::Point3D& point3) {
    return Plane(point2, Vector3D::crossProduct(
                     point2.positionVector() - point1.positionVector(),
                     point2.positionVector() - point3.positionVector()));
}

Primary Primary::Plane(const Primary& point1, const Primary& point2, const Primary& point3) {
    if (point1.p_shape != Shape::Point
            || point2.p_shape != Shape::Point
            || point2.p_shape != Shape::Point)
        throw std::runtime_error("The Primary configuration is not a point.");
    return Plane(*point1.p_position, *point2.p_position, *point3.p_position);
}

Primary Primary::Plane(const Primary& p1, const Primary& p2) {
    if (p1.is(Shape::Point) && p2.is(Shape::Line))  {
        return Plane(p1.p_position->positionVector(),
                     p2.p_position->positionVector(),
                     p2.p_position->positionVector() + *p2.p_vector);
    } else if ((p1.is(Shape::Line)) && p2.is(Shape::Point)) {
        return Plane(p2, p1);
    } else if ((p1.is(Shape::Line)) && p2.is(Shape::Line)) {
        const auto intersect = intersection(p1, p2);
        if (intersect && intersect->is(Shape::Point)) {
            return Plane(Point(*intersect->p_position + *p1.p_vector), p2);
        }
        throw std::runtime_error("The Line Primamaries are not intersecting.");
    }
    throw std::runtime_error("The Primary configuration is does not match with the requirements.");
}

Primary Primary::XYPlane() {
    return Plane(Point3D({0, 0, 0}), space::consts::k3D);
}

Primary Primary::YXPlane() {
    return Plane(Point3D({0, 0, 0}), -space::consts::k3D);
}

Primary Primary::XZPlane() {
    return Plane(Point3D({0, 0, 0}), -space::consts::j3D);
}

Primary Primary::ZXPlane() {
    return Plane(Point3D({0, 0, 0}), space::consts::j3D);
}

Primary Primary::YZPlane() {
    return Plane(Point3D({0, 0, 0}), space::consts::i3D);
}

Primary Primary::ZYPlane() {
    return Plane(Point3D({0, 0, 0}), -space::consts::i3D);
}

Primary Primary::Sphere(const Primary::Point3D &centre, double radius) {
    if (space::isZero(radius) || radius < 0)
        throw std::runtime_error("Cannot create a shpere with ziro or mines radius.");
    return Primary(Shape::Sphere, std::make_shared<Primary::Point3D>(centre), nullptr, radius);
}

Primary Primary::Sphere(const Primary::Point3D &centre, const Primary::Point3D &aPointOnShpere) {
    return Sphere(centre, (centre.positionVector() - aPointOnShpere.positionVector()).length());
}

Primary Primary::Sphere(const Primary &point, double radius) {
    if (!point.is(Shape::Point))
        throw std::runtime_error("The Primary configuration is not a point.");
    return Sphere(*point.p_position, radius);
}

Primary Primary::Sphere(const Primary &centre, const Primary &aPointOnShpere) {
    if (!centre.is(Shape::Point) || !aPointOnShpere.is(Shape::Point))
        throw std::runtime_error("The Primary configuration is not a point.");
    return Sphere(*centre.p_position, *aPointOnShpere.p_position);
}

Primary Primary::Space() {
    return Sphere(Point3D({0, 0, 0}), std::numeric_limits<double>::max());
}

Primary Primary::Bar(const Primary::Point3D &point, const Primary::Vector3D &vector, double radius) {
    if (space::isZero(radius) || radius < 0)
        throw std::runtime_error("Cannot create a bar with ziro or mines radius.");
    return Primary(Shape::Bar, point, vector, radius);
}

Primary Primary::Bar(const Primary &line, double radius) {
    if (!line.is(Shape::Line))
        throw std::runtime_error("The Primary configuration is not a line.");
    return Bar(*line.p_position, *line.p_vector, radius);
}

Primary Primary::Plate(const Primary::Point3D &point, const Primary::Vector3D &normalVector, Primary::Thickness thickness) {
    if (thickness < 0)
        return Primary(Shape::Plate, point, -normalVector, -thickness);
    return Primary(Shape::Plate, point, normalVector, thickness);
}

Primary Primary::Plate(const Primary &plane, double thickness) {
    if (!plane.is(Shape::Plane))
        throw std::runtime_error("The Primary configuration is not a plane.");
    return Plate(*plane.p_position, *plane.p_vector, thickness);
}

Primary Primary::Offset(const Primary &primary, const Geometry::Vector3D &vector) {
    Primary ans = primary;
    if (ans.is(Shape::Empty))
        return ans;
    *ans.p_position = *ans.p_position + vector;
    return ans;
}

Primary Primary::Transfer(const Primary &primary, const Geometry::Point3D &position) {
    Primary ans = primary;
    if (ans.is(Shape::Empty))
        return ans;
    *ans.p_position = position;
    return ans;
}

double Primary::distance(const Primary& primary, const Primary::Point3D &point) {
    return distance(primary, Point(point));
}

double Primary::distance(const Primary::Point3D &point, const Primary& primary) {
    return distance(primary, Point(point));
}

double Primary::distance(const Primary& p1, const Primary& p2) {
    if (p1.is(Shape::Empty) || p2.is(Shape::Empty))
        throw std::runtime_error("Cannot define distance to an empyt space.");
    if (p1.p_shape < p2.p_shape)
        return distance(p2, p1);
    // if we have tickness,
    if(p1.hasThickness() || p2.hasThickness()) {
        // if any of the geometries have tickness we need to find the distance of thier bases first.
        Primary temp[2] = {p1, p2};
        temp[0].setThickness(0);
        temp[1].setThickness(0);
        if (p1.is(Shape::Plate)) {
            auto tempDist = distance(temp[0], temp[1]);
            if(space::isZero(tempDist)) {
                return 0;
            }
            if ((temp[1].positionVector() - temp[0].positionVector()).projectionValueOn( *p1.p_vector ) > 0) {
                tempDist -= p1.thickness();
            }
            if(p2.is(Shape::Plate) && (temp[0].positionVector() - temp[1].positionVector()).projectionValueOn( *p2.p_vector ) > 0) {
                return tempDist - p2.thickness();
            }
            return tempDist;
        }
        return distance(temp[0], temp[1]) - p1.thickness() - p2.thickness();
    } else if (p1.is(Shape::Plane)) {
        if (p2.is(Shape::Plane)) {
            // if unit vectors are parallel we have distance otherwise distance is 0
            if (Vector3D::areParallel(*p1.p_vector, *p2.p_vector)) {
                return fabs((p2.positionVector() - p1.positionVector()).projectionValueOn(*p1.p_vector));
            }
            return 0;
        } else if (p2.is(Shape::Line)) {
            // If unit vectors are perpendicular we have distance otherwise distance is 0
            if (Vector3D::arePerpendicular(*p1.p_vector, *p2.p_vector)) {
                return fabs((p2.positionVector() - p1.positionVector()).projectionValueOn(*p1.p_vector));
            }
            return 0;
        } else if (p2.is(Shape::Point)) {
            return fabs((p2.positionVector() - p1.positionVector()).projectionValueOn(*p1.p_vector));
        }
    } else if (p1.is(Shape::Line)) {
        if (p2.is(Shape::Line)) {
            // If two line are parallel then find their distance
            if (Vector3D::areParallel(*p1.p_vector, *p2.p_vector)) {
                return (Vector3D::crossProduct(p2.positionVector() - p1.positionVector(), *p1.p_vector).length() / p1.p_vector->length());
            }
            // If not parallel, then the projection of the position line (p2-p1) on the vector that is normal to both line has a length of their distance.
            // If two lines has an intersect then they lay on a plane which the normal of the plane is normal to both line meaning the distance is 0, as the projection length.
            return fabs((p2.positionVector() - p1.positionVector()).projectionValueOn(Vector3D::crossProduct(*p2.p_vector, *p1.p_vector)));
        } else if (p2.is(Shape::Point))
            return Vector3D::crossProduct((p2.positionVector() - p1.positionVector()), *p1.p_vector).length() / p1.p_vector->length();
    } else if (p1.is(Shape::Point)) {
        if (p2.is(Shape::Point))
            return (p2.positionVector() - p1.positionVector()).length();
    }
    throw std::runtime_error("Unable to compute the distance between two Primary geomentires.");
}

std::shared_ptr<Primary> Primary::intersection(const Primary& p1, const Primary &p2) {
    if (p1.p_shape < p2.p_shape)
        return intersection(p2, p1);

    if (p1 == p2)
        return std::make_shared<Primary>(p1);

    if (p1.is(Shape::Empty) || p2.is(Shape::Empty)) {
        return std::make_shared<Primary>(Empty());
    }

    const auto intersect = [&]() -> std::shared_ptr<Primary>  {
        const auto dist = distance(p1, Point(*p2.p_position));
        if (dist <= - std::min(p1.p_thickness, p2.p_thickness) ) {
            return std::make_shared<Primary>(p2);
        } else if (dist >= p2.p_thickness) {
             return std::make_shared<Primary>(Empty());
        }
        return nullptr;
    };
    const auto pointerTo = [](const Primary & p) {
        return std::make_shared<Primary>(p);
    };

    if (p1.is(Shape::Plate)) {
        if (p2.is(Shape::Plate)) {
            return platePlateIntersection(p1, p2);
        } else if (p2.is(Shape::Plane)) {
            if (Vector3D::areParallel(*p1.p_vector, *p2.p_vector)) {
                if (Primary:: is(*p2.p_position, Outside, p1)) {
                    return std::make_shared<Primary>(Empty());
                } else {
                    return std::make_shared<Primary>(p2);
                }
            }
        } else if (p2.is(Shape::Bar) || p2.is(Shape::Line)) {
            if (Vector3D::arePerpendicular(*p1.p_vector, *p2.p_vector)) {
                return intersect();
            }
        } else if (p2.is(Shape::Sphere) || p2.is(Shape::Point)) {
            return intersect();
        }
    } else if (p1.is(Shape::Bar)) {
        if (p2.is(Shape::Bar) || p2.is(Shape::Line)) {
            if (Vector3D::areParallel(*p1.p_vector, *p2.p_vector)) {
                return intersect();
            }
        } else if (p2.is(Shape::Sphere) || p2.is(Shape::Point)) {
            return intersect();
        }
    } else if (p1.is(Shape::Sphere)) {
        if (p2.is(Shape::Sphere) || p2.is(Shape::Point)) {
            return intersect();
        }
    } else if (p1.is(Shape::Plane)) {
        if (p2.is(Shape::Plane)) {
            if (Vector3D::areParallel(*p1.p_vector, *p2.p_vector)) {
                if (is(*p2.p_position, On, p1)) {
                    return std::make_shared<Primary>(p1);
                }
                return std::make_shared<Primary>(Empty());
            }
            // In case that they are not parallel, we can only have a line at the intersection of two planes.
            // If the position vectors are coinciding or one of them located on "other plane", then we have the intersection line position vector and unit vector as:
            if (is(*p2.p_position, On, p1)) {
                return std::make_shared<Primary>(Line(*p2.p_position, Vector3D::crossProduct(*p1.p_vector, *p2.p_vector)));
            }

            if (is(*p1.p_position, On, p2)) {
                return std::make_shared<Primary>(Line(*p1.p_position, Vector3D::crossProduct(*p1.p_vector, *p2.p_vector)));
            }

            // Any random line on p2 should intersect on a p1 face at the line intersection.
            auto randomLineVector = p1.p_vector->perpendicularUnitVectorInDirectionOF(*p2.p_vector);
            if (Vector3D::arePerpendicular(randomLineVector, p2.unitVector()))
                randomLineVector = p1.p_vector->perpendicularUnitVectorInDirectionOF(space::consts::i3D);

            const auto randomLine = Line(*p1.p_position, randomLineVector);
            auto intersect = intersection(randomLine, p2);
            if (intersect && intersect->is(Shape::Point))
                return std::make_shared<Primary>(Line(*intersect->p_position, Vector3D::crossProduct(*p1.p_vector, *p2.p_vector)));

            throw std::runtime_error("If the planes are not parrallel, they should of have a line as intersection.");
        } else if (p2.is(Shape::Line)) {
            const auto project = projection(p2, p1);
            if (project) {
                if (project->is(Shape::Point)) {
                    return project;
                } else if (project->is(Shape::Line)) {
                    if (Vector3D::areParallel(*p2.p_vector, *project->p_vector)) {
                        if (is(*project->p_position, On, p2)) {
                            return std::make_shared<Primary>(p2);
                        }
                        return nullptr;
                    }
                    return intersection(p2, *project);
                }
            }
            throw std::runtime_error("We should not end up here!!!");
        } else if (p2.is(Shape::Point)) {
            if (is(*p2.p_position, On, p1)) {
                return std::make_shared<Primary>(p2);
            }
            return std::make_shared<Primary>(Empty());
        }
    } else if (p1.is(Shape::Line)) {
        if (p2.is(Shape::Line)) {
            if (Vector3D::areParallel(*p1.p_vector, *p2.p_vector)) {
                if (is(*p2.p_position, On, p1)) {
                    return std::make_shared<Primary>(p1);
                }
                return std::make_shared<Primary>(Empty());
            }
            // Are line skew?
            const auto dist = distance(p1, p2);
            if(space::isZero(dist)) {
                if (*p1.p_position == *p2.p_position) {
                    return pointerTo(Point(*p1.p_position));
                }
                // The intersection has to be a point!
                // First, find the ratio of the distance vector (h) and the projection of my unit vector on it!
                // We use the mirror to avoid the situation that my unit vector is perpendicular to the difference vector (note: the lines cannot be parallel at this stage).
                const auto p1PosOnP2 = projection(Point(*p1.p_position), p2);
                if (p1PosOnP2) {
                    const auto heightVector = p1PosOnP2->positionVector() - p1.positionVector();
                    const auto rasio = heightVector.length() / p1.p_vector->projectionValueOn(heightVector);
                    // Since this ratio does not have a direction, find the direction and report the point of intersection.
                    if (distance(p2, (*p1.p_position) - rasio * (*p1.p_vector)) < distance(p2, (*p1.p_position) + rasio * (*p1.p_vector))) {
                        return std::make_shared<Primary>(Point((*p1.p_position) - rasio * (*p1.p_vector)));
                    }
                    return std::make_shared<Primary>(Point((*p1.p_position) + rasio * (*p1.p_vector)));
                }
                throw std::runtime_error("We should not end up here!!!");
            }
            return std::make_shared<Primary>(Empty());
        } else if (p2.is(Shape::Point)) {
            if (is(*p2.p_position, On, p1)) {
                return std::make_shared<Primary>(p2);
            }
            return std::make_shared<Primary>(Empty());
        }
    } else if (p1.is(Shape::Point) && p2.is(Shape::Point)) {
        if (p1.positionVector() == p2.positionVector()) {
            return std::make_shared<Primary>(p1);
        }
        return std::make_shared<Primary>(Empty());
    }
    return nullptr; // <= cannot be presented by a Primary geometry.
}

bool Primary::is(const Primary::Point3D &point, PointStatus expectedStatus, const Primary &primary) {
    return primary.pointStatus(point) == expectedStatus;
}

const std::shared_ptr<Primary> Primary::projection(const Primary& projectee, const Primary &surface) {
    if (surface.is(Shape::Empty) || projectee.is(Shape::Empty))
        return std::make_shared<Primary>(Empty());

    const auto difVec = [&]() {
        return projectee.positionVector() - surface.positionVector();
    };
    const auto pointerTo = [](const Primary & p) {
        return std::make_shared<Primary>(p);
    };

    // If projectee covers the surface object, then return the surface as projection!
    if (surface.is(Shape::Point) ) {
        return pointerTo(surface);
    } else if (surface.p_shape <= projectee.p_shape) {
        if (surface.is(Shape::Line)) {
            if (projectee.is(Shape::Line) || projectee.is(Shape::Bar)) {
                if (Vector3D::arePerpendicular(*projectee.p_vector, *surface.p_vector)) {
                    return intersection(projectee, surface);
                }
            } else if (projectee.is(Shape::Plane) || projectee.is(Shape::Plate)) {
                if (Vector3D::areParallel(*projectee.p_vector, *surface.p_vector)) {
                    return intersection(projectee, surface);
                }
            }
            return pointerTo(surface);
        } else if (surface.is(Shape::Plane)) {
            if (projectee.is(Shape::Plane) || projectee.is(Shape::Plate)) {
                if (Vector3D::arePerpendicular(*projectee.p_vector, *surface.p_vector)) {
                    return intersection(projectee, surface);
                }
                return pointerTo(surface);
            }
        }
    } else if (projectee.is(Shape::Point)) {
        if (*projectee.p_position == *surface.p_position) {
            return pointerTo(Point(*surface.p_position));
        } else if (surface.is(Shape::Line)) {
            return pointerTo(Point(*surface.p_position + difVec().projectionOn(*surface.p_vector)));
        } else if (surface.is(Shape::Plane)) {
            const auto dif = difVec();
            if (Vector3D::areParallel(dif, *surface.p_vector)) {
                return pointerTo(Point(*surface.p_position));
            } else {
                return pointerTo(Point(*surface.p_position + dif.projectionOn(surface.p_vector->perpendicularUnitVectorInDirectionOF(dif))));
            }
        } else if (surface.is(Shape::Sphere)) {
            const auto dif = difVec();
            return pointerTo(Point(*surface.p_position + surface.p_thickness * dif.unit()));
        } else if (surface.is(Shape::Bar)) {
            const auto dif = difVec();
            if (Vector3D::areParallel(dif, *surface.p_vector)) {
                return pointerTo(Empty());
            } else {
                return pointerTo(Point(*surface.p_position + dif.projectionOn(*surface.p_vector) + surface.p_thickness * dif.projectionOn(surface.p_vector->perpendicularUnitVectorInDirectionOF(dif)).unit()));
            }
        } else if (surface.is(Shape::Plate)) {
            const auto basePlane = Plane(*surface.p_position, *surface.p_vector);
            const auto pointOnBase = projection(projectee, basePlane);
            if (pointOnBase) {
                const auto secondPlane = Plane(*surface.p_position + (*surface.p_vector * surface.p_thickness), *surface.p_vector);
                const auto pointOnSecond = projection(projectee, secondPlane);
                if (pointOnSecond) {
                    if (distance(basePlane, *pointOnBase) < distance(secondPlane, *pointOnSecond)) {
                        return pointOnBase;
                    } else {
                        return pointOnSecond;
                    }
                }
            }
        }
    } else if (projectee.is(Shape::Line) && surface.is(Shape::Plane)) {
        // If the line is perpendicular to the plane we have one point as the projection
        if (Vector3D::areParallel(*projectee.p_vector, *surface.p_vector)) {
            return projection(Point(*projectee.p_position), surface);
        }
        // Otherwise find two points of the projection line.
        const auto p1 = projection(Point(*projectee.p_position), surface);
        const auto p2 = projection(Point(*projectee.p_position + *projectee.p_vector), surface);
        if (p1 && p2 && p1->is(Shape::Point) && p2->is(Shape::Point)) {
            if (p1->positionVector() == p2->positionVector()) {
                return p1;
            }
            return pointerTo(Line(*p1->p_position, *p2->p_position));
        }
    }
    // Other options are not posible to be presented by a Primary geometry.
    return nullptr;
}

double Primary::distanceTo(const Geometry::Point3D &point) const {
    return distance(*this, point);
}

void Primary::exportTo(exporting::VTKFile file, const Primary::Vector3D &min, const Primary::Vector3D &max) const {
    exportTo(file, std::make_shared<Vector3D>(min), std::make_shared<Vector3D>(max));
}

void Primary::exportTo(exporting::VTKFile file, const Primary::VectorPtr &min, const Primary::VectorPtr &max) const {

    if (!is(Shape::Point) && !is(Shape::Sphere) && (!min || !max))
        throw std::runtime_error("Exporing Line requires max and min vectors.");

    if (is(Shape::Point)) {
        const Vector3D v = p_position->positionVector();
        file.appendCell(exporting::VTKCell::Vertex, {exporting::VTKFile::Point({v[0], v[1], v[2]})});
    } else if (is(Shape::Line)) {
        const auto points = elemntsForExporting(min, max)[0];
        if (points.size() >= 2) {
            // If the points are all outside the limits, the line does not cross the simulation box; thus, it will not be exported.
            // Note some of the very close lines to the simulation box may not be drown by this due to double precision.
            file.appendCell(exporting::VTKCell::Line, {
                                exporting::VTKFile::Point({points[0][0], points[0][1], points[0][2]}),
                                exporting::VTKFile::Point({points[1][0], points[1][1], points[1][2]})
                            });
        }
    } else if (is(Shape::Plane)) {
        const auto points = elemntsForExporting(min, max)[0];
        std::vector<exporting::VTKFile::Point> vtkPoints;
        for (const auto & point : points) {
            vtkPoints.push_back({point[0], point[1], point[2]});
        }
        file.appendCell(exporting::VTKCell::Polygon, vtkPoints);
    } else if (is(Shape::Sphere)) {
        const auto elements = elemntsForExporting(min, max);
        for(const auto & points : elements) {
            std::vector<exporting::VTKFile::Point> vtkPoints;
            for (const auto & point : points) {
                vtkPoints.push_back({point[0], point[1], point[2]});
            }
            file.appendCell(exporting::VTKCell::QuadraticPyramid, vtkPoints);
        }
    } else if (is(Shape::Bar)) {
        const auto elements = elemntsForExporting(min, max);
        for(const auto & points : elements) {
            std::vector<exporting::VTKFile::Point> vtkPoints;
            for (const auto & point : points) {
                vtkPoints.push_back({point[0], point[1], point[2]});
            }
            file.appendCell(exporting::VTKCell::QuadraticWedge, vtkPoints);
        }
    } else if (is(Shape::Plate)) {
        const auto basePlane = Plane(*p_position, *p_vector);
        const auto coverPlane = Plane(*p_position + *p_vector * p_thickness, *p_vector);
        const auto basePoints = basePlane.elemntsForExporting(min, max);
        const auto coverPoints = coverPlane.elemntsForExporting(min, max);
        if (!basePlane.elemntsForExporting(min, max).empty()
                && !coverPlane.elemntsForExporting(min, max).empty()) {
            basePlane.exportTo(file, min, max);
            coverPlane.exportTo(file, min, max);
        }
    }
}

bool Primary::hasThickness() const {
    return !space::isZero(p_thickness);
}

bool Primary::is(Primary::Shape shape) const {
    return p_shape == shape;
}

Geometry::PointPtr Primary::pointProjection(const Geometry::Point3D &point) const {
    const auto p = projection(Primary::Point(point), *this);
    if (p) {
        if (p->is(Shape::Point)) {
            return std::make_shared<Point3D>(p->position());
        } else if (p->is(Shape::Empty)) {
            return nullptr;
        }
    }
    throw std::runtime_error("Cannot find the projection of the point on this geometry.");
}

PointStatus Primary::pointStatus(const Primary::Point3D &point) const {
    const auto distance = Primary::distance(*this, point);
    if (space::isZero(distance))
        return PointStatus::On;
    if (distance < 0) {
        return PointStatus::Inside;
    }
    return PointStatus::Outside;
}

const Primary::Point3D &Primary::position() const {
    if(p_position)
        return *p_position;
    throw std::runtime_error("Primary geometry does not possess a position.");
}

const Primary::Vector3D &Primary::positionVector() const {
    if(p_position)
        return p_position->positionVector();
    throw std::runtime_error("Primary geometry does not possess a position.");
}

Primary::Thickness Primary::thickness() const {
    return p_thickness;
}

const Primary::Vector3D &Primary::unitVector() const {
    if(p_vector)
        return *p_vector;
    throw std::runtime_error("Primary geometry does not possess a unit vector.");
}

std::vector<Geometry::Point3D> Primary::vertices() const {
    std::vector<Point3D> ans;
    switch (p_shape) {
    case Shape::Empty:
    case Shape::Line:
    case Shape::Plane:
    case Shape::Sphere:
    case Shape::Bar:
    case Shape::Plate:
        break;
    case Shape::Point:
        ans.emplace_back(*p_position);
        break;
    }
    return ans;
}

void Primary::move(const Geometry::Vector3D &vector) {
    if (p_position)
        *p_position += vector;
}

void Primary::mirror(const Geometry::Point3D &planePosition, const Geometry::Vector3D &planeNormal) {
    const auto mirrorPlane = Primary::Plane(planePosition, planeNormal);
    if(p_position) {
        const auto pointProjection = Primary::projection(Primary::Point(*p_position), mirrorPlane);
        if (pointProjection && pointProjection->is(Shape::Point)) {
            *p_position += (pointProjection->positionVector() - p_position->positionVector()) * 2;
        } else {
            throw std::runtime_error("Could not find the mirror of the point on the mirror plane.");
        }
    }
    if (p_vector) {
        if (!Vector3D::areParallel(*p_vector, planeNormal)) {
            const auto vectorProjection = p_vector->projectionOn(planeNormal.perpendicularUnitVectorInDirectionOF(*p_vector));
            const auto angle = Vector3D::angleBetween(*p_vector, vectorProjection);
            const auto projectionDirection = (p_vector->projectionValueOn(planeNormal) > 0)? 1 : -1;
            const auto rotationAxis = Vector3D::crossProduct(projectionDirection * planeNormal, *p_vector);
            *p_vector = Vector3D::rotate(*p_vector, rotationAxis, 2 * angle);
        } else {
            *p_vector = -(*p_vector);
        }
    }
}

void Primary::rotate(const Geometry::Point3D &rotationCentre, const Geometry::Vector3D &rotationAxis, double angle) {
    const auto rotateRelatively = [&](const Vector3D& reletivePosition) {
        return Vector3D::rotate(reletivePosition, rotationAxis, angle);
    };
    if (p_position) {
        const auto positionAfterRotation = rotateRelatively(p_position->positionVector() - rotationCentre.positionVector())
                                             + rotationCentre.positionVector();
        if (p_vector) {
            const auto vectorTipAfterRotation = rotateRelatively(p_position->positionVector() + *p_vector - rotationCentre.positionVector())
                                                  + rotationCentre.positionVector();
            *p_vector = (vectorTipAfterRotation - positionAfterRotation).unit();
        }
        *p_position = positionAfterRotation; // += since the computed value is relative to the centre.
    }
}

Primary &Primary::setPosition(const Primary::Point3D &positionPoint) {
    p_position = std::make_shared<Point3D>(positionPoint);
    correctShapeType();
    return *this;
}

Primary &Primary::setPosition(Primary& positionPoint) {
    positionPoint.correctShapeType();
    if (positionPoint.p_shape != Shape::Point)
        throw std::runtime_error("The Primary configuration is not a point.");
    return setPosition(*positionPoint.p_position);
}

Primary &Primary::setPositionVector(const Primary::Vector3D &positionVector) {
    p_position = std::make_shared<Point3D>(positionVector);
    correctShapeType();
    return *this;
}

Primary &Primary::setThickness(Primary::Thickness thickness) {
    if (thickness < 0 && !space::isZero(thickness))
        throw std::out_of_range("Thickness cannot be negative.");
    p_thickness = abs(thickness);
    correctShapeType();
    return *this;
}

Primary &Primary::setUnitVector(Primary::Vector3D vector) {
    p_vector = std::make_shared<Vector3D>(vector.unit());
    correctShapeType();
    return *this;
}

geometry::Primary::operator std::string() const {
    const auto thickness = [](const Thickness thickness) {
        const auto infinity = std::numeric_limits<double>::max();
        return (space::areEqual(infinity, thickness))? " infinity" : std::to_string(thickness);
    };

    std::string ans = "PrimaryGeometry:";
    switch (p_shape) {
    case Shape::Empty:
        return ans + "Empty{}";
    case Shape::Point:
        return ans + "Point{on:" + std::string(*p_position) + "}";
    case Shape::Sphere:
        return ans + "Sphere{on:" + std::string(*p_position) +
                ", radius:" + thickness(p_thickness) + "}";
    case Shape::Line:
        return ans + "Line{on:" + std::string(*p_position) +
                ", unit vector:" + std::string(*p_vector) + "}";
    case Shape::Bar:
        return ans + "Bar{on:" + std::string(*p_position) +
                ", unit vector:" + std::string(*p_vector) +
                ", radius:" + thickness(p_thickness) + "}";
    case Shape::Plane:
        return ans + "Plane{on:" + std::string(*p_position) +
                ", normal vector:" + std::string(*p_vector) + "}";
    case Shape::Plate:
        return ans + "Plate{on:" + std::string(*p_position) +
                ", normal vector:" + std::string(*p_vector) +
                ", thickness:" + thickness(p_thickness) + "}";
    }
    throw std::runtime_error("The shape of the Primary geometry is unknown.");
}

std::vector<std::vector<Primary::Vector3D> > Primary::elemntsForExporting(const Primary::VectorPtr &min, const Primary::VectorPtr &max) const {
    std::vector<std::vector<Vector3D> > elements;

    Point3D p1, p2;
    std::vector<Primary> planes;
    if (min && max) {
        p1 = Point3D(*min);
        p2 = Point3D(*max);
        planes = {
            Plane(p1, space::consts::i3D),
            Plane(p1, space::consts::j3D),
            Plane(p1, space::consts::k3D),
            Plane(p2, space::consts::i3D),
            Plane(p2, space::consts::j3D),
            Plane(p2, space::consts::k3D)
        };
    }

    const auto insideLimits = [&](const auto& vec) {
        for (int dir = 0; dir < 3; dir++) {
            if (space::areEqual((*min)[dir], vec[dir]) || space::areEqual((*max)[dir], vec[dir]) ) {
                continue;
            } else if ( vec[dir] < (*min)[dir] || (*max)[dir] < vec[dir]) {
                return false;
            }
        }
        return true;
    };

    switch (p_shape) {
    case Shape::Empty:
        break;
    case Shape::Point: {
        elements.push_back({p_position->positionVector()});
        break;
    }
    case Shape::Line: {
        std::vector<Vector3D> points;
        for (const auto & plane : planes) {
            const auto intersect = intersection(plane, *this);
            if (intersect && intersect->is(Shape::Point)) {
                points.push_back(intersect->positionVector());
            }
        }
        std::sort(points.begin(), points.end(), [](const auto &first, const auto& second) {
            return first.lengthSquared() < second.lengthSquared();
        });
        points.erase(std::unique(points.begin(), points.end()), points.end());
        points.erase(std::remove_if(points.begin(), points.end(), [&insideLimits](const auto & item) {return !insideLimits(item); }), points.end());
        elements.push_back(points);
        break;
    }
    case Shape::Plane: {
        std::vector<Primary> lines;
        for (const auto & plane : planes) {
            const auto intersect = intersection(plane, *this);
            if (intersect && intersect->is(Shape::Line)) {
                if (std::find(lines.begin(), lines.end(), *intersect) == lines.end()) {
                    lines.push_back(*intersect);
                }
            }
        }
        std::vector<Vector3D> points;
        for (const auto & line : lines) {
            const auto lineElements = line.elemntsForExporting(min, max);
            for(const auto& element : lineElements)
                for(const auto& newPoint : element )
                    if (std::find(points.begin(), points.end(), newPoint) == points.end()) {
                        points.push_back(newPoint);
                    }
        }

        if (points.size() == 3) {
            elements.push_back(points);
            break;
        }

        const auto anInnerPoint = std::reduce(points.begin(), points.end(), space::consts::o3D, [](const auto & init, const auto& value){ return init + value; }) / points.size();

        std::vector<Vector3D> pointsInLocalCS;
        pointsInLocalCS.reserve(points.size());
        for(const auto& point: points) {
            const auto localPoint = point - anInnerPoint;
            if (std::find(pointsInLocalCS.begin(), pointsInLocalCS.end(), localPoint) == pointsInLocalCS.end()) {
                pointsInLocalCS.push_back(localPoint);
            }
        }
        points.clear();

        Vector3D ax, ay;
        try {
            if (Vector3D::areParallel(*p_vector, space::consts::i3D))
                throw "Try an other direction!";
            ax = p_vector->perpendicularUnitVectorInDirectionOF(space::consts::i3D);
        } catch (...) {
            ax = p_vector->perpendicularUnitVectorInDirectionOF(space::consts::j3D);
        }
        ay = ax | *p_vector;
        const auto angle = [&ax, &ay](const auto & vec) {
            const double sign = (vec * ay > 0)? 1 : -1;
            return sign * std::acos((vec * ax) / vec.length());
        };
        std::sort(pointsInLocalCS.begin(), pointsInLocalCS.end(), [&angle](const auto &first, const auto& second) {
            return angle(first) < angle(second);
        });
        std::transform(pointsInLocalCS.begin(), pointsInLocalCS.end(), pointsInLocalCS.begin(), [&anInnerPoint](const auto & item) {return item + anInnerPoint;});
        elements.push_back(pointsInLocalCS);
        break;
    }
    case Shape::Sphere: {
        const auto quad = [&] (const auto& i1, const auto& i2, const auto& i3) {
            std::vector<Vector3D> points;
            points.push_back(p_position->positionVector() + i1 * p_thickness);
            points.push_back(p_position->positionVector() + i2 * p_thickness);
            points.push_back(p_position->positionVector() + i3 * p_thickness);
            points.push_back(p_position->positionVector() + i3 * p_thickness);
            points.push_back(p_position->positionVector());
            points.push_back(p_position->positionVector() + (i1 + i2).unit() * p_thickness);
            points.push_back(p_position->positionVector() + (i2 + i3).unit() * p_thickness);
            points.push_back(p_position->positionVector() + i3 * p_thickness);
            points.push_back(p_position->positionVector() + (i1 + i3).unit() * p_thickness);
            points.push_back(p_position->positionVector() + i1 * p_thickness / 2);
            points.push_back(p_position->positionVector() + i2 * p_thickness / 2);
            points.push_back(p_position->positionVector() + i3 * p_thickness / 2);
            points.push_back(p_position->positionVector() + i3 * p_thickness / 2);
            return points;
        };
        elements.push_back(quad( space::consts::i3D,  space::consts::j3D,  space::consts::k3D));
        elements.push_back(quad( space::consts::j3D, -space::consts::i3D,  space::consts::k3D));
        elements.push_back(quad(-space::consts::i3D, -space::consts::j3D,  space::consts::k3D));
        elements.push_back(quad(-space::consts::j3D,  space::consts::i3D,  space::consts::k3D));

        elements.push_back(quad( space::consts::i3D,  space::consts::j3D, -space::consts::k3D));
        elements.push_back(quad( space::consts::j3D, -space::consts::i3D, -space::consts::k3D));
        elements.push_back(quad(-space::consts::i3D, -space::consts::j3D, -space::consts::k3D));
        elements.push_back(quad(-space::consts::j3D,  space::consts::i3D, -space::consts::k3D));
        break;
    }
    case Shape::Bar: {
        Vector3D i1, i2;
        if (Vector3D::areParallel(*p_vector, space::consts::i3D)) {
            i1 = p_vector->perpendicularUnitVectorInDirectionOF(space::consts::j3D);
            i2 = p_vector->perpendicularUnitVectorInDirectionOF(space::consts::k3D);
        } else if (Vector3D::areParallel(*p_vector, space::consts::j3D)) {
            i1 = p_vector->perpendicularUnitVectorInDirectionOF(space::consts::k3D);
            i2 = p_vector->perpendicularUnitVectorInDirectionOF(space::consts::i3D);
        } else {
            i1 = p_vector->perpendicularUnitVectorInDirectionOF(space::consts::i3D);
            i2 = p_vector->perpendicularUnitVectorInDirectionOF(space::consts::j3D);
        }

        std::shared_ptr<Vector3D> centre;
        const auto element = [&](const auto& i1, const auto& i2) {
            std::vector<Primary> lines = {
                Line(*p_position + i1 * p_thickness, *p_vector),
                Line(*p_position, *p_vector),
                Line(*p_position + i2 * p_thickness, *p_vector),
                Line(*p_position + i1 * p_thickness / 2, *p_vector),
                Line(*p_position + i2 * p_thickness / 2, *p_vector),
                Line(*p_position + (i1 + i2).unit() * p_thickness, *p_vector),
            };

            if (!centre) {
                const auto centerLinePoints = lines[1].elemntsForExporting(min, max)[0];
                if (centerLinePoints.size() >= 2) {
                    centre = std::make_shared<Vector3D>((centerLinePoints[0] + centerLinePoints[1]) / 2);
                } else {
                    throw "The centre lines is outside of the limits => cannot draw thid bar inside the limits.";
                }
            }

            std::vector< std::vector<Vector3D> > linePoints;
            for (const auto& line : lines){
                const auto points = line.elemntsForExporting(min, max)[0];
                if (points.size() == 2) {
                    if ((points[0] - *centre) * (*p_vector) > 0) {
                        linePoints.push_back(points);
                    } else {
                        linePoints.push_back({points[1], points[0]});
                    }
                } else {
                    throw "one of the lines is outside of the limits => cannot draw thid bar inside the limits.";
                }
            }
            return std::vector<Vector3D>{
                linePoints[0][0], linePoints[1][0], linePoints[2][0],
                        linePoints[0][1], linePoints[1][1], linePoints[2][1],
                        linePoints[3][0], linePoints[4][0], linePoints[5][0],
                        linePoints[3][1], linePoints[4][1], linePoints[5][1]
            };
        };

        try {
            elements.push_back(element( i1,  i2));
            elements.push_back(element( i2, -i1));
            elements.push_back(element(-i1, -i2));
            elements.push_back(element(-i2,  i1));
        } catch (...) {
            elements.clear();
            return elements;
        }
        break;
    }
    case Shape::Plate:
    default:
        throw std::runtime_error("The current shape is not recognized.");
    }
    return elements;
}

bool Primary::isNameCompatible() const {
    switch (p_shape) {
    case Shape::Empty:
        return space::isZero(p_thickness) && !p_position && !p_vector;
        break;
    case Shape::Point:
        return space::isZero(p_thickness) && p_position && !p_vector;
        break;
    case Shape::Plane:
    case Shape::Line:
        return space::isZero(p_thickness) && p_position && p_vector;
        break;
    case Shape::Sphere:
        return !space::isZero(p_thickness) && p_position && !p_vector;
        break;
    case Shape::Plate:
    case Shape::Bar:
        return !space::isZero(p_thickness) && p_position && p_vector;
        break;
    default:
        break;
    }
    throw std::out_of_range("The name of the Primary is not known.");
}

Primary::Shape Primary::correctShapeType() {
    if (!isNameCompatible()) {
        if (!p_position) {
            p_shape = Shape::Empty;
            return p_shape;
        }

        if (!p_vector || p_vector->isZero()) {
            if (is(Shape::Line) || is(Shape::Bar) || is(Shape::Plane) || is(Shape::Plate)) {
                throw std::runtime_error("Cannot convert the shape type!");
            }
            p_shape = (space::isZero(p_thickness))? Shape::Point : Shape::Sphere;
            p_vector = nullptr;
        }

        if (is(Shape::Line) || is(Shape::Bar)) {
            p_shape = (space::isZero(p_thickness))? Shape::Line : Shape::Bar;
        } else if (is(Shape::Plane) || is(Shape::Plate)) {
            p_shape = (space::isZero(p_thickness))? Shape::Plane : Shape::Plate;
        }
    }
    return p_shape;
}

void Primary::swap(Primary &other) {
    std::swap(p_position, other.p_position);
    std::swap(p_shape, other.p_shape);
    std::swap(p_thickness, other.p_thickness);
    std::swap(p_vector, other.p_vector);
}

std::shared_ptr<Primary> Primary::platePlateIntersection(const Primary& p1, const Primary &p2) {
    if (!p1.is(Shape::Plate) || !p2.is(Shape::Plate))
        throw std::runtime_error("Primary shape is not plate!");

    if (Vector3D::areParallel(*p1.p_vector, *p2.p_vector)) { // <= The intersection can be presented by a Primary geometry.
        const auto p11 = *p1.p_position;
        const auto p12 = (*p1.p_position) + p1.p_thickness * (*p1.p_vector);
        const auto p21 = *p2.p_position;
        const auto p22 = (*p2.p_position) + p2.p_thickness * (*p2.p_vector);
        const bool p11inP2 = !Primary::is(p11, Outside, p2);
        const bool p12inP2 = !Primary::is(p12, Outside, p2);
        const bool p21inP1 = !Primary::is(p21, Outside, p1);
        const bool p22inP1 = !Primary::is(p22, Outside, p1);

        if (!p11inP2 && !p12inP2) {
            return std::make_shared<Primary>(Empty());
        } else if (p11inP2 && p12inP2) {
            return std::make_shared<Primary>(p1);
        } else if (p21inP1 && p22inP1) {
            return std::make_shared<Primary>(p2);
        } else if (p11inP2 && p21inP1) {
            const auto vec = (p21.positionVector() - p11.positionVector()).projectionOn(*p1.p_vector);
            return std::make_shared<Primary>(Plate(*p1.p_position, vec, vec.length()));
        } else if (p11inP2 && p22inP1) {
            const auto vec = (p22.positionVector() - p11.positionVector()).projectionOn(*p1.p_vector);
            return std::make_shared<Primary>(Plate(*p1.p_position, vec, vec.length()));
        } else if (p12inP2 && p21inP1) {
            const auto vec = (p12.positionVector() - p21.positionVector()).projectionOn(*p2.p_vector);
            return std::make_shared<Primary>(Plate(*p2.p_position, vec, vec.length()));
        } else if (p12inP2 && p22inP1) {
            const auto vec = (p12.positionVector() - p22.positionVector()).projectionOn(*p2.p_vector);
            return std::make_shared<Primary>(Plate(p22, vec, vec.length()));
        }
        return std::make_shared<Primary>(Empty());
    }
    return nullptr;
}

std::ostream &operator <<(std::ostream &out, const Primary &geometry) {
    return out << std::string(geometry);
}

} // namespace rbs::geometry




//
//  geometry.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 10.10.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "coordinate_system/CoordinateSystem.h"
#include "coordinate_system/grid.h"
#include "exporting/VTK.h"
#include "geometry/Combined.h"
#include "report/Logger.h"

using namespace rbs;

/**
 * @brief Presents how to work with the geometry namespace.
 * @note Examples are suitable for clang compiler. Running them suing other compilers may require modification.
 * @note Examples are suitable for mac file-system. Running them on other operating systems may require modification.
 * @return EXIT_SUCCESS if all examples are build successfuly, EXIT_FAILURE otherwise.
 */
int main() {

    { // Static constructors
        auto Point_1 = geometry::Primary::Point({2, 1.3, 1});
        auto Point_2 = geometry::Primary::Point(space::Vector<3>{1.2, 3, 5});
        auto Point_3 = geometry::Primary::Point(space::Point<3>{0, 2.3, 5});
        auto origin = geometry::Primary::Origin();

        auto Line_1 = geometry::Primary::Line(space::Point<3>{2, 1.3, 1}, space::Vector<3>{-0.8, 1.7, 4});
        auto Line_2 = geometry::Primary::Line(space::Point<3>{2, 1.3, 1}, space::Point<3>{1.2, 3, 5});
        auto Line_3 = geometry::Primary::Line(Point_1, Point_2);
        auto Line_4 = geometry::Primary::Line(Point_2, Point_1);

        auto XAxis = geometry::Primary::XAxisLine();
        auto YAxis = geometry::Primary::YAxisLine();
        auto ZAxis = geometry::Primary::ZAxisLine();

        const auto normalVector =  space::Vector<3>::crossProduct(space::Vector<3>{-1, -1, 4}, space::Vector<3>{-0.8, 1.7, 4});
        auto Plane_1 = geometry::Primary::Plane(space::Point<3>{0, 2.3, 5}, normalVector);
        auto Plane_2 = geometry::Primary::Plane(space::Point<3>{2, 1.3, 1}, space::Point<3>{1.2, 3, 5}, space::Point<3>{0, 2.3, 5});
        auto Plane_3 = geometry::Primary::Plane(Point_3, Line_1);
        auto Plane_4 = geometry::Primary::Plane(Line_1, Point_3);

        auto XYPlane = geometry::Primary::XYPlane();
        auto YXPlane = geometry::Primary::YXPlane();
        auto YZPlane = geometry::Primary::YZPlane();
        auto ZYPlane = geometry::Primary::ZYPlane();
        auto XZPlane = geometry::Primary::XZPlane();
        auto ZXPlane = geometry::Primary::ZXPlane();

        const auto lineOnPlane = geometry::Primary::Line(Point_1, Point_3);
        auto Plane_5 = geometry::Primary::Plane(Line_1, lineOnPlane);
        auto Plane_6 = geometry::Primary::Plane(lineOnPlane, Line_1);

        auto Shpere_1 = geometry::Primary::Sphere(space::Point<3>{2, 1.3, 1}, 5.5);

        auto Sphere_2 = geometry::Primary::Sphere(space::Point<3>{2, 1.3, 1}, space::Point<3>{2, 1.3, 6.5});
        auto Shpere_3 = geometry::Primary::Sphere(Point_1, 5.5);

        const auto pointOnSphere = geometry::Primary::Point(Point_1.positionVector() + space::Vector<3>{0, 0, 5});
        auto Sphere_4 = geometry::Primary::Sphere(Point_1, pointOnSphere);

        auto Bar_1 = geometry::Primary::Bar(space::Point<3>{2, 1.3, 1}, space::Vector<3>{-0.8, 1.7, 4}, 2);
        auto Bar_2 = geometry::Primary::Bar(Line_1, 2);

        auto Plate_1 = geometry::Primary::Plate(space::Point<3>{0, 2.3, 5}, normalVector, 1.);
        auto Plate_2 = geometry::Primary::Plate(XYPlane, 1);
        auto Plate_3 = geometry::Primary::Plate(YXPlane, 1);
        auto Plate_4 = geometry::Primary::Plate(geometry::Primary::Plane({0, 1, 0}, {0, 0, 1}), -1);

        auto Space = geometry::Primary::Space();
        auto HalfSpace = geometry::Primary::HalfSpace({0, 0, 0}, {0, 0, 1}); // Denotes the space of all points with positive Z in a cartesian coordinate system.
    }

    { // Ellipsoid
        using namespace coordinate_system;
        auto& globalCS = CoordinateSystem::Global();
        auto& localCS = globalCS.appendCustomLocal({0, 0, 0},                                    // origin
                                                   {2, 0, 0}, {0, 1, 0}, {0, 0, 1},              // i, j, k vectors
                                                   convertors::cartesian::toCartesian(),         // mapping
                                                   convertors::cartesian::toCartesianInverse()); // inverse mapping

        auto sphere = geometry::Primary::Sphere({0, 0, 0}, 2.5);

        grid::cartesian::uniformAroundOrigin(space::consts::one3D * 0.05, space::consts::one3D * sphere.thickness() + 0.05, localCS.axes());

        localCS.include([sphere](const space::Point<3>& point) {
            return sphere.pointStatus(point) != geometry::PointStatus::Outside;
        });

        const auto& localPoints = localCS.getAllPoints();

        const auto path = "/<An existing folder path for exporting ellipsoid>/";
        auto file_local = exporting::VTKFile(path, "local", "vtk");

        file_local.appendCell(exporting::vtk::Cell::PolyVertex, exporting::vtk::convertors::convertToVertexes(localPoints));
        file_local.assemble();

        auto file_global = exporting::VTKFile(path, "global", "vtk");
        for(const auto& localPoint : localPoints) {
            const auto globalPoint = globalCS.convert(localPoint, localCS);
            file_global.appendCell(exporting::vtk::Cell::Vertex, exporting::vtk::convertors::convertToVertexes({globalPoint}));
        }
        file_global.assemble();
    }

    { // Elliptical Paraboloid Plate
        using Point = space::Point<3>;
        using Vector = space::Vector<3>;

        const auto mapping =  [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
            const auto& positionVector = point.positionVector(); // The position vector of the point in parent coordinate system.
            return Point{
                positionVector[0],
                positionVector[1],
                0
            };
        };

        const auto inverseMapping = [](const Point& point, const Point& origin, const Vector& i, const Vector& j, const Vector& k) {
            const auto& positionVector = point.positionVector(); // The position vector of the point in child coordinate system.
            return Point{
                positionVector[0],
                positionVector[1],
                (pow(positionVector[0], 2) + pow(positionVector[1] / 2, 2))
            };
        };

        using namespace coordinate_system;
        auto& globalCS = CoordinateSystem::Global();
        auto& localCS = globalCS.appendCustomLocal(space::consts::o3D,
                                                   space::consts::i3D, space::consts::j3D, space::consts::k3D,
                                                   mapping, inverseMapping);

        const auto plateThickness = 0.04;
        auto plate = geometry::Primary::Plate({0, 0, -plateThickness / 2}, space::consts::k3D, plateThickness);

        grid::cartesian::uniformAroundOrigin(space::consts::one3D * 0.02, {5, 10, plateThickness}, localCS.axes());

        localCS.include([plate](const space::Point<3>& point) {
            return plate.pointStatus(point) != geometry::PointStatus::Outside;
        });

        const auto& localPoints = localCS.getAllPoints();

        const auto path = "An existing folder path for exporting elliptical_paraboloid_plate";
        auto file_local = exporting::VTKFile(path, "local", "vtk");

        file_local.appendCell(exporting::vtk::Cell::PolyVertex, exporting::vtk::convertors::convertToVertexes(localPoints));
        file_local.assemble();

        auto file_global = exporting::VTKFile(path, "global", "vtk");
        for(const auto& localPoint : localPoints) {
            const auto globalPoint = globalCS.convert(localPoint, localCS);
            file_global.appendCell(exporting::vtk::Cell::Vertex, exporting::vtk::convertors::convertToVertexes({globalPoint}));
        }
        file_global.assemble();
    }

    return EXIT_SUCCESS;
}

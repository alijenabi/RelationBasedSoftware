//
//  CoordinateSystemExamples.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 10.10.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "variables/Variant.h"
#include "coordinate_system/CoordinateSystem.h"
#include "coordinate_system/grid.h"
#include "exporting/VTK.h"
#include "geometry/Combined.h"
#include <iostream>
#include <vector>

using namespace rbs;
using namespace rbs::exporting;
using namespace rbs::coordinate_system;
using namespace std;

/**
 * @brief Presents examples of how to work with CoordinateSystems.
 * @note Examples are suitable for clang compiler. Running them suing other compilers may require modification.
 * @note Examples are suitable for mac file-system. Running them on other operating systems may require modification.
 * @return EXIT_SUCCESS if all examples are build successfuly, EXIT_FAILURE otherwise.
 */
int main() {

    rbs::variables::Variant v(2);

    cout << "has value: " << ((v.hasValue())? "yes" : "no") << "\n\n";

    using Vector3D = space::Vector<3>;
    using Point3D  = space::Point<3>;

    cout << "\n\n======================================< Global Coordinate System >=====================================\n\n";

    cout << "The global coordinate system (GCS) is a Singleton. Thus:\n";
    const auto& GCS = CoordinateSystem::Global();
    cout << "1st call address: " << &GCS << endl;
    cout << "2nd call address: " << &CoordinateSystem::Global() << "\n\n";
    cout << "The CS is " << ((GCS.isGlobal()) ? "global" : "local") << " coordinate system.\n";

    CoordinateSystem::Global().include(Point3D{2, 2, 0});

    CoordinateSystem::Global().include(Point3D{3, 3, 0});
    CoordinateSystem::Global().include(Point3D{3, 2, 0});
    CoordinateSystem::Global().include(Point3D{3, 1, 0});

    CoordinateSystem::Global().include(Point3D{2, 1, 0});

    CoordinateSystem::Global().include(Point3D{1, 1, 0});
    CoordinateSystem::Global().include(Point3D{1, 2, 0});
    CoordinateSystem::Global().include(Point3D{1, 3, 0});

    CoordinateSystem::Global().include(Point3D{2, 3, 0});

    cout << CoordinateSystem::tree() << endl;

    CoordinateSystem::Global().include(Point3D{0, 0, 0});
    cout << CoordinateSystem::tree() << endl;

    CoordinateSystem::Global().include(Point3D{10, 10, 10});
    cout << CoordinateSystem::tree() << endl;

    CoordinateSystem::Global().include(Point3D{2.5, 1.5, -0.5});
    cout << CoordinateSystem::tree() << endl;

    CoordinateSystem::Global().exclude(Point3D{0, 0, 0});
    CoordinateSystem::Global().exclude(Point3D{10, 10, 10});
    CoordinateSystem::Global().exclude(Point3D{2.5, 1.5, -0.5});
    cout << CoordinateSystem::tree() << endl;

    cout << "\n\n=============================================< clearing >==============================================\n\n";

    cout << "Removing the global coordiante system (GCS) points.\n";
    CoordinateSystem::Global().clearPoints();
    cout << CoordinateSystem::tree() << endl;

    cout << "Removing the global coordiante system (GCS) points and grids.\n";
    CoordinateSystem::Global().clearGrids();
    cout << CoordinateSystem::tree() << endl;

    cout << "\n\n=======================================< Local Coordinate System >=====================================\n\n";
    cout << "Adding a local cartesian coordinate system exactly matched with the GCS.\n";
    auto& cs_cartesian = CoordinateSystem::Global().appendLocal(CoordinateSystem::Cartesian, {1,0,0}, {0,1,0}, {0,0,1});

    cout << "Adding a local cylindrical coordinate system exactly matched with the GCS.\n";
    auto& cs_cylindrical = CoordinateSystem::Global().appendLocal(CoordinateSystem::Cylindrical, {1,0,0}, {0,1,0}, {0,0,1});

    cout << "Adding a local cartesian coordinate system to the added local cylindrical coordinate system.\n";
    auto& cs_cyl_cartesian = cs_cylindrical.appendLocal(CoordinateSystem::Cartesian, {1,0,0}, {1, grid::toRad(90) ,0}, {0,0,1});

    cout << "Adding a local cylindrical coordinate system to the added local cylindrical coordinate system.\n";
    auto& cs_cyl_cylindrical = cs_cylindrical.appendLocal(CoordinateSystem::Cylindrical, {1,0,0}, {1, grid::toRad(90) ,0}, {0,0,1});

    cout << "Adding a local spherical coordinate system to the added local cylindrical coordinate system.\n";
    auto& cs_cyl_spherical = cs_cylindrical.appendLocal(CoordinateSystem::Spherical, {1,0,0}, {1, grid::toRad(90) ,0}, {0,0,1});

    cout << "Adding a local spherical coordinate system exactly matched with the GCS.\n";
    auto& cs_spherical = CoordinateSystem::Global().appendLocal(CoordinateSystem::Spherical, {1,0,0}, {0,1,0}, {0,0,1});

    cout << "Adding a local cartesian coordinate system to the added local spherical coordinate system.\n";
    auto& cs_sph_cartesian = cs_spherical.appendLocal(CoordinateSystem::Cartesian, {1, 0, grid::toRad(90)}, {1, grid::toRad(90), grid::toRad(90)}, {1, 0, 0});

    cout << "Adding a local cylindrical coordinate system to the added local spherical coordinate system.\n";
    auto& cs_sph_cylindrical = cs_spherical.appendLocal(CoordinateSystem::Cylindrical, {1, 0, grid::toRad(90)}, {1, grid::toRad(90), grid::toRad(90)}, {1, 0, 0});

    cout << "Adding a local spherical coordinate system to the added local spherical coordinate system.\n";
    auto& cs_sph_spherical = cs_spherical.appendLocal(CoordinateSystem::Spherical, {1, 0, grid::toRad(90)}, {1, grid::toRad(90), grid::toRad(90)}, {1, 0, 0});

    cout << CoordinateSystem::tree() << endl;

    cout << "\n\n==============================================< meching >==============================================\n\n";
    cout <<     "==============================================< GCS::cartesian\n";

    clock_t t_start;
    const auto printExacutionTime = [&t_start](const unsigned item = 1, const string name = "item" ) {
        const auto t_duration = clock() - t_start - 1;
        cout << ". \n  ==> executed within " << t_duration << " CPU Clock" << "(i.e., " << scientific;
        const auto timeInSec = (double)(t_duration) / CLOCKS_PER_SEC;
        if (timeInSec > 1)
            cout << timeInSec << defaultfloat << " seconds).\n";
        else if(timeInSec * 1000 > 1)
            cout << timeInSec * 1000 << defaultfloat << " milliseconds).\n";
        else if(timeInSec * 1e6 > 1)
            cout << timeInSec * 1e6 << defaultfloat << " microseconds).\n";
        else
            cout << timeInSec * 1e9 << defaultfloat << " nanoseconds).\n";

        if (item > 1) {
            cout << "  ==> " << t_duration / item << " CPU Clock per " << name << ".\n";
        }
        cout << "\n";
    };

    const auto path = "/<An existing folder path>/";
    const auto exportToVTK = [&path, &printExacutionTime](const string folder, const string fileName, const string name, const auto& points) {
        cout << "Exporting "+ name +"s \"" + fileName + "\"\n";
        auto file = vtk::File(path + folder, fileName, "vtk");
        file.appendCell(exporting::VTKCell::PolyVertex, exporting::vtk::convertors::convertToVertexes(points));
        file.assemble(true);
        printExacutionTime(CoordinateSystem::Global().gridSize(true), name);
    };

    cout << "\n\nMeshing the cartesian coordinate system with uniform mesh around the origin\n";
    cout << "    with a distance vector of {0.25, 0.1, 0.5} and radius vector of {10, 10, 10}.\n\n";
    grid::cartesian::uniformAroundOrigin(Vector3D{0.25, 0.1, 0.5}, {10, 10, 10}, cs_cartesian.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cartesian - Uniform Mesh Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the cartesian coordinate system with uniform mesh around the origin including the origin\n";
    cout << "    with a distance vector of {0.25, 0.1, 0.5} and radius vector of {10, 10, 10}.\n\n";
    grid::cartesian::uniformOnOrigin(Vector3D{.25, .1, .5}, {10, 10, 10}, cs_cartesian.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cartesian - Uniform Mesh Around Origin Including Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the cartesian coordinate system with uniform mesh\n";
    cout << "   from point {1, -2, 3} to point {11, 2, 13} with a distance vector of {0.1, 4.0 / 16, 0.2}.\n\n";
    grid::cartesian::uniformDirectional(Point3D{1, -2, 3}, Point3D{11, 2, 13}, {0.1, 4.0 / 16, 0.2}, cs_cartesian.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cartesian - Uniform Mesh between Two Points", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the cartesian coordinate system with uniform mesh\n";
    cout << "   from point {1, -2, 3}, a distance vector of {0.1, 4.0 / 16, 0.2}, and for {100, 16, 50} steps.\n\n";
    grid::cartesian::uniformDirectional(Point3D{1, -2, 3}, Vector3D{0.1, 4.0 / 16, 0.2}, {100, 16, 50}, cs_cartesian.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cartesian - Uniform Mesh with Point and Distance Vector", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning cartesian coordinate system.\n";
    cs_cartesian.clearGrids();

    cout << "\n\n==============================================< GCS::cylindrical\n";

    cout << "\n\nMeshing the cylindrical coordinate system with uniform mesh around the orgin froming a ring\n";
    cout << "   with inner radius of 5, outer radius of 6, heigth from -2 to 2 and a distance vector of {0.1, 1 degree, 0.1}.\n\n";
    grid::cylindrical::uniformRing(5, 6, -2, 2, {.1, grid::toRad(1), .1}, cs_cylindrical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cylindrical - Uniform Mesh - Ring Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the cylindrical coordinate system with uniform mesh around the orgin froming a cylinder\n";
    cout << "   with radius of 5, heigth from -2 to 2 and a distance vector of {0.1, 1 degree, 0.1}.\n\n";
    grid::cylindrical::uniformCylinder(5, -2, 2, {.1, grid::toRad(1), .1}, cs_cylindrical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cylindrical - Uniform Mesh - Cylinder Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the cylindrical coordinate system with uniform mesh around the orgin\n";
    cout << "   from a point at {1.0, -20 degree, 4} to a point at {11.0, 180 degree, 13} with a distance vector of {0.1, 1 degree, 0.1}.\n\n";
    grid::cylindrical::uniformDirectional(Point3D{1, grid::toRad(-20), 4}, Point3D{11, grid::toRad(180), 13}, {0.1, grid::toRad(1), 0.1}, cs_cylindrical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cylindrical - Uniform Mesh between Two Points", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the cylindrical coordinate system with uniform mesh around the orgin\n";
    cout << "   from a point at {1.0, -20 degree, 4}, a distance vector of {0.1, 1 degree, 0.1}, and for {100, 160, 150} steps.\n\n";
    grid::cylindrical::uniformDirectional(Point3D{1, grid::toRad(-20), 4}, Vector3D{0.1, grid::toRad(1), 0.1}, {100, 16, 50}, cs_cylindrical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cylindrical - Uniform Mesh with Point and Distance Vector", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning cylindrical coordinate system.\n";
    cs_cylindrical.clearGrids();

    cout << "\n\n==============================================< GCS::cylindrical::cartesian\n";

    cout << "\n\nMeshing the cartesian coordinate system with uniform mesh around the origin\n";
    cout << "    with a distance vector of {0.25, 0.1, 0.5} and radius vector of {10, 10, 10}.\n\n";
    grid::cartesian::uniformAroundOrigin(Vector3D{0.25, 0.1, 0.5}, {10, 10, 10}, cs_cyl_cartesian.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cylindrical::Cartesian - Uniform Mesh Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning cartesian coordinate system.\n";
    cs_cyl_cartesian.clearGrids();

    cout << "\n\n==============================================< GCS::cylindrical::cylindrical\n";

    cout << "\n\nMeshing the cylindrical coordinate system with uniform mesh around the orgin froming a ring\n";
    cout << "   with inner radius of 5, outer radius of 6, heigth from -2 to 2 and a distance vector of {0.1, 1 degree, 0.1}.\n\n";
    grid::cylindrical::uniformRing(5, 6, -2, 2, {.1, grid::toRad(1), .1}, cs_cyl_cylindrical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cylindrical::Cylindrical - Uniform Mesh - Ring Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning cylindrical coordinate system.\n";
    cs_cyl_cylindrical.clearGrids();

    cout << "\n\n==============================================< GCS::cylindrical::spherical\n";

    cout << "\n\nMeshing the spherical coordinate system with uniform mesh around the orgin froming a ball\n";
    cout << "   with inner radius of 5, outer radius of 6, and a distance vector of {0.1, 1 degree, 2 degrees}.\n\n";
    grid::spherical::uniformBall(5, 6, {0.1, grid::toRad(1), grid::toRad(2)}, cs_cyl_spherical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Cylindrical::Spherical - Uniform Mesh - Ball Around Origin", "grid point", GCS.getAllGridPoints(true));
    cout << "\nCleaning spherical coordinate system.\n";
    cs_cyl_spherical.clearGrids();

    cout << "\n\n==============================================< GCS::spherical\n";

    cout << "\n\nMeshing the spherical coordinate system with uniform mesh around the orgin froming a Spherical\n";
    cout << "   with radius of 5, a distance vector of {0.1, 1 degree, 2 degrees}.\n\n";
    grid::spherical::uniformSphere(5, {0.1, grid::toRad(1), grid::toRad(2)}, cs_spherical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Spherical - Uniform Mesh - Sphere Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the spherical coordinate system with uniform mesh around the orgin froming a ball\n";
    cout << "   with inner radius of 5, outer radius of 6, and a distance vector of {0.1, 1 degree, 2 degrees}.\n\n";
    grid::spherical::uniformBall(5, 6, {0.1, grid::toRad(1), grid::toRad(2)}, cs_spherical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Spherical - Uniform Mesh - Ball Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the spherical coordinate system with uniform mesh around the orgin\n";
    cout << "   from a point at {1.0, -20 degree, 50 degree} to a point at {11.0, 185 degree, 120 degree} with a distance vector of {0.1, 1 degree, 2 degree}.\n\n";
    grid::spherical::uniformDirectional(Point3D{1, grid::toRad(-20), grid::toRad(50)}, Point3D{11, grid::toRad(185), grid::toRad(120)}, {0.1, grid::toRad(1), grid::toRad(2)}, cs_spherical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Spherical - Uniform Mesh between Two Points", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\nMeshing the spherical coordinate system with uniform mesh around the orgin\n";
    cout << "   from a point at {1.0, -20 degree, 50 degree}, a distance vector of {0.1, 1 degree, 2 degree}, and for {100, 160, 150} steps.\n\n";
    grid::spherical::uniformDirectional(Point3D{1, grid::toRad(-20), grid::toRad(50)}, Vector3D{0.1, grid::toRad(1), grid::toRad(2)}, {100, 160, 150}, cs_spherical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Spherical - Uniform Mesh with Point and Distance Vector", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning spherical coordinate system.\n";
    cs_spherical.clearGrids();

    cout << "\n\n==============================================< GCS::spherical::cartesian\n";

    cout << "\n\nMeshing the cartesian coordinate system with uniform mesh around the origin\n";
    cout << "    with a distance vector of {0.25, 0.1, 0.5} and radius vector of {10, 10, 10}.\n\n";
    grid::cartesian::uniformAroundOrigin(Vector3D{0.25, 0.1, 0.5}, {10, 10, 10}, cs_sph_cartesian.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Spherical::Cartesian - Uniform Mesh Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning cartesian coordinate system.\n";
    cs_sph_cartesian.clearGrids();

    cout << "\n\n==============================================< GCS::spherical::cylindrical\n";

    cout << "\n\nMeshing the cylindrical coordinate system with uniform mesh around the orgin froming a ring\n";
    cout << "   with inner radius of 5, outer radius of 6, heigth from -2 to 2 and a distance vector of {0.1, 1 degree, 0.1}.\n\n";
    grid::cylindrical::uniformRing(5, 6, -2, 2, {.1, grid::toRad(1), .1}, cs_sph_cylindrical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Spherical::Cylindrical - Uniform Mesh - Ring Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning cylindrical coordinate system.\n";
    cs_sph_cylindrical.clearGrids();

    cout << "\n\n==============================================< GCS::spherical::spherical\n";

    cout << "\n\nMeshing the spherical coordinate system with uniform mesh around the orgin froming a ball\n";
    cout << "   with inner radius of 5, outer radius of 6, and a distance vector of {0.1, 1 degree, 2 degrees}.\n\n";
    grid::spherical::uniformBall(5, 6, {0.1, grid::toRad(1), grid::toRad(2)}, cs_sph_spherical.axes());
    t_start = clock(); exportToVTK("coordinates/vtks/", "Spherical::Spherical - Uniform Mesh - Ball Around Origin", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning spherical coordinate system.\n";
    cs_sph_spherical.clearGrids();

    cout << "\n\n==========================================< Editing Meching >==========================================\n\n";
    cout <<     "==========================================< GCS::cylindrical::spherical\n";

    cout << "\n\nMeshing the spherical coordinate system with uniform mesh around the orgin froming a ball\n";
    cout << "   with inner radius of 5, outer radius of 6, and a distance vector of {0.1, 1 degree, 2 degrees}.\n\n";
    grid::spherical::uniformBall(5, 6, {0.1, grid::toRad(1), grid::toRad(2)}, cs_cyl_spherical.axes());
    t_start = clock(); exportToVTK("editing/vtks/", "1. Original", "grid point", GCS.getAllGridPoints(true));

    cout << "Moving the origin of the cylindrical coordinate system to {10, -15, 50}.\n";
    cs_cylindrical.origin() = Point3D{10, -15, 50};
    t_start = clock(); exportToVTK("editing/vtks/", "2. Cylindrical - origin: {10, -15, 50}", "grid point", GCS.getAllGridPoints(true));

    cout << "Changing the z axis of the cylindrical coordinate system.\n";
    cs_cylindrical.axesDirectionVector(2) = Vector3D{1, 1, 1};
    t_start = clock(); exportToVTK("editing/vtks/", "3. Cylindrical - z: {1, 1, 1}", "grid point", GCS.getAllGridPoints(true));

    cout << "Changing the z axis of the cylindrical coordinate system.\n";
    cs_cylindrical.axesDirectionVector(2).normalize();
    t_start = clock(); exportToVTK("editing/vtks/", "4. Cylindrical - z: {1, 1, 1}.unit()", "grid point", GCS.getAllGridPoints(true));

    cout << "Scaling the spherical r axis by two.\n";
    cs_cyl_spherical.axesDirectionVector(0) = convertors::cylindrical::multiply(2, cs_cyl_spherical.axesDirectionVector(0));
    t_start = clock(); exportToVTK("editing/vtks/", "5. Cylindrical::Spherical - R0 = 2 * r0", "grid point", GCS.getAllGridPoints(true));

    cout << "\nCleaning spherical coordinate system.\n";
    cs_cyl_spherical.clearGrids();

    cout << "returning the coordinate systems back to normal.\n";
    cs_cylindrical.origin() = Point3D{0, 0, 0};
    cs_cylindrical.axesDirectionVector(2) = Vector3D{0, 0, 1};
    cs_cyl_spherical.axesDirectionVector(0) = convertors::cylindrical::multiply(1.0 / 2, cs_cyl_spherical.axesDirectionVector(0));

    cout << "\n\n========================================< Neighborhood Search >========================================\n\n";
    cout <<     "========================================< Adding mesh to all of the coordinate systems\n";

    t_start = clock();

    grid::cylindrical::uniformCylinder(12, -1, 1, {1, grid::toRad(5), .25}, cs_sph_cylindrical.axes());
    grid::cylindrical::uniformRing(12, 15, -2, 2, {.5, grid::toRad(.1), .25}, cs_cyl_cylindrical.axes());
    cs_sph_cylindrical.origin() = {0,0,0};
    cs_cyl_cylindrical.origin() = {0,0,0};

    grid::spherical::uniformBall(3, 5, {5, grid::toRad(45), grid::toRad(60)}, cs_sph_spherical.axes());
    grid::spherical::uniformSphere(3, {.1, grid::toRad(1), grid::toRad(1)}, cs_cyl_spherical.axes());
    cs_sph_spherical.origin() = {20, grid::toRad(30), grid::toRad(90)};
    cs_cyl_spherical.origin() = {20, grid::toRad(30), 0};

    grid::cartesian::uniformDirectional({-30, -30,  10}, Point3D{0, -15, -10}, {1, 1, 1}, cs_sph_cartesian.axes());
    grid::cartesian::uniformDirectional({ 30, -30, -10}, Point3D{0, -15,  10}, {1, 1, 1}, cs_cyl_cartesian.axes());
    grid::cartesian::uniformOnOrigin({1,1,1},{30 ,7.5,10}, cs_cartesian.axes());
    cs_cartesian.origin() = {0, 23, 0};

    printExacutionTime(CoordinateSystem::Global().gridSize(true), "grid point");

    t_start = clock(); exportToVTK("neighbors/vtks/", "Grid_Points", "grid point", GCS.getAllGridPoints(true));

    cout << "\n\n========================================< Adding Points\n";
    cout << "Creating a cuboid.\n";
    const auto plate = geometry::Combined::Cuboid({-30, -30,  -5}, space::vec3{60, 0, 0}, {0, 60, 0}, {0, 0, 10});

    cout << "Converting the grid points of all coordinate systems to the points if they are inside a Cuboid.\n";
    t_start = clock();
    CoordinateSystem::Global().include([&plate](const Point3D& point) {
        return plate.pointStatus(point) == geometry::Inside;
    }, true);
    printExacutionTime(CoordinateSystem::Global().gridSize(true), "point");

    t_start = clock(); exportToVTK("neighbors/vtks/", "Points", "point", GCS.getAllPoints(true));

    cout << "\n\n========================================< Searching\n";

    {   cout << "Searching for selected spherical neighborhoods inside the CGS::Spherical::Cartezian coordinate system.\n";
        using AttSource = exporting::vtk::AttSource;
        using VTKCell   = exporting::VTKCell;
        using namespace   exporting::vtk::convertors;
        const auto neighborhoodRadius = 10;

        auto neighborhoodShape = geometry::Primary::Sphere({0, 0, 0}, neighborhoodRadius);
        const auto includer = [&neighborhoodShape](const Point3D& centre, const Point3D& neighbor) -> bool {
            using namespace geometry;
            neighborhoodShape.setPosition(centre);
            return neighborhoodShape.pointStatus(neighbor) != PointStatus::Outside;
        };

        const auto random = [](const double range) -> double { return range * ((double) rand() / (RAND_MAX)) + 1; };
        CoordinateSystem::Points centers;
        centers.push_back(Point3D{cs_sph_cartesian.axes()[0].at(0),
                                  cs_sph_cartesian.axes()[1].at(0),
                                  cs_sph_cartesian.axes()[2].at(0)});                                             // Point outside and beside the corner.
        centers.push_back(*cs_sph_cartesian.getPoint(0));                                                         // Point on the corner.
        centers.push_back(Point3D{cs_sph_cartesian.axes()[0].at(cs_sph_cartesian.axes()[0].size() / 2),
                                  cs_sph_cartesian.axes()[1].at(0),
                                  cs_sph_cartesian.axes()[2].at(0)});                                             // Point outside and in the middle of the one edge.
        centers.push_back(*cs_sph_cartesian.getPoint(cs_sph_cartesian.axes()[0].size() / 2));                     // Point at the middle of the one edge.
        centers.push_back({-30 + random(30), -30 + random(15), -10 + random(20)});                                // Random point inside the grid cluster.
        centers.push_back({-30 + random(30), -30 + random(15), -15 + random(10)});                                // Random point outside the grid cluster.
        centers.push_back({-30 + random(30), -30 + random(15), -5 + random(10)});                                 // Random point inside the point cluster.
        centers.push_back(Point3D{cs_sph_cartesian.axes()[0].at(cs_sph_cartesian.axes()[0].size() / 2),
                                  cs_sph_cartesian.axes()[1].at(cs_sph_cartesian.axes()[1].size() / 2),
                                  cs_sph_cartesian.axes()[2].at(cs_sph_cartesian.axes()[2].size() / 2)});         // Point at the middle.
        // Set up vtk file
        const std::string folder = "neighbors/vtks/";
        const auto endsFileName = "Spherical_Neighborhoods_points";
        const auto bondFileName = "Spherical_Neighborhoods_bonds";

        cout << "Exporting neighborhoods' points to: \"" << endsFileName  << "\" file.\n";
        cout << "      and neighborhoods' bonds to: \"" << bondFileName  << "\" file.\n";
        t_start = clock();

        auto endsFile = vtk::FileSeries(path + folder, endsFileName, "vtk", centers.size());
        auto bondFile = vtk::FileSeries(path + folder, bondFileName, "vtk", centers.size());

        // Removing previously produced files.
        endsFile.removeAll(false);
        bondFile.removeAll(false);

        size_t itr = 0;
        for(const auto& centre : centers) {
            const auto neighbors = cs_sph_cartesian.getNeighborPoints(centre, Vector3D{neighborhoodRadius, neighborhoodRadius, neighborhoodRadius}, includer);
            vector<double> att;

            if (!neighbors.empty()) {
                endsFile.at(itr).setDataTitle("An example of neighborhood search for searching spherical neighborhoods inside the CGS::Spherical::Cartezian coordinate system.");
                endsFile.at(itr)["distance"].setSource(AttSource::Cell);
                endsFile.at(itr)["distance"].setAttributeType(exporting::vtk::AttType::Scalar);

                endsFile.at(itr).appendCell(VTKCell::Vertex, convertToVertexes({centre}));
                att.emplace_back(0);

                for(const auto& neighbor: neighbors) {
                    endsFile.at(itr).appendCell(VTKCell::Vertex, convertToVertexes({neighbor}));
                    att.emplace_back((neighbor.positionVector() - centre.positionVector()).length());
                }

                endsFile.at(itr)["distance"].appendAttribute(att);
                endsFile.at(itr).assemble();

                att.erase(att.begin());

                bondFile.at(itr).setDataTitle("An example of neighborhood search for searching spherical neighborhoods inside the CGS::Spherical::Cartezian coordinate system.");
                bondFile.at(itr)["bond_length"].setSource(AttSource::Cell);
                bondFile.at(itr)["bond_length"].setAttributeType(exporting::vtk::AttType::Scalar);

                for(const auto& neighbor: neighbors)
                    bondFile.at(itr).appendCell(VTKCell::Line, convertToVertexes({centre, neighbor}));

                bondFile.at(itr)["bond_length"].appendAttribute(att);
                bondFile.at(itr).assemble();
            }
            itr++;
        }

        printExacutionTime(centers.size() * 2, "neighborhood search");
    }

    {   cout << "Searching counterpart neighbors of GCS::Cartesian on CGS::Cylindrical::Cartezian coordinate system.\n";
        t_start = clock();

        const auto singleNeighbor = [](const Point3D& centre, const Point3D& neighbor) -> bool {
            const auto& centerPosVec = centre.positionVector();
            const auto& neighborPosVec = neighbor.positionVector();
            if (space::areEqual(centerPosVec[0], neighborPosVec[0])
                && space::areEqual(centerPosVec[1], -neighborPosVec[1])
                && space::areEqual(centerPosVec[2], neighborPosVec[2]))
                return true;
            return false;
        };

        const auto cs_cartesian_poitns = cs_cartesian.getAllPoints();

        const auto name = "neighbor";
        const std::string folder = "neighbors/PairedNeighbors/";
        const auto fileName = "PairedNeighbors";

        cout << "Exporting " << name  << "s \"" << fileName  << "\"\n";
        auto file = vtk::FileSeries(path + folder, fileName, "vtk", cs_cartesian_poitns.size());
        file.removeAll(false); // Removing previously produced files.
        const auto attName = "neighbor-pair";

        t_start = clock();
        size_t itr = 0;
        for(const auto& point : cs_cartesian_poitns) {
            file.at(itr).setDataTitle("An example of neighborhood search for searching neighbors of GCS::Cartesian on CGS::Cylindrical::Cartezian coordinate system.");

            const auto pointInGCS = GCS.convert(point, cs_cartesian);
            const auto neighbors = cs_cyl_cartesian.getNeighborPoints(pointInGCS, Vector3D{0.5, 100, 0.5}, singleNeighbor, pointInGCS);

            file.at(itr)[attName].setSource(exporting::vtk::AttSource::Cell);
            file.at(itr)[attName].setAttributeType(exporting::vtk::AttType::Scalar);
            file.at(itr).appendCell(exporting::VTKCell::Vertex, exporting::vtk::convertors::convertToVertexes({pointInGCS}));
            if (!neighbors.empty()) {
                file.at(itr).appendCell(exporting::VTKCell::PolyVertex, exporting::vtk::convertors::convertToVertexes(neighbors));
                file.at(itr)[attName].appendAttribute<int>({0, 1});
            } else {
                file.at(itr)[attName].appendAttribute<int>({0});
            }

            file.at(itr).assemble();
            itr++;
        }

        printExacutionTime(CoordinateSystem::Global().gridSize(true), "neighborhood search");
    }

    return EXIT_SUCCESS;
}

//
//  WaveDispersionAndPropagation.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 10.10.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "coordinate_system/grid.h"
#include "configuration/Part.h"
#include "relations/peridynamic.h"
#include <iostream>
#include <time.h>

using namespace rbs;
using namespace rbs::configuration;
using namespace std;

/**
 * @brief Presents the construction process of work done by Nishawal et.al. on their paper:
 *        "Simulation of Elastic Wave Propagation using Cellular Automata and Peridynamics, and Comparison with Experiments."
 * @details: Find more information and step by step description on https://github.com/alijenabi/RelationBasedSimulator/examples/Elastic%20Wave%20Propagation%20in%20Plate/Elastic%20Wave%20Propagation%20in%20Plate.md
 * @note Examples are suitable for clang compiler. Running them suing other compilers may require modification.
 * @note Examples are suitable for mac file-system. Running them on other operating systems may require modification.
 * @return EXIT_SUCCESS if all examples are build successfuly, EXIT_FAILURE otherwise.
 */
int main() {

    const std::string path = "/<An existing folder path>/";
    auto& logger = report::Logger::centre();

    /* - Uncomment the code below if you like to log the simulation.- */
//    logger.setFileLevel(logger.Debug,   path + "logs/", "Debug");
//    logger.setFileLevel(logger.Timing,  path + "logs/", "Timing");
//    logger.setFileLevel(logger.Process, path + "logs/", "Process");
//    logger.setFileLevel(logger.Warning, path + "logs/", "Warning");

    using BC = report::Logger::Broadcast;
    logger.log(BC::Block, "Problem definition");

    const double dencity = 1300;           // kg / m^3
    const double youngsModulus = 3.85e9;   // GPa
    const double poissonRasio = 1. / 3.;

    const auto plateHeight = 0.5;          // 0.5 m
    const auto plateWidth  = 1.0;          // 1.0 m
    const auto plateThickness = 0.006655;  // 6.655 mm

    const auto horizonRasio = 3.;          // m = 3
    const auto gridSpacing = std::min({plateWidth / 1024, plateHeight / 512});
    const auto horizonRadius = horizonRasio * gridSpacing;

    logger.log(BC::Process, "Initiating the plate's Part.\n");
    using CS = coordinate_system::CoordinateSystem;
    auto platePart = Part("Plate", CS::Global().appendLocal(CS::Cartesian));

    logger.log(BC::Process, "Creating the part geometry.");
    {
        const auto plateShape = geometry::Combined::Cuboid({-plateWidth / 2, -plateHeight, -plateThickness / 2},
                                                          space::vec3{plateWidth, 0, 0},
                                                          space::vec3{0, plateHeight, 0},
                                                          space::vec3{0, 0, plateThickness});
        platePart.setGeometry(plateShape);
    }

    logger.log(BC::Block, "Meshing the part's coordinate system.");
    {
        const auto distanceVector = gridSpacing * space::consts::one3D;
        const auto startPoint = space::Point<3>{-plateWidth / 2, -plateHeight, -plateThickness / 2} + distanceVector / 2;
        const auto endPoint   = space::Point<3>{plateWidth / 2, 0, plateThickness / 2} - distanceVector / 2;

        coordinate_system::grid::cartesian::uniformDirectional(startPoint, endPoint,distanceVector, platePart.local().axes());
        platePart.local().axes()[2] = std::set<double>{0};
    }

    logger.log(BC::Block, "Including the points to the coordiante system.");
    logger.log(BC::Process, "Including the grid points that are inside the part shape.\n");
    {
        const auto& plateShape = platePart.geometry();
        platePart.local().include([&plateShape](const auto& localPoint) {
            return plateShape.pointStatus(localPoint) == geometry::Inside;
        });
    }

    logger.log(BC::Block, "Neighborhood search");

    platePart.initiateNeighborhoods();

    logger.log(BC::Process, "Adding the volume and the density.");
    const double gridVolume = pow(gridSpacing, 2) * plateThickness;
    const auto& neighborhoods = platePart.neighborhoods();
    std::for_each(neighborhoods.begin(), neighborhoods.end(), [gridVolume, dencity](const Part::NeighborhoodPtr& neighborhood) {
        using Property = relations::peridynamic::Property;
        auto& centre = *neighborhood->centre();
        centre.at(Property::Volume).setValue(gridVolume);
        centre.at(Property::Density).setValue(dencity);
    });

    platePart.searchInnerNeighbors(horizonRadius);

   /* - Uncomment the code below to check the configuration and neighborhood search checkpoints. - */
//    logger.log(BC::Block, "Exporting to VTK");
//    logger.log(BC::ProcessStart, "");
//    platePart.exportConfiguration(path + "vtks/");
//    platePart.exportCheckpointNeighborhoods(path + "vtks/");
//    logger.log(BC::ProcessEnd, "");

    logger.log(BC::Block, "Defining the relations.");
    const auto maxForcePerNode = 20.7e3 / ( plateThickness * gridSpacing );
    const auto boundaryConditioner = [gridSpacing, maxForcePerNode](const double time, configuration::Node& node) {
        using Property = relations::peridynamic::Property;
        auto& postion = node.initialPosition().value<space::Point<3> >().positionVector();

        if (-gridSpacing <= postion[0] && postion[0] <= gridSpacing && -gridSpacing * 2.1 < postion[1]) {
            if (time <= 10e-6) {
                node.at(Property::Force) = -space::vec3{0, time * maxForcePerNode / 10e-6, 0};
            } else if(time <= 20e-6) {
                node.at(Property::Force) = -space::vec3{0, maxForcePerNode - (time - 10e-6) * maxForcePerNode / 10e-6, 0};
            } else {
                node.at(Property::Force) = space::consts::o3D;
            }
        } else {
            if (node.has(Property::Force))
                node.at(Property::Force) = space::consts::o3D;
        }
    };
    auto load = relations::peridynamic::BoundaryDomain(boundaryConditioner, platePart);

    auto timeIntegration = relations::peridynamic::time_integration::VelocityVerletAlgorithm(platePart);

    const double bulkModulus = youngsModulus / (3 * (1 - 2 * poissonRasio));
    const double materialConstant = 12 * bulkModulus / (M_PI * pow(horizonRadius, 3) * plateThickness);
    logger.log(BC::Process, "Bulk Modulus = " + to_string(bulkModulus));
    logger.log(BC::Process, "Material Constant = " + to_string(materialConstant));
    auto platePDRelation = relations::peridynamic::BondBased::Elastic(materialConstant, gridSpacing, horizonRadius, platePart, false);

    using P = relations::peridynamic::Property;
    using On = relations::peridynamic::Exporter::Target;
    const std::set properties = {P::Displacement, P::Velocity, P::Acceleration, P::Force};

    auto plateExporterCC = relations::peridynamic::Exporter(properties, On::CurrentConfiguration, platePart, path + "vtks/", "PlatePartOnCurrentConfig");
    plateExporterCC.setCondition([](const double, const size_t timeStep) {
        return timeStep % 10 == 0;
    });

    const auto exportationCondition = [](const double, const size_t timeStep) {
        return timeStep == 857;
    };
    auto plateExporterAt107ms = relations::peridynamic::Exporter(properties, On::InitialConfiguration, platePart, path + "vtks/", "plateExporterAt107ms");
    auto plateExporterAt107msCurrent = relations::peridynamic::Exporter(properties, On::InitialConfiguration, platePart, path + "vtks/", "plateExporterAt107msCurrent");
    plateExporterAt107ms.setCondition(exportationCondition);
    plateExporterAt107msCurrent.setCondition(exportationCondition);

    const auto maxSondSpeed = sqrt(youngsModulus / dencity);
    const auto maxTimeSpan = std::min({0.125e-6, gridSpacing / maxSondSpeed * 0.8}); // .5 Safty Factor.
    logger.log(BC::Process, "Maximum Sound Speed = " + to_string(maxSondSpeed));
    logger.log(BC::Process, "Maximum Time Span = " + report::date_time::duration::formated(maxTimeSpan, 3));

    auto& analysis = Analyse::current();
    analysis.setTimeSpan(maxTimeSpan);
    analysis.setMaxTime(308e-6);
    analysis.appendRelation(load);
    analysis.appendRelation(platePDRelation);
    analysis.appendRelation(timeIntegration);
    analysis.appendRelation(plateExporterCC);
    analysis.appendRelation(plateExporterAt107ms);
    analysis.appendRelation(plateExporterAt107msCurrent);

    return analysis.run();

    return EXIT_SUCCESS;
}

//
//  PlateWithPreExistingCrack.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 10.10.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#include "coordinate_system/grid.h"
#include "configuration/Part.h"
#include "relations/peridynamic.h"

using namespace rbs;
using namespace rbs::configuration;
using namespace std;

/**
 * @details: Find more information and step by step description on 
              https://github.com/alijenabi/RelationBasedSoftware/examples/Plate%20with%20a%20pre-existing%20crack/PlateWithPreExistingCrack.md
 * @note Examples are suitable for clang compiler. Running them suing other compilers may require modification.
 * @note Examples are suitable for mac file-system. Running them on other operating systems may require modification.
 * @return EXIT_SUCCESS if all examples are build successfuly, EXIT_FAILURE otherwise.
 */
int main() {

    const std::string path = "An existing folder path";
    auto& logger = report::Logger::centre();

    /** - Uncomment the code below if you like to log the simulation.- */
//    logger.setFileLevel(logger.Debug,   path + "logs/", "Debug");
//    logger.setFileLevel(logger.Timing,  path + "logs/", "Timing");
//    logger.setFileLevel(logger.Process, path + "logs/", "Process");
//    logger.setFileLevel(logger.Warning, path + "logs/", "Warning");

    using BC = report::Logger::Broadcast;
    logger.log(BC::Block, "Problem definition");

    const double dencity = 8000;           // kg / m^3
    const double youngsModulus = 192e9;    // 192GPa
//    const double poissonRasio = 1. / 3.;

    const auto boundaryVelocityMagnitude = 20.; // 20m/s or 50m/s

    const auto plateDimension = 0.05;      // 5 cm
    const auto plateThickness = 0.0001;    // 0.1 mm
    const auto preCrackLength = 0.01;      // 1 cm

    const auto horizonRasio = 3.015;       // m = 3.015
    const auto minMeshsInDimension = 500;
    const auto minMeshsInThickness = 1;
    const auto gridSpacing = std::min({plateDimension / minMeshsInDimension, plateThickness / minMeshsInThickness});
    const auto horizonRadius = horizonRasio * gridSpacing;

    const auto gridVolume = pow(gridSpacing, 2) * plateThickness;
//    const auto boundaryVolume = 3. * plateThickness * plateDimension;

    const auto criticalStretch = 0.04472; // 1 for elastic.

    const auto expectedTimeSpan = 1.3367e-8;
    const auto maxTimeSteps = 1250;
    const auto maxSimulationTime = maxTimeSteps * expectedTimeSpan;

    logger.log(BC::Process, "Initiating the plate's Part.\n");
    using CS = coordinate_system::CoordinateSystem;
    auto platePart = Part("Plate", CS::Global().appendLocal(CS::Cartesian));

    logger.log(BC::Block, "Meshing the part's coordinate system.");
    {
        const auto distanceVector = gridSpacing * space::consts::one3D;
        const auto radiusVector = space::vec3{plateDimension / 2, plateDimension / 2 + 3. * gridSpacing, plateThickness / 2};

        coordinate_system::grid::cartesian::uniformAroundOrigin(distanceVector, radiusVector, platePart.local().axes());
        platePart.local().axes()[2] = std::set<double>{ 0 };
    }

    logger.log(BC::Block, "Including the points to the coordiante system.");
    platePart.local().include([](const auto&) { return true; });

    logger.log(BC::Block, "Neighborhood search");
    platePart.initiateNeighborhoods();

    logger.log(BC::Process, "Adding the volume and the density.");
    {
        const auto& neighborhoods = platePart.neighborhoods();
        std::for_each(neighborhoods.begin(), neighborhoods.end(), [gridVolume, dencity](const Part::NeighborhoodPtr& neighborhood) {
            using Property = relations::peridynamic::Property;
            auto& centre = *neighborhood->centre();
            centre.at(Property::Volume).setValue(gridVolume);
            centre.at(Property::Density).setValue(dencity);
        });
    }

    platePart.searchInnerNeighbors(horizonRadius);

    logger.log(BC::Process, "Removeing the bonds that are passing through the notch.");
    {
        const auto halfSpace = geometry::Primary::HalfSpace(space::Point<3>{0, 0, 0}, space::consts::j3D);
        size_t removedBonds = 0;
        const auto& neighborhoods = platePart.neighborhoods();
        std::for_each(neighborhoods.begin(), neighborhoods.end(), [&halfSpace, &removedBonds, preCrackLength, horizonRadius](const Part::NeighborhoodPtr& neighborhood) {
            const auto& centerPos = neighborhood->centre()->initialPosition().value<space::Point<3> >();
            const auto& centerPosVec = centerPos.positionVector();
            if (-preCrackLength / 2 < centerPosVec[0] && centerPosVec[0] < preCrackLength / 2
                && -horizonRadius * 2 < centerPosVec[1] && centerPosVec[1] < horizonRadius * 2) {
                const auto centerStatus = halfSpace.pointStatus(centerPos);
                auto& neighbors = neighborhood->neighbors();
                std::for_each(neighbors.begin(), neighbors.end(), [&halfSpace, &removedBonds, &neighborhood, centerStatus](const Part::NeighborhoodPtr& neighbor) {
                    using Property = relations::peridynamic::Property;
                    const auto neighborStatus = halfSpace.pointStatus(neighbor->centre()->initialPosition().value<space::Point<3> >());
                    const bool shouldMark = neighborStatus != centerStatus || (neighborStatus == centerStatus && neighborStatus == geometry::On);
                    if (shouldMark) {
                        removedBonds++;
                        neighborhood->setStatus(neighbor, Property::Damage, int(1));
                    }
                });

                /**
                 * - Alternatively, we can remove the bonds.
                 * - This will be faster, but the pre-crack will not be seen as a damaged area in the exported VTK files.
                 */
//                neighbors.erase(
//                    std::remove_if(neighbors.begin(), neighbors.end(), [&halfSpace, &removedBonds, centerStatus](const Part::NeighborhoodPtr& neighbor) -> bool{
//                        const auto neighborStatus = halfSpace.pointStatus(neighbor->centre()->initialPosition().value<space::Point<3> >());
//                        const bool shouldRemove = neighborStatus != centerStatus || (neighborStatus == centerStatus && neighborStatus == geometry::On);
//                        if (shouldRemove) { removedBonds++; }
//                        return shouldRemove;
//                    }),
//                    neighbors.end());
            }
        });

        logger.log(BC::Process, std::to_string(removedBonds) + " bonds removed.");
    }

   /** - Uncomment the code below to check the configuration and neighborhood search checkpoints. - */
//    logger.log(BC::Block, "Exporting to VTK");
//    logger.log(BC::ProcessStart, "");
//    platePart.exportConfiguration(path + "vtks/");
//    platePart.exportCheckpointNeighborhoods(path + "vtks/");
//    logger.log(BC::ProcessEnd, "");

    logger.log(BC::Block, "Defining the relations.");
    const auto boundaryConditioner = [boundaryVelocityMagnitude, plateDimension](const double time, configuration::Node& node) {
        using Property = relations::peridynamic::Property;
        auto& position = node.initialPosition().value<space::Point<3> >().positionVector();

        if (position[1] > plateDimension / 2 || position[1] < -plateDimension / 2) {
            const auto velocity = node.has(Property::Velocity) ? node.at(Property::Velocity).value<space::vec3>() : space::consts::o3D;
            const auto displacement = node.has(Property::Displacement) ? node.at(Property::Displacement).value<space::vec3>() : space::consts::o3D;

            const auto directionalVelocityMagnitude = (position[1] > 0 ? 1. : -1.) * boundaryVelocityMagnitude;
            const auto directionalDisplaceMagnitude = boundaryVelocityMagnitude * time;

            node.at(Property::Velocity)     = space::vec3{velocity[0], directionalVelocityMagnitude, velocity[2]};
            node.at(Property::Displacement) = space::vec3{displacement[0], directionalDisplaceMagnitude, displacement[2]};
        }
    };
    auto load = relations::peridynamic::BoundaryDomain(boundaryConditioner, platePart);

    const double materialConstant = 18 * youngsModulus  / (M_PI * pow(horizonRadius, 3) * plateThickness);
    logger.log(BC::Process, "Young's Modulus = " + to_string(youngsModulus));
    logger.log(BC::Process, "Material Constant = " + to_string(materialConstant));

    auto platePDRelation = relations::peridynamic::BondBased::BrittleFracture(criticalStretch, materialConstant,
                                                                              gridSpacing, horizonRadius,
                                                                              platePart, true,
                                                                              [plateDimension](const Part::NeighborhoodPtr& center, const Part::NeighborhoodPtr&) {
        const auto& posVec = center->centre()->initialPosition().value<space::Point<3> >().positionVector();
        return - plateDimension / 4 < posVec[1] && posVec[1] < plateDimension / 4;
    });

    auto timeIntegration = relations::peridynamic::time_integration::Euler(platePart);

    using P = relations::peridynamic::Property;
    using On = relations::peridynamic::Exporter::Target;
    const std::set properties = {P::Displacement, P::Velocity, P::Acceleration, P::Force, P::Damage};

    const auto exportConditioner = [](const double, const size_t timeStep) {
        return timeStep % 10 == 0;
    };
    auto plateExporterCC = relations::peridynamic::Exporter(properties, On::CurrentConfiguration, platePart, path + "vtks/", "CurrentConfig");
    auto plateExporterIC = relations::peridynamic::Exporter(properties, On::InitialConfiguration, platePart, path + "vtks/", "InitialConfig");
    plateExporterCC.setCondition(exportConditioner);
    plateExporterIC.setCondition(exportConditioner);

    const auto maxSondSpeed = sqrt(youngsModulus / dencity);
    const auto maxTimeSpan = 0.8 * sqrt(2 * dencity / (M_PI * pow(horizonRadius,2) * materialConstant));
    logger.log(BC::Process, "Maximum Sound Speed = " + to_string(maxSondSpeed));
    logger.log(BC::Process, "Maximum Time Span = " + report::date_time::duration::formated(maxTimeSpan, 3));

    auto& analysis = Analyse::current();
    analysis.setTimeSpan(maxTimeSpan);
    analysis.setMaxTime(maxSimulationTime + maxTimeSpan / 2);

    analysis.appendRelation(load);
    analysis.appendRelation(platePDRelation);
    analysis.appendRelation(timeIntegration);

    analysis.appendRelation(plateExporterCC);
    analysis.appendRelation(plateExporterIC);

    return analysis.run();
}

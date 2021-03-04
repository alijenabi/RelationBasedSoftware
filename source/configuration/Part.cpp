//
//  Part.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Part.h"

namespace rbs::configuration {

Part::Part(const std::string &name) : Part(name, CS::Global().appendLocal(CS::Cartesian)) {
}

Part::Part(const std::string &name, Part::CS &coordinateSystem)
    : p_geometry{nullptr}
    , p_localCoordinateSystem{std::shared_ptr<CS>(&coordinateSystem, [](auto){}) }
    , p_name{name}
    , p_neighborParts{}
{
}

size_t Part::exportConfiguration(const std::string &path) const noexcept{
    if (p_name.find("Part") != std::string::npos)
        return exportConfiguration(path, p_name);
    return exportConfiguration(path, p_name + "Part");
}

size_t Part::exportConfiguration(const std::string &path, const std::string &fileName) const noexcept{
    using Logger = report::Logger;
    auto& logger = Logger::centre();

    try {
        logger.log(Logger::Broadcast::Process, "Exporting \"" + p_name + "\" Part neighbors.");
        logger.log(Logger::Broadcast::ProcessStart, "to: " + path + fileName + ".vtk");
        logger.log(Logger::Broadcast::ProcessEnd, "");

        if (p_neighborhoods.empty()) {
            logger.log(Logger::Broadcast::Warning, "No neighborhood found to export.");
            return 0;
        }

        const auto start_t = clock();
        auto file = exporting::vtk::File(path, fileName, "vtk");

        const auto neighborhoodSizeAttrName = "neighborhood-size";
        file[neighborhoodSizeAttrName].setSource(exporting::vtk::AttSource::Cell);
        file[neighborhoodSizeAttrName].setAttributeType(exporting::vtk::AttType::Scalar);
        std::vector<int> neighborhoodSizes;
        neighborhoodSizes.reserve(p_neighborhoods.size());

        if (p_localCoordinateSystem && !p_localCoordinateSystem->isGlobal()) {
            const auto& GCS = coordinate_system::CoordinateSystem::Global();
            for (const auto& neighborhood : p_neighborhoods) {
                const auto point = GCS.convert(neighborhood->centre()->initialPosition().value< space::Point<3> >(), *p_localCoordinateSystem);
                file.appendCell(exporting::VTKCell::Vertex, exporting::vtk::convertors::convertToVertexes({ point }));
                neighborhoodSizes.emplace_back(neighborhood->neighbors().size());
            }
        } else {
            for (const auto& neighborhood : p_neighborhoods) {
                const auto point = neighborhood->centre()->initialPosition().value< space::Point<3> >();
                file.appendCell(exporting::VTKCell::Vertex, exporting::vtk::convertors::convertToVertexes({ point }));
                neighborhoodSizes.emplace_back(neighborhood->neighbors().size());
            }
        }

        file[neighborhoodSizeAttrName].appendAttribute(neighborhoodSizes);
        file.assemble(true);

        const auto problemSize = p_neighborhoods.size();
        logProcessTiming(clock() - start_t, problemSize, "neighborhood", "exported");
        return problemSize;
    } catch (std::exception e) {
        logger.log(Logger::Broadcast::Exeption, e.what());
        logFileMissing(path);
    } catch (...) {
        logUnknownExeption();
    }
    return 0;
}

size_t Part::exportCheckpointNeighborhoods(const std::string &path) const {
    if (p_name.find("Part") != std::string::npos)
        return exportCheckpointNeighborhoods(path, p_name);
    return exportCheckpointNeighborhoods(path, p_name + "Part");
}

size_t Part::exportCheckpointNeighborhoods(const std::string &path, const std::string &fileName) const {
    using Logger = report::Logger;
    auto& logger = Logger::centre();

    const auto& GCS = coordinate_system::CoordinateSystem::Global();
    const auto convert = [&](const space::Point<3>& point) {
        if (p_localCoordinateSystem && !p_localCoordinateSystem->isGlobal())
            return GCS.convert(point, *p_localCoordinateSystem);
        return point;
    };

    logger.log(Logger::Broadcast::Process, "Exporting \"" + p_name + "\" Part neighborhood check points.");
    logger.log(Logger::Broadcast::ProcessStart, "to: " + path + fileName + "_checkpoint_neighborhoods.vtk");
    logger.log(Logger::Broadcast::ProcessEnd, "");

    if (p_neighborhoods.empty()) {
        logger.log(Logger::Broadcast::Warning, "No neighborhood found to export.");
        return 0;
    }

    const auto start_t = clock();

    using Index = size_t;
    std::map<Index, NeighborhoodPtr> checkpointNeighborhoods;
    for(const auto& neighborhood: p_neighborhoods) {
        if (checkpointNeighborhoods.find(neighborhood->neighbors().size()) == checkpointNeighborhoods.end()) {
            checkpointNeighborhoods[neighborhood->neighbors().size()] = neighborhood;
        }
    }

    auto file = exporting::vtk::FileSeries(path, fileName + "_checkpoint_neighborhoods", "vtk", checkpointNeighborhoods.size() + 1);

    size_t itr = 0;
    const auto bondVectorAttName = "neighbor-distance";
    const auto bondLenghtAttName = "bond-lenght";
    const auto indexAttName = "neighbor-index";

    std::vector<double> bondVectors;
    std::vector<double> bondLengths;
    std::vector<int> neighborIndices;

    for (const auto& pair : checkpointNeighborhoods) {
        neighborIndices.clear();
        bondVectors.clear();
        bondLengths.clear();

        const auto neighborhoodCentre = convert( pair.second->centre()->initialPosition().value<space::Point<3> >() );
        {
            const auto centreForVtk = exporting::vtk::convertors::convertToVertexes({ neighborhoodCentre});
            file.at(itr).appendCell(exporting::VTKCell::Vertex, centreForVtk);
        }

        neighborIndices.emplace_back(-1);

        bondVectors.emplace_back(0);
        bondVectors.emplace_back(0);
        bondVectors.emplace_back(0);
        bondLengths.emplace_back(0);

        file.at(itr)[indexAttName].setSource(exporting::vtk::AttSource::Cell);
        file.at(itr)[indexAttName].setAttributeType(exporting::vtk::AttType::Scalar);

        file.at(itr)[bondVectorAttName].setSource(exporting::vtk::AttSource::Cell);
        file.at(itr)[bondVectorAttName].setAttributeType(exporting::vtk::AttType::Vector);

        file.at(itr)[bondLenghtAttName].setSource(exporting::vtk::AttSource::Cell);
        file.at(itr)[bondLenghtAttName].setAttributeType(exporting::vtk::AttType::Scalar);

        const auto& neighbors = pair.second->neighbors();
        for (const auto& neighbor: neighbors) {
            const auto neighboorPosition = convert( neighbor->centre()->initialPosition().value<space::Point<3> >() );
            file.at(itr).appendCell(exporting::VTKCell::Vertex, exporting::vtk::convertors::convertToVertexes({ neighboorPosition }));

            neighborIndices.emplace_back(bondLengths.size() - 1);

            const auto neighborCentre = convert( neighbor->centre()->initialPosition().value<space::Point<3> >().positionVector() );
            const auto vector = neighborCentre.positionVector() - neighborhoodCentre.positionVector();
            bondVectors.emplace_back(vector[0]);
            bondVectors.emplace_back(vector[1]);
            bondVectors.emplace_back(vector[2]);

            bondLengths.emplace_back(vector.length());
        }

        file.at(itr)[indexAttName].appendAttribute(neighborIndices);
        file.at(itr)[bondVectorAttName].appendAttribute(bondVectors);
        file.at(itr)[bondLenghtAttName].appendAttribute(bondLengths);

        file.at(itr).assemble();

        itr++;
    }

    const auto problemSize = checkpointNeighborhoods.size();
    logProcessTiming(clock() - start_t, problemSize, "neighborhood check point", "exported");
    return problemSize;
}

size_t Part::exportConnection(const Part &part, const std::string &path) const noexcept {
    if (p_name.find("Part") != std::string::npos)
        return exportConnection(part, path, p_name + "PartConnectionTo" + part.p_name + "Part");
    return exportConnection(part, path, p_name + "PartConnectionTo" + part.p_name + "Part");
}

size_t Part::exportConnection(const Part &part, const std::string &path, const std::string &fileName) const noexcept {
    using Logger = report::Logger;
    auto& logger = Logger::centre();

    try {
        logger.log(Logger::Broadcast::Process, "Exporting \"" + p_name + "\" Part neighbors with \"" + part.p_name + "\".");
        logger.log(Logger::Broadcast::ProcessStart, "to: " + path + fileName + ".vtk");
        logger.log(Logger::Broadcast::ProcessEnd, "");

        const auto map = std::find_if(p_neighborParts.begin(), p_neighborParts.end(), [&part](const auto& pair) { return &(*pair.first) == &part; });
        if (map == p_neighborParts.end()) {
            logger.log(Logger::Broadcast::Warning, "No connection found to export.");
            return 0;
        }

        const auto start_t = clock();
        auto file = exporting::vtk::File(path, fileName, "vtk");

        const auto neighborhoodSizeAttrName = "neighborhood-size";
        file[neighborhoodSizeAttrName].setSource(exporting::vtk::AttSource::Cell);
        file[neighborhoodSizeAttrName].setAttributeType(exporting::vtk::AttType::Scalar);
        std::vector<int> neighborhoodSizes;
        neighborhoodSizes.reserve(map->second.size());

        if (p_localCoordinateSystem && !p_localCoordinateSystem->isGlobal()) {
            const auto& GCS = coordinate_system::CoordinateSystem::Global();
            for (const auto& neighborhood : map->second) {
                const auto point = GCS.convert(neighborhood->centre()->initialPosition().value< space::Point<3> >(), *p_localCoordinateSystem);
                file.appendCell(exporting::VTKCell::Vertex, exporting::vtk::convertors::convertToVertexes({ point }));
                neighborhoodSizes.emplace_back(neighborhood->neighbors().size());
            }
        } else {
            for (const auto& neighborhood : map->second) {
                const auto point = neighborhood->centre()->initialPosition().value< space::Point<3> >();
                file.appendCell(exporting::VTKCell::Vertex, exporting::vtk::convertors::convertToVertexes({ point }));
                neighborhoodSizes.emplace_back(neighborhood->neighbors().size());
            }
        }

        file[neighborhoodSizeAttrName].appendAttribute(neighborhoodSizes);
        file.assemble(true);

        const auto problemSize = p_neighborhoods.size();
        const auto dist = clock() - start_t;
        const auto duration = double(dist) / CLOCKS_PER_SEC;
        const auto durationText = report::date_time::duration::formated(duration, 3) ;

        logger.log(Logger::Broadcast::Process, std::to_string(problemSize) + " neighborhood exported.");
        logger.log(Logger::Broadcast::ProcessStart, "");
        logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText) + ".");
        logger.log(Logger::Broadcast::ProcessTiming, std::to_string( dist / problemSize ) + " CPU Clock per neighborhood.");
        logger.log(Logger::Broadcast::ProcessTiming, "or " + report::date_time::duration::formated( duration / problemSize, 3) + " per neighborhood.");
        logger.log(Logger::Broadcast::ProcessEnd, "");
        logger.log(Logger::Broadcast::ProcessEnd, "");
        return problemSize;
    } catch (std::exception e) {
        logger.log(Logger::Broadcast::Exeption, e.what());
        logger.log(Logger::Broadcast::Exeption, "Check if the \"" + path + "\" path exists on the hard drive.");
        logger.log(Logger::Broadcast::Exeption, "Check if the software has read and write access to the \"" + path + "\" path.");
        logger.log(Logger::Broadcast::Exeption, "If non of the above, please open an issue on https://github.com/alijenabi/RBS/issues." );
    } catch (...) {
        logger.log(Logger::Broadcast::Exeption, "An unknown exception (i.g., non std::exception) thrown.");
        logger.log(Logger::Broadcast::Exeption, "Try to rerun the program; if the same happens, the software is unstable. Please open an issue on https://github.com/alijenabi/RBS/issues." );
    }
    return 0;
}

const Part::Geometry &Part::geometry() const {
    if (p_geometry)
        return *p_geometry;

    static const auto space = geometry::Primary::Space();
    return space; // return 3D space if there is no geometry set!
}

const coordinate_system::CoordinateSystem &Part::local() const {
    return *p_localCoordinateSystem;
}

const std::string &Part::name() const {
    return p_name;
}

const Part::NeighborhoodPtrs &Part::neighborhoods() const {
    return p_neighborhoods;
}

size_t Part::initiateNeighborhoods() {
    using Logger = report::Logger;
    auto& logger = Logger::centre();
    logger.log(Logger::Broadcast::Process, "Initiating \"" + p_name + "\" neighborhoods.");

    if (!p_localCoordinateSystem || p_localCoordinateSystem->gridSize() == 0) {
        logger.log(Logger::Broadcast::Warning, "The local coordinate system does not have any grid points.");
        logger.log(Logger::Broadcast::Warning, "Make sure that you mesh the part before initiating the neighborhoods.");
        return 0;
    }

    const auto start_t = clock();
    p_mapper = p_localCoordinateSystem->getLinearindexMapper<std::shared_ptr<Neighborhood> >([](const space::Point<3>& point) {
        return std::make_shared<Neighborhood>(Node(point));
    });

    std::transform(p_mapper.begin(), p_mapper.end(), std::back_inserter(p_neighborhoods),
                   [](const std::pair<size_t, std::shared_ptr<Neighborhood> >& pair) {
                       return pair.second;
                   });

    const auto problemSize = p_localCoordinateSystem->pointsSize();
    logProcessTiming(clock() - start_t, problemSize, "neighborhood", "initiated");
    return problemSize;
}

size_t Part::searchInnerNeighbors(const double searchRadius) {
    const auto squareHorizonRadius = pow(searchRadius, 2);
    return searchInnerNeighbors( searchRadius, [&squareHorizonRadius](const space::Point<3>& centre, const space::Point<3>& neighbor) {
        return (neighbor.positionVector() - centre.positionVector()).lengthSquared() <= squareHorizonRadius;
    });
}

coordinate_system::CoordinateSystem &Part::local() {
    return *p_localCoordinateSystem;
}

std::string &Part::name() {
    return p_name;
}

Part::NeighborhoodPtrs &Part::neighborhoods() {
    return p_neighborhoods;
}

Part::NeighborhoodPtrs &Part::neighborhoods(const Part &neighborPart)
{
    const auto isSelf = this == &neighborPart;
    if (isSelf)
        return neighborhoods();

    const auto contactPair = std::find_if(p_neighborParts.begin(), p_neighborParts.end(), [&neighborPart](const auto& pair) { return &(*pair.first) == &neighborPart; });
    if (contactPair != p_neighborParts.end())
        return contactPair->second;

    throw std::runtime_error("No connection is defined for \"" + p_name + "\" Part and \"" + neighborPart.p_name + "\" Part");
}

size_t Part::searchInnerNeighbors(const double searchRadius, const Part::NeighborhoodSearchFunction &function) {
    using Logger = report::Logger;
    auto& logger = Logger::centre();

    logger.log(Logger::Broadcast::Process, "Searching for inner neighbors of \"" + p_name + "\" Part.");

    if (p_neighborhoods.empty()) {
        logger.log(Logger::Broadcast::Exeption, "The \"" + p_name + "\" Part is not meshed!");
        throw std::runtime_error("The \"" + p_name + "\" Part is not meshed!");
    }

    const auto start_t = clock();

    const auto searchVector = space::consts::one3D * searchRadius;
    size_t problemSize = 0;
    const auto search = [&](const auto& begin, const auto end) {
        std::for_each(begin, end, [&](const NeighborhoodPtr& neighborhood) {
            const auto centrePosition = neighborhood->centre()->initialPosition().value<space::Point<3> >();
            const auto neighborIndexes = p_localCoordinateSystem->getNeighborPointIndices(centrePosition, searchVector, function, centrePosition);

            const auto neighborhoodSize = neighborIndexes.size();
            problemSize += neighborhoodSize;

            if (neighborhoodSize) {
                const auto& localMapper = p_mapper;
                auto& neighborhoodNeighbors = neighborhood->neighbors();
                std::transform(neighborIndexes.begin(), neighborIndexes.end(), std::back_inserter(neighborhoodNeighbors),
                               [&localMapper](const coordinate_system::CoordinateSystem::LinearIndex& neighborCentreLinearindex){
                                   return localMapper.at(neighborCentreLinearindex);
                               });
            }
        });
    };
    search(p_neighborhoods.begin(), neighborhoods().end());

    logProcessTiming(clock() - start_t, problemSize, "neighhor", "found");
    return problemSize;
}

size_t Part::searchNeighborsWith(const Part &neighborPart, const double searchRadius, const bool update) {
    const auto squareHorizonRadius = pow(searchRadius, 2);
    return searchNeighborsWith(neighborPart, searchRadius, [&squareHorizonRadius](const space::Point<3>& centre, const space::Point<3>& neighbor) {
        return (neighbor.positionVector() - centre.positionVector()).lengthSquared() <= squareHorizonRadius;
    }, update);
}

size_t Part::searchNeighborsWith(const Part &neighborPart, const double searchRadius, const Part::NeighborhoodSearchFunction &function, const bool update) {
    const auto removeContact = [&neighborPart](NeighborParts& contacts) {
        const auto contactPair = std::find_if(contacts.begin(), contacts.end(), [&neighborPart](const auto& pair) { return &(*pair.first) == &neighborPart; });
        if(contactPair != contacts.end())
            contacts.erase(contactPair);
    };

    using Logger = report::Logger;
    auto& logger = Logger::centre();

    const auto checkMeshing = [&logger](const Part& part) {
        if (part.p_neighborhoods.empty()) {
            logger.log(Logger::Broadcast::Exeption, "The \"" + part.p_name + "\" Part is not meshed!");
            throw std::runtime_error("The \"" + part.p_name + "\" Part is not meshed!");
        }
    };

    const auto isSelfSearch = this == &neighborPart;
    if (isSelfSearch)
        return searchInnerNeighbors(searchRadius, function);

    logger.log(Logger::Broadcast::Process, "Searching for neighbors of \"" + p_name + "\" Part inside the \"" + neighborPart.p_name + "\" Part.");
    checkMeshing(*this);
    checkMeshing(neighborPart);

    if (&local() == &neighborPart.local()) {
        logger.log(Logger::Broadcast::Warning, "The local coordinate systems are the same; this will include the inner neighbors to the contact neighborhoods.");
        logger.log(Logger::Broadcast::Warning, "You can resolve this issue by defining different coordinate systems for each part of this connection.");
    }

    const auto start_t = clock();

    const auto contactPair = std::find_if(p_neighborParts.begin(), p_neighborParts.end(), [&neighborPart](const auto& pair) { return &(*pair.first) == &neighborPart; });
    const size_t oldSize = (update)? contactPair->second.size() : 0;

    auto& contactNeighbors = searchPotentialNeighborhoods(neighborPart, searchRadius * 1.25, update);
    const auto checkSize = update && contactNeighbors.size() != p_neighborhoods.size();

    const auto searchVector = space::consts::one3D * searchRadius;
    size_t problemSize = 0;
    const auto search = [&](const auto& begin, const auto end) {
        std::for_each(begin, end, [&](const NeighborhoodPtr& neighborhood) {
            const auto centrePosition = neighborhood->centre()->initialPosition().value<space::Point<3> >();
            const auto centrePositionOnNeighborPart = neighborPart.p_localCoordinateSystem->convert( centrePosition, *p_localCoordinateSystem);
            const auto neighborIndexes = neighborPart.p_localCoordinateSystem->getNeighborPointIndices(centrePositionOnNeighborPart, searchVector, function, centrePositionOnNeighborPart);

            const auto neighborhoodSize = neighborIndexes.size();
            problemSize += neighborhoodSize;

            if (neighborhoodSize) {
                const auto& localMapper = neighborPart.p_mapper;
                auto& neighborhoodNeighbors = neighborhood->neighbors();
                std::transform(neighborIndexes.begin(), neighborIndexes.end(), std::back_inserter(neighborhoodNeighbors),
                               [&localMapper](const coordinate_system::CoordinateSystem::LinearIndex& neighborCentreLinearindex){
                                   return localMapper.at(neighborCentreLinearindex);
                               });
            }
        });
    };
    search(contactNeighbors.begin(), contactNeighbors.end());

    // Search for empty neighbors and remove them.
    contactNeighbors.erase(std::remove_if(contactNeighbors.begin(), contactNeighbors.end(), [](const NeighborhoodPtr& neighborhood) {
        return neighborhood->neighbors().empty();
    }), contactNeighbors.end());

    if (checkSize) {
        if (oldSize) {
            const auto newSize = contactNeighbors.size();
            const auto rasio = double(newSize) / oldSize;
            if ( rasio < .75 || 1.25 < rasio) {
                logger.log(Logger::Broadcast::Warning, "The contact surface tolerance is higher than maximum allowed 15%.");
                logger.log(Logger::Broadcast::Warning, "Re-searching the neighborhoods, this time considering all of the neighborhood centers of \"" + p_name + "\" Part.");
                contactNeighbors.clear();
                removeContact(p_neighborParts);
                return searchNeighborsWith(neighborPart, searchRadius, function, false);
            }
        } else {
            logger.log(Logger::Broadcast::Warning, "Contact is not found.");
            logger.log(Logger::Broadcast::Warning, "No connection made between \"" + p_name + "\" Part and \"" + neighborPart.p_name + "\" Part.");
        }
    }

    if (contactNeighbors.empty())
        removeContact(p_neighborParts);

    logProcessTiming(clock() - start_t, problemSize, "neighhor", "found");
    return problemSize;
}

void Part::setGeometry(const geometry::Combined &shape) {
    p_geometry = std::make_shared<geometry::Combined>(shape);
}

void Part::setGeometry(const geometry::Primary &shape) {
    p_geometry = std::make_shared<geometry::Primary>(shape);
}

void Part::logProcessTiming(const long dist, const size_t problemSize, const std::string &item, const std::string action) const noexcept {
    using Logger = report::Logger;
    auto& logger = Logger::centre();

    const auto duration = double(dist) / CLOCKS_PER_SEC;
    const auto durationText = report::date_time::duration::formated(duration, 3);
    if (problemSize) {
        logger.log(Logger::Broadcast::ProcessStart, std::to_string(problemSize) + " " + item + ((problemSize == 1)? " ": "s ") + action + ".");
        logger.log(Logger::Broadcast::ProcessStart, "");
        logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText)  + ".");
        logger.log(Logger::Broadcast::ProcessTiming, std::to_string( dist / problemSize )  + " CPU Clock per " + item + ".");
        logger.log(Logger::Broadcast::ProcessTiming, report::date_time::duration::formated(duration / problemSize, 3) + " per " + item + ".");
        logger.log(Logger::Broadcast::ProcessEnd, "");
        logger.log(Logger::Broadcast::ProcessEnd, "");
    } else {
        logger.log(Logger::Broadcast::Warning, "No " + item + " found.");
        logger.log(Logger::Broadcast::ProcessStart, "");
        logger.log(Logger::Broadcast::ProcessTiming, "Done in " + durationText + ".");
        logger.log(Logger::Broadcast::ProcessEnd, "");
    }
}

void Part::logFileMissing(const std::string &path) const noexcept {
    using Logger = report::Logger;
    auto& logger = Logger::centre();

    logger.log(Logger::Broadcast::Exeption, "Check if the \"" + path + "\" path exists on the hard drive.");
    logger.log(Logger::Broadcast::Exeption, "Check if the software has read and write access to the \"" + path + "\" path.");
    logger.log(Logger::Broadcast::Exeption, "If non of the above, please open an issue on https://github.com/alijenabi/RBS/issues." );
}

void Part::logUnknownExeption() const noexcept {
    using Logger = report::Logger;
    auto& logger = Logger::centre();

    logger.log(Logger::Broadcast::Exeption, "An unknown exception (i.g., non std::exception) thrown.");
    logger.log(Logger::Broadcast::Exeption, "Try to re-run the program; if the same happens, the software is unstable. Please open an issue on https://github.com/alijenabi/RBS/issues." );
}

Part::NeighborhoodPtrs &Part::searchPotentialNeighborhoods(const Part &neighborPart, const double searchRadius, const bool update) {
    const auto include = [](const auto& source, NeighborhoodPtrs& target) {
        std::transform(source.begin(), source.end(), std::back_inserter(target),
                       [](const auto& neighborhood){
                           return std::make_shared<Neighborhood>(neighborhood->centre());
                       });
    };

    // for self inner neighborhood search we need all of the neighborhoods.
    if (&neighborPart == this)
        throw std::runtime_error("Cannot stablish a connection between a Part and itself.");

    const auto contactPair = std::find_if(p_neighborParts.begin(), p_neighborParts.end(), [&neighborPart](const auto& pair) { return &(*pair.first) == &neighborPart; });

    // If not found, then we need to search for all of the neighborhoods.
    if (contactPair == p_neighborParts.end()) {
        include(p_neighborhoods, p_neighborParts[std::shared_ptr<const Part>(&neighborPart, [](auto&){})]);
        const auto contactPair = std::find_if(p_neighborParts.begin(), p_neighborParts.end(), [&neighborPart](const auto& pair) { return &(*pair.first) == &neighborPart; });
        return contactPair->second;
    }

    if (!update) {
        include(p_neighborhoods, contactPair->second);
        return contactPair->second;
    }

    std::set<size_t> potentialIndices = {};
    const auto radiusVector = searchRadius * space::consts::one3D;
    const auto searchFunction = [](const auto&, const auto&) { return true; };
    std::for_each(contactPair->second.begin(), contactPair->second.end(), [&radiusVector, &searchFunction, &potentialIndices, this](const NeighborhoodPtr& neighborhood) {
        const auto& centrePosition = neighborhood->centre()->initialPosition().value<space::Point<3> >();
        const auto& currentIndices = p_localCoordinateSystem->getNeighborPointIndices(centrePosition, radiusVector, searchFunction, centrePosition);

        potentialIndices.insert(currentIndices.begin(), currentIndices.end());
    });

    contactPair->second.clear();
    std::transform(potentialIndices.begin(), potentialIndices.end(), std::back_inserter(contactPair->second),
                   [this](const coordinate_system::CoordinateSystem::LinearIndex neighborCentreLinearindex){
                       const auto& centre = p_mapper.at(neighborCentreLinearindex)->centre();
                       return std::make_shared<Neighborhood>(centre);
                   });

    return contactPair->second;
}

void Part::sortNeighborhoods() {
    const auto compare = [](const auto& n1, const auto& n2) {
        return n1->neighbors().size() < n2->neighbors().size();
    };

    using Logger = report::Logger;
    auto& logger = Logger::centre();
    logger.log(Logger::Broadcast::Process, "Sorting the \"" + p_name + "\" Part neighborhoods.");

    if (p_neighborhoods.empty()) {
        logger.log(Logger::Broadcast::Warning, "No neighborhood found to export.");
        return;
    }

    const auto start_t = clock();
    std::sort(p_neighborhoods.begin(), p_neighborhoods.end(), compare);

    auto durationText = report::date_time::duration::formated(double(clock() - start_t) / CLOCKS_PER_SEC, 6) ;
    logger.log(Logger::Broadcast::ProcessStart, "");
    logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText) + ".");
    logger.log(Logger::Broadcast::ProcessEnd, "");

    std::for_each(p_neighborParts.begin(), p_neighborParts.end(), [&compare, &logger, this](auto& pair) {
        logger.log(Logger::Broadcast::Process, "Sorting the \"" + p_name + "\" Part neighborhoods with " + pair.first->p_name + " Part.");

        const auto start_t = clock();
        std::sort(pair.second.begin(), pair.second.end(), compare);

        const auto durationText = report::date_time::duration::formated(double(clock() - start_t) / CLOCKS_PER_SEC, 6) ;
        logger.log(Logger::Broadcast::ProcessStart, "");
        logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText) + ".");
        logger.log(Logger::Broadcast::ProcessEnd, "");
    });

    durationText = report::date_time::duration::formated(double(clock() - start_t) / CLOCKS_PER_SEC, 6) ;
    logger.log(Logger::Broadcast::ProcessStart, "");
    logger.log(Logger::Broadcast::ProcessTiming, "In total: done in " + ((durationText.empty())? " less than a nanosecond" : durationText) + ".");
    logger.log(Logger::Broadcast::ProcessEnd, "");
}

} // namespace rbs::configuration

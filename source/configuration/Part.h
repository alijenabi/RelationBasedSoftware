//
//  Part.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef PART_H
#define PART_H

#include "Neighborhood.h"
#include "../coordinate_system/CoordinateSystem.h"
#include "../exporting/VTK.h"
#include "../geometry/Combined.h"
#include "../report/Logger.h"
#include <map>
#include <memory>

/**
 * The configuration namespace provides the means to create configurations and use them.
 */
namespace rbs::configuration {

/**
 * @brief Part stores the configuration information both at initialization time and during the simulation.
 *          It is also responsible for the serialization of configuration at each timestep.
 */
class Part
{
public:
    using NeighborhoodPtr = std::shared_ptr<Neighborhood>;
    using NeighborhoodPtrs = std::vector<NeighborhoodPtr>;

    using Geometry = geometry::Geometry;
    using GeometryPtr = std::shared_ptr<Geometry>;

    using CS = coordinate_system::CoordinateSystem;

    using NeighborhoodSearchFunction = std::function<bool(const space::Point<3>&, const space::Point<3>&)>;

    /**
     * @brief Part Constructor.
     * @param name The name of the part.
     */
    explicit Part(const std::string& name);

    /**
     * @brief Part Constructor.
     * @param name The name of the part.
     * @param coordinateSystem The local coordinate system.
     */
    explicit Part(const std::string& name, CS& coordinateSystem);

    /**
     * @brief Exports initial configuration to Vtk file.
     * @note The part name plus "Part" keyword will be used for exportation.
     * @param path The folder path with last folder seperator.
     * @return The number of exported neighborhoods.
     */
    size_t exportConfiguration(const std::string& path) const noexcept;

    /**
     * @brief Exports initial configuration to Vtk file.
     * @param path The folder path with last folder seperator.
     * @param fileName The file name.
     * @return The number of exported neighborhoods.
     */
    size_t exportConfiguration(const std::string& path, const std::string& fileName) const noexcept;

    /**
     * @brief Exports the neighborhoods that their centre is a checkpoint for verifying the neighborhood search correctness.
     * @note The checkpoints are the neighborhoods that have different number of neighbors.
     * @note The part name plus "Part" keyword will be used for exportation.
     * @note The file name will be the Part name plus "_checkpoint_neighborhoods" extention.
     * @param path The folder path with last folder seperator.
     * @return The number of exported neighborhoods.
     */
    size_t exportCheckpointNeighborhoods(const std::string& path) const;

    /**
     * @brief Exports the neighborhoods that their centre is a checkpoint for verifying the neighborhood search correctness.
     * @note The checkpoints are the neighborhoods that have different number of neighbors.
     * @note The file will be created with "_checkpoint_neighborhoods" extention after the fileName.
     * @param path The folder path with last folder seperator.
     * @param fileName The file name.
     * @return The number of exported neighborhoods.
     */
    size_t exportCheckpointNeighborhoods(const std::string& path, const std::string& fileName) const;

    /**
     * @brief Exports the neighborhood nodes that are in connection with other part and their number of neighbors.
     * @note The part name plus "Part" keyword will be used for exportation.
     * @param part The neighborhood part.
     * @param path The folder path with last folder seperator.
     * @return The number of exported neighborhoods.
     */
    size_t exportConnection(const Part& part, const std::string& path) const noexcept;

    /**
     * @brief Exports the neighborhood nodes that are in connection with other part and their number of neighbors.
     * @param part The neighborhood part.
     * @param path The folder path with last folder seperator.
     * @param fileName The file name.
     * @return The number of exported neighborhoods.
     */
    size_t exportConnection(const Part& part, const std::string& path, const std::string& fileName) const noexcept;

    /**
     * @brief Gives the gometry of the part.
     * @return The goemetry of the part if found, 3D space if not found.
     */
    const Geometry& geometry() const;

    /**
     * @brief Gives access to the local coordinate system;
     */
    const coordinate_system::CoordinateSystem& local() const;

    /**
     * @brief Gives the name of the part.
     */
    const std::string& name() const;

    /**
     * @brief Gives direct access to the neighborhoods.
     */
    const NeighborhoodPtrs& neighborhoods() const;

    /**
     * @brief Initiates the neighborhoods.
     * @return The number of neighborhoods.
     */
    size_t initiateNeighborhoods();

    /**
     * @brief Gives direct access to the local coordinate system;
     */
    coordinate_system::CoordinateSystem& local();

    /**
     * @brief Gives direct access to the name.
     */
    std::string& name();

    /**
     * @brief Gives direct access to the neighborhoods.
     */
    NeighborhoodPtrs& neighborhoods();

    /**
     * @brief Gives direct access to the connection neighborhoods.
     * @brief It will return this part neighborhoods if given neighborhood part is the same as this part.
     */
    NeighborhoodPtrs& neighborhoods(const Part& neighborPart);

    /**
     * @brief Performs an spherical inner neighborhood search.
     * @param searchRadius The radius of the spherical neighborhood.
     * @return The number of total found neighbors (i.e., the number of one ended bonds).
     */
    size_t searchInnerNeighbors(const double searchRadius);

    /**
     * @brief Performs a neighborhood search finding the neighbors of the neighborhood inside this part.
     * @param searchRadius The cubic neighborhood radius.
     * @param function The function that desides whether the neighbor point in the cubic neighborhood is a neighbor.
     * @return The number of total found neighbors (i.e., the number of one ended bonds).
     */
    size_t searchInnerNeighbors(const double searchRadius, const NeighborhoodSearchFunction& function);

    /**
     * @brief Perfoms a spherical neighborhood search on the neihbour Part neighborhood centers.
     * @note Use update only if the relative displacement between two part is small compare to the base part grid spacing.
     * @param neighborPart The neighbor Part.
     * @param searchRadius The radius of the spherical neighborhood.
     * @param update Whether to update the current connection or search on all the nodes.
     * @return The number of total found neighbors (i.e., the number of one ended bonds).
     */
    size_t searchNeighborsWith(const Part& neighborPart, const double searchRadius, const bool update = false);

    /**
     * @brief Perfoms a neighborhood search on the neihbour Part neighborhood centers.
     * @note Use update only if the relative displacement between two part is small compare to the base part grid spacing.
     * @param neighborPart The neighbor Part.
     * @param searchRadius The cubic neighborhood radius.
     * @param function The function that desides whether the neighbor point in the cubic neighborhood is a neighbor.
     * @param update Whether to update the current connection or search on all the nodes.
     * @return The number of total found neighbors (i.e., the number of one ended bonds).
     */
    size_t searchNeighborsWith(const Part& neighborPart, const double searchRadius, const NeighborhoodSearchFunction& function, const bool update = false);

    /**
     * @brief Sets the shape.
     */
    void setGeometry(const geometry::Combined& shape);

    /**
     * @brief Sets the shape.
     */
    void setGeometry(const geometry::Primary& shape);

protected:

    /**
     * @brief Logs the timing of the process.
     * @param dist The CPU clock distance from the start of the process to the end of it.
     * @param problemSize The number of items that are processed.
     * @param item The name of the item.
     * @param action Action done on the items.
     */
    void logProcessTiming(const long dist, const size_t problemSize, const std::string& item, const std::string action) const noexcept;

    /**
     * @brief Logs the exeption message on the file missing.
     */
    void logFileMissing(const std::string& path) const noexcept;

    /**
     * @brief Logs the unknown exeption message.
     */
    void logUnknownExeption() const noexcept;

    /**
     * @brief Searches the neighborhoods of this part and finds the potential neighborhoods for searching bonds among them.
     * @param part The other part.
     */
    NeighborhoodPtrs& searchPotentialNeighborhoods(const Part& part, const double searchRadius, const bool update);

    /**
     * @brief Sort the Neighborhoods.
     */
    void sortNeighborhoods();

private:

    using CSPtr = std::shared_ptr<CS>;
    using Mapper = std::map<size_t, std::shared_ptr<Neighborhood> >;
    using NeighborParts = std::map<std::shared_ptr<const Part>, NeighborhoodPtrs>;

    GeometryPtr p_geometry;             // The geometry of the part ( if null, then all of the points inside the geometry will be taken )
    CSPtr p_localCoordinateSystem;      // The local coordinate of the part.
    Mapper p_mapper;                    // The mapper
    std::string p_name;                 // The name of the part.
    NeighborhoodPtrs p_neighborhoods;   // The included neighborhoods of the part configuration.
    NeighborParts p_neighborParts;      // The neighborhood with other parts neighborhoods.
};

} // namespace rbs::configuration

#endif // PART_H

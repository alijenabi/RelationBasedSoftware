//
//  CoordinateSystem.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 01.02.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include "Axis.h"
#include "convertors.h"
#include <functional>
#include <map>
#include <vector>

#include "../space/Index.h"
#include "../space/Point.h"

//namespace rbs::space {

//template<unsigned char SpaceDimension> class Index;
//template<unsigned char SpaceDimension> class Point;
//template<unsigned char SpaceDimension> class Vector;

//}

/**
 * The coordinate_system namespace provides the means to work with different coordinate systems.
 */
namespace rbs::coordinate_system {

class CoordinateSystem
{
public:

    using Index = space::Index<3>;
    using IndexSet = std::set<Index>;
    using LinearIndex = size_t;
    using LinearIndices = std::set<LinearIndex>;

    using Point = space::Point<3>;
    using Points = std::vector<Point>;
    using OptionalPoint = std::shared_ptr<Point>;
    using Vector = space::Vector<3>;
    using Vectors = std::vector<Vector>;
    using OptionalVector = std::shared_ptr<Vector>;

    using CoordinateSystemPtr = std::shared_ptr<CoordinateSystem>;
    using Children = std::vector<CoordinateSystemPtr>;

    enum Type {
        Custom,         // Denotes the custom Coordinate System (CS).
        Cartesian,
        Cylindrical,
        Spherical
    };

    /**
     * @brief Converting forward and backward between the coordinte systems requres mapping between two spaces (i.e. 3D vector stapce) regardless of the direction.
     * @param 1st The vector in parent/local coordinate system (in forward/backward mapping).
     * @param 2nd The origin of the local coordinate system in the parent coordinate system.
     * @param 3rd The i-vector of the local coordinate system in the parent coordinate system.
     * @param 4th The j-vector of the local coordinate system in the parent coordinate system.
     * @param 5th The k-vector of the local coordinate system in the parent coordinate system.
     * @return The vector in local/parent coordinate system (in forward/backward mapping).
     */
    using Mapping = std::function<Point(const Point&, const Point&, const Vector&, const Vector&, const Vector&)>;

    /**
     * @brief Constructs a CoordinateSystem.
     * @param parent The parent of the coordinate system.
     * @param origin The origin of the new coordinate system in its parent coordinate system.
     * @param i The i vector of the ne coordinate system in its parent coordinate system.
     * @param j The j vector of the ne coordinate system in its parent coordinate system.
     * @param k The k vector of the ne coordinate system in its parent coordinate system.
     * @param forwardMapping The forward mapping function.      parent points -> local points
     * @param backwardMapping The backward mapping function.    local points -> parent points
     */
    CoordinateSystem(CoordinateSystem & parent,
                     const Point& origin,
                     const Vector& i, const Vector j, const Vector k,
                     const Mapping& forwardMapping, const Mapping& backwardMapping);

    ///@ {
    /**
     * @details Static members providing access to the coordinate system tree.
     */

    /**
     * @brief Give the Global Coordinate system (cartezian).
     */
    static CoordinateSystem& Global();

    /**
     * @brief Gives the coordinate system tree.
     */
    static std::string tree();

    /**
     * @brief Whether the given coordinate system is incuded in the coordinate system tree or not.
     */
    static bool isIncludedInTree(CoordinateSystem& cs);

    ///@ }

    /**
     * @brief Gives direct access to the axes direction vectors.
     * @param index The index of the axes.
     */
    const Vector& axesDirectionVector(const char index) const;

    /**
     * @brief Converts a point from a source coordinate system to this coordinate system.
     * @param point A point in source coordinate system.
     * @param source The source coordinate system.
     */
    Point convert(const Point& point, const CoordinateSystem& source) const;

    /**
     * @brief Computes the number of posible indices.
     * @param includeChildrenGrids Whether to include the children grids too or not.
     */
    size_t gridSize(const bool includeChildrenGrids = false) const;

    /**
     * @brief Computes the number of grids on a specific axis.
     * @param index The index of the axis.
     * @throws If the index is equal or bigger than 3.
     */
    size_t gridSize(const unsigned char index) const;

    /**
     * @brief Gives all the grid point.
     * @note All the exported points will be in the current coordinate system (i.e., the children points will be converted).
     * @param includeChildrenGrids Whether to include the children grids too or not.
     */
    Points getAllGridPoints(const bool includeChildrenGrids = false) const;

    /**
     * @brief Gives all the points.
     * @note All the exported points will be in the current coordinate system (i.e., the children points will be converted).
     * @param includeChildrenPoints Whether to include the children points too or not.
     */
    Points getAllPoints(const bool includeChildrenPoints = false) const;

    /**
     * @brief Gets the grid point at specific 3D index.
     * @param index The 3D index.
     * @throws If the 3D index does not present a grid point.
     */
    Point gridPointAt(const Index & index) const;

    /**
     * @brief Gets the grid point at specific linear index.
     * @param index The linear index.
     * @throws If the linear index does not present a grid point.
     */
    Point gridPointAt(const LinearIndex& linearIndex) const;

    /**
     * @brief Gets the cuboid neighbor points.
     * @note The cuboid neighborhood in cylindrical and spherical coordinates systems does not shape as a cuboid.
     * @param centre The centre of the neighborhood.
     * @param radiusVector The cuboid radius of the neighborhood (i.e., from the centre to one of the neighborhood corner).
     */
    Points getNeighborPoints(const Point& centre, const Vector radiusVector) const;

    /**
     * @brief Extracts the neighbor points from the cuboid neighborhood.
     * @param centre The centre of the neighborhood.
     * @param radiusVector The cuboid radius of the neighborhood (i.e., from the centre to one of the neighborhood corner).
     * @param include A function that receives centre and neighbor and returns a boolean indicating whether to add the point to the neighborhood or not.
     */
    Points getNeighborPoints(const Point& centre, const Vector radiusVector, const std::function<bool(Point, Point)>& include) const;

    /**
     * @brief Gets the neighbor points.
     * @note The centre of neighborhood will be passed to the include function.
     * @param searchCentre The centre of the neighborhood that will be used for extracting the cuboid neighborhood.
     * @param radiusVector The cuboid radius of the neighborhood (i.e., from the centre to one of the neighborhood corner).
     * @param include A function that receives centre and neighbor and returns a boolean indicating whether to add the point to the neighborhood or not.
     * @param neighborhoodCentre The centre of the neighborhood.
     */
    Points getNeighborPoints(const Point& searchCentre, const Vector radiusVector, const std::function<bool(Point, Point)>& include, const Point& neighborhoodCentre) const;

    /**
     * @brief Gets the neighbors indexes of a neighborhood.
     * @note The centre of neighborhood will be passed to the include function.
     * @param searchCentre The centre of the neighborhood that will be used for extracting the cuboid neighborhood.
     * @param radiusVector The cuboid radius of the neighborhood (i.e., from the centre to one of the neighborhood corner).
     * @param include A function that receives centre and neighbor and returns a boolean indicating whether to add the point to the neighborhood or not.
     *                  function @param 1st The centre of the neighborhood.
     *                           @param 2nd The current neighbor.
     *                           @return True if inside the neighborhood, false otherwise.
     * @param neighborhoodCentre The centre of the neighborhood.
     */
    LinearIndices getNeighborPointIndices(const Point& searchCentre, const Vector radiusVector, const std::function<bool(const Point&, const Point&)>& include, const Point& neighborhoodCentre) const;

    /**
     * @brief Creates a mapper to the point's linear indexes
     * @tparam T The type of the map values.
     * @param constructor A function that recives the points location and creates the map vlaues.
     * @return A mapper from linear indexes to the custom values.
     */
    template<typename T>
    std::map<LinearIndex, T> getLinearindexMapper(const std::function<T(const space::Point<3>& point)> constructor) {
        std::map<LinearIndex, T> mapper;
        std::transform(p_indices.begin(), p_indices.end(), std::inserter(mapper, mapper.begin()), [&constructor, this](const LinearIndex& linearIndex) {
            const auto index = convertInverce(linearIndex);
            const auto point = space::Point<3>{p_axes[0].at(index[0]), p_axes[1].at(index[1]), p_axes[2].at(index[2])};
            return std::pair(linearIndex, constructor(point));
        });
        return mapper;
    }

    /**
     * @brief Gets the point at index.
     * @param index The point index, from 0 to point size - 1.
     * @throws If the index is out of range.
     */
    OptionalPoint getPoint(const size_t & index) const;

    /**
     * @brief Gets the point at specific 3D index.
     * @param index The 3D index.
     */
    OptionalPoint getPointAt(const Index & index) const;

    /**
     * @brief Gets the point at specific linear index.
     * @param index The linear index.
     */
    OptionalPoint getPointAt(const LinearIndex& linearIndex) const;

    /**
     * @brief Whether the coordinate system is the global coordinate system or not.
     * @throws If this is a global coordinate system but not the Global coordintate system.
     */
    bool isGlobal() const;

    /**
     * @brief Gives direct access to the origion.
     * @throws Incase of attempting to change the global coordinate system origin.
     */
    const Point& origin() const;

    /**
     * @brief Appends a local coordinate system to this one.
     * @note The i, j, and k can be any vector in the parent (i.g., this) coordinate system.
     * @param origin The origin of the local coordinate system in this coordinate system.
     * @param i The i-vector of the local coordinate system in this coordinate system.
     * @param j The j-vector of the local coordinate system in spherical coordinate system.
     * @param k The k-vector of the local coordinate system in spherical coordinate system.
     * @param forwardMapping The forward mapping function.      parent points -> local points
     * @param backwardMapping The backward mapping function.    local points -> parent points
     * @return The address to the new local coordinate system inside the coordinate system tree.
     */
    CoordinateSystem& appendCustomLocal(const Point& origin,
                                        const Vector& i, const Vector j, const Vector k,
                                        const Mapping& forwardMapping, const Mapping& backwardMapping);

    /**
     * @brief Appends a local coordinate system to this one.
     * @param type The type of the coordinate system.
     * @return The address to the new local coordinate system inside the coordinate system tree.
     * @throws If the given local coordinate system type or this coordinate system type is Type::Custom.
     * @throws If the given local coordinate system type or this coordinate system type is unknown.
     */
    CoordinateSystem& appendLocal(const Type type);

    /**
     * @brief Appends a local coordinate system to this one.
     * @note The i, j, and k can be any vector in the parent (i.g., this) coordinate system.
     * @param type The type of the coordinate system.
     * @param i The i-vector of the local coordinate system in this coordinate system.
     * @param j The j-vector of the local coordinate system in spherical coordinate system.
     * @param k The k-vector of the local coordinate system in spherical coordinate system.
     * @return The address to the new local coordinate system inside the coordinate system tree.
     * @throws If the given local coordinate system type or this coordinate system type is Type::Custom.
     * @throws If the given local coordinate system type or this coordinate system type is unknown.
     */
    CoordinateSystem& appendLocal(const Type type, const Vector& i, const Vector j, const Vector k);

    /**
     * @brief Appends a local coordinate system to this one.
     * @note The i, j, and k can be any vector in the parent (i.g., this) coordinate system.
     * @param type The type of the coordinate system.
     * @param origin The origin of the local coordinate system in this coordinate system.
     * @param i The i-vector of the local coordinate system in this coordinate system.
     * @param j The j-vector of the local coordinate system in spherical coordinate system.
     * @param k The k-vector of the local coordinate system in spherical coordinate system.
     * @return The address to the new local coordinate system inside the coordinate system tree.
     * @throws If the given local coordinate system type or this coordinate system type is Type::Custom.
     * @throws If the given local coordinate system type or this coordinate system type is unknown.
     */
    CoordinateSystem& appendLocal(const Type type,const Point& origin, const Vector& i, const Vector j, const Vector k);

    /**
     * @brief Provides direct access to the axes of the coordinate system.
     * @warning Changing the axes' values will not rearrange the point indices.
     */
    Axis* axes();

    /**
     * @brief Gives direct access to the axes direction vectors.
     * @param index The index of the axes.
     * @throws Incase of attempting to change the global coordinate system origin.
     */
    Vector& axesDirectionVector(const char index);

    /**
     * @brief Removes all the points.
     * @note This does not remove the node grid but the indices.
     */
    void clearPoints();

    /**
     * @brief Removes all the grids.
     * @warning This will remove all the points as well since the nodes can only sit on the grid net.
     */
    void clearGrids();

    /**
     * @brief Removes the given value from the axis if exist.
     * @param point The point to remove.
     */
    void exclude(const Point& point) noexcept;

    /**
     * @brief Removes the given value from the axis if exist.
     * @param vector The point position vector to remove.
     */
    void exclude(const Vector& vector) noexcept;

    /**
     * @brief Removes the grid points to the point list if remove(grid_point) == true.
     * @param remove A binary function
     * @param excludeFromChildrenGrids Whether to exclude from the children grids or not.
     */
    void exclude(std::function<bool(const Point&)> remove, const bool excludeFromChildrenGrids = false);

    /**
     * @brief Adds new value to the axis.
     * @param point The point to add.
     */
    void include(const Point& point);

    /**
     * @brief Adds new value to the axis.
     * @param vector The point position vector to add.
     */
    void include(const Vector& vector);

    /**
     * @brief Adds the grid points to the point list if add(grid_point) == true.
     * @param add A binary function
     * @param includeToChildrenGrids Whether to inlucde to the children grids too or not.
     */
    void include(std::function<bool(const Point&)> add, const bool includeToChildrenGrids = false);

    /**
     * @brief Gives direct access to the origion.
     * @throws Incase of attempting to change the global coordinate system origin.
     */
    Point& origin();

    /**
     * @brief Computes the number points currently included.
     * @param includeChildrenGrids Whether to include the children points too or not.
     */
    size_t pointsSize(const bool includeChildrenPoints = false) const;

private:

    /**
     * @brief Constroctor.
     * @note This will only be used for the global coordinate system and should not be used anywhere else.
     * @note This is private since the global coordinate system is a Singleton.
     */
    CoordinateSystem();

    /**
     * @brief Copy Constroctor.
     * @note This is private since copying coordinate systems should only be handled here;
     *        otherwise, the coordinate system tree will not be a Singleton tree anymore.
     */
    CoordinateSystem(const CoordinateSystem& other);

    /**
     * @brief Assignment Operation.
     * @note This is private since assiging coordinate systems should only be handled here;
     *        otherwise, the coordinate system tree will not be a Singleton tree anymore.
     */
    CoordinateSystem& operator =(CoordinateSystem&);

    /**
     * @brief The converting coefficient for the second order.
     */
    static LinearIndex c1();

    /**
     * @brief The converting coefficient for the third order.
     */
    static LinearIndex c2();

    /**
     * @brief Converts the 3D index of the point's linear index.
     * @param index The 3D index.
     */
    static LinearIndex convert(const Index& index);

    /**
     * @brief Converts the linear index to the point's 3D index.
     * @param index The linear index.
     */
    static Index convertInverce(const LinearIndex index);

    /**
     * @brief Provides the string representation of the tree of this coordinate system tree.
     */
    std::string branchInfo(const size_t tab = 0) const;

    /**
     * @brief Composes the position vector from the individual axis.
     * @param index The index of composition.
     */
    OptionalVector composePositionVectorAt(const Index & index) const;

    /**
     * @brief Forwards the linear indices that are bigger than index and is specific direction only.
     * @note This will use only and only if we need to adjust the indices due to appending new value ot the axes.
     * @param index The index of the point.
     * @param directions The directions to forward.
     */
    void forwardLinearIndices(const Index& index, bool * directions);

    /**
     * @brief Whether the given coordinate system is under this coordinate system in the tree.
     */
    bool includes(const CoordinateSystem& cs) const;

    Axis p_axes[3];                 // Three axes that hold the vector components in each direction.
    Children p_children;            // The children of this coordinte system.
    Vector p_i;                     // The i vector of this coordinate system in its parent coordinate system.
    Vector p_j;                     // The j vector of this coordinate system in its parent coordinate system.
    LinearIndices p_indices;        // The linaer indices of the existing points.
    Vector p_k;                     // The k vector of this coordinate system in its parent coordinate system.
    Point p_origin;                 // The origin of the coordinate system in its parent coordinate system.
    CoordinateSystemPtr p_parent;   // The parent of this coordinate system, nullptr if its Global.
    Type p_type;                    // The type of the coordinate system.

    /**
     * @brief Converts the point location from the parent perspective to the local perspective.
     * @param _0 The position of the point in the parent coordinate system.
     * @param _1 The origin of this coordinate system in the parent coordinate system.
     * @param _2 The i vector of this coordinate system in the parent coordinate system.
     * @param _3 The k vector of this coordinate system in the parent coordinate system.
     * @return The position vector of the point in local (this) coordinate system.
     */
    Mapping p_forwardMapping;

    /**
     * @brief Converts the point location from the local perspective to the parent perspective.
     * @param _0 The position of the point in the local (this) coordinate system.
     * @param _1 The origin of this coordinate system in the parent coordinate system.
     * @param _2 The i vector of this coordinate system in the parent coordinate system.
     * @param _3 The k vector of this coordinate system in the parent coordinate system.
     * @return The position vector of the point in the parent coordinate system.
     */
    Mapping p_backwardMapping;

};

} // namespace rbs::coordinate_system

/**
 * The file_system namespace provides the means to handle files on the operating system file system.
 * The vtk namespace provides easy access to all of the namespaces realted to VTK files.
 * The convertors namespace provides the functionality to convert information to the format recognizable by the VTK files.
 */
namespace rbs::exporting::vtk::convertors {

using VTKVertex = std::vector<double>;
using VTKVertexes = std::vector<VTKVertex>;

/**
 * @brief Converts the coordinate system points to exportable vtk vertexes.
 */
VTKVertexes convertToVertexes(const coordinate_system::CoordinateSystem::Points& points);

} // namespace rbs::exporting::vtk

#endif // COORDINATESYSTEM_H

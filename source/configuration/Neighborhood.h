//
//  Neighborhood.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include "Node.h"
#include "../variables/Variant.h"
#include <memory>
#include <map>
#include <vector>

/**
 * The configuration namespace provides the means to create configurations and use them.
 */
namespace rbs::configuration {

/**
 * @brief All of the nonlocal numerical methods' constitutive models require a set of nodes to
 *          implement the governing equation. These set of nodes are known as neighborhood
 *          where the constitutive model defines the relation between them.
 * @note Depending on the constitutive model, Neighborhood may require storing different properties along with the neighbors.
 * @note If you need to introduce new form of neighborhood, use this class as a public base class and then implement the required properties.
 * @warning Avoid using this class directly in your simulation. For each supported constitutive model, there should be subclass of this class.
 */
class Neighborhood
{
public:
    using InitialDistance   = double;
    using NodePtr           = std::shared_ptr<Node>;
    using NeighborPtr       = std::shared_ptr<Neighborhood>;
    using Neighbors         = std::vector<NeighborPtr>;

    using Status            = variables::Variant;
    using StatusIndex       = int;

    /**
     * @brief The constructor.
     * @param centre The centre of neighborhood.
     */
    explicit Neighborhood(const Node& centre);

    /**
     * @brief The constructor.
     * @param centre Pointer to the centre of neighborhood.
     */
    explicit Neighborhood(const NodePtr& centre);

    /**
     * @brief The default copy constructor.
     */
    Neighborhood(const Neighborhood& other) = default;

    /**
     * @brief The move constructor.
     */
    Neighborhood(const Neighborhood&& other);

    /**
     * @brief Move/copy assign operator.
     * @param other The other Space to move or copy.
     * @return The new Space.
     */
    Neighborhood &operator =(Neighborhood other);

    /**
     * @brief Gives the centre of neighborhood.
     */
    const NodePtr& centre() const;

    /**
     * @brief Whether the neighborhood includes any neighbors or not.
     */
    bool hasNeighbors() const;

    /**
     * @brief Whether the neighbor has status at the given index.
     */
    bool hasStatus(const NeighborPtr& neighbor, const StatusIndex index) const;

    /**
     * @brief Gives the neighbors of the neighborhood at once.
     */
    const Neighbors& neighbors() const;

    /**
     * @brief Gives the size of neighborhood.
     */
    size_t size() const;

    /**
     * @brief Gives the status of the neighbor (i.e., bond).
     * @param neighbor The other end of the bond.
     * @param index The index of the bond status.
     * @return The saves status.
     * @throws If there is no bond status saved or if the bond status has no status on the given index.
     */
    const Status& status(const NeighborPtr& neighbor, const StatusIndex index) const;

    /**
     * @brief Gives the centre of neighborhood.
     */
    NodePtr& centre();

    /**
     * @brief Gives the neighbors of the neighborhood at once.
     */
    Neighbors& neighbors();

    /**
     * @brief Set the neighbor (i.e., bond) status.
     * @param neighbor The other end of the bond.
     * @param index The index of the status.
     * @param status The new bond status.
     */
    void setStatus(const NeighborPtr& neighbor, const StatusIndex index, const Status& status);

private:

    using StatusMap = std::map<StatusIndex, Status>;
    using Statuses = std::map<Neighborhood*, StatusMap>;

    /**
     * @brief Swaps the content of this object with an other objec of same type.
     * @param other The other instance.
     */
    void swap(Neighborhood & other);

    NodePtr p_centre;       // The centre point.
    Neighbors p_neighbors;  // The neighbors.
    Statuses p_statuses;    // The neighbors' statuses.

};

} // namespace rbs::configuration

#endif // NEIGHBORHOOD_H

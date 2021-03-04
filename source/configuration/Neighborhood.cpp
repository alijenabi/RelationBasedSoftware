//
//  Neighborhood.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Neighborhood.h"

namespace rbs::configuration {

Neighborhood::Neighborhood(const Node &centre)
    : p_centre{ std::make_shared<Node>(centre) }
    , p_neighbors{}
{
}

Neighborhood::Neighborhood(const Neighborhood::NodePtr &centre)
    : p_centre{ centre }
    , p_neighbors{}
{
}

Neighborhood::Neighborhood(const Neighborhood &&other) {
    p_centre = std::move(other.p_centre);
    p_neighbors = std::move(other.p_neighbors);
}

Neighborhood &Neighborhood::operator =(Neighborhood other) {
    swap(other);
    return *this;
}

const Neighborhood::NodePtr &Neighborhood::centre() const {
    return p_centre;
}

bool Neighborhood::hasNeighbors() const {
    return !p_neighbors.empty();
}

bool Neighborhood::hasStatus(const Neighborhood::NeighborPtr &neighbor, const Neighborhood::StatusIndex index) const {
    if (p_statuses.size() != 0) {
        const auto search = p_statuses.find(neighbor.get());
        if (search != p_statuses.end()) {
            const auto statusPair = search->second.find(index);
            if (statusPair != search->second.end()) {
                return true;
            }
        }
        return false;
    }

    const auto search = p_statuses.find(neighbor.get());
    if (search != p_statuses.end()) {
        const auto statusPair = search->second.find(index);
        if (statusPair != search->second.end()) {
            return true;
        }
    }
    return false;
}

const Neighborhood::Neighbors &Neighborhood::neighbors() const {
    return p_neighbors;
}

size_t Neighborhood::size() const {
    return p_neighbors.size();
}

const Neighborhood::Status &Neighborhood::status(const Neighborhood::NeighborPtr &neighbor, const Neighborhood::StatusIndex index) const {
    const auto search = p_statuses.find(neighbor.get());
    if (search != p_statuses.end()) {
        const auto statusPair = search->second.find(index);
        if (statusPair != search->second.end()) {
            return statusPair->second;
        } else {
            throw std::out_of_range("The neighbor has no status at " + std::to_string(index) + ".");
        }
    }
    throw std::out_of_range("Neighbor's status not found!");
}

Neighborhood::NodePtr &Neighborhood::centre() {
    return p_centre;
}

Neighborhood::Neighbors &Neighborhood::neighbors() {
    return p_neighbors;
}

void Neighborhood::setStatus(const Neighborhood::NeighborPtr &neighbor, const Neighborhood::StatusIndex index, const Neighborhood::Status &status) {
    const auto search = p_statuses.find(neighbor.get());
    if (search != p_statuses.end()) {
        const auto statusPair = search->second.find(index);
        if (statusPair != search->second.end()) {
            statusPair->second = status;
        } else {
            search->second[index] = status;
        }
    } else {
        p_statuses[neighbor.get()][index] = status;
    }
}

void Neighborhood::swap(Neighborhood &other) {
    std::swap(p_centre, other.p_centre);
    p_neighbors.swap(other.p_neighbors);
}

} // namespace rbs::configuration

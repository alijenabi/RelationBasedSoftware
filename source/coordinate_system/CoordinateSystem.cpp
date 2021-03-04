//
//  CoordinateSystem.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 01.02.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "CoordinateSystem.h"

namespace rbs::coordinate_system {

CoordinateSystem::CoordinateSystem(CoordinateSystem &parent, const Point &origin,
                                   const Vector &i, const Vector j, const Vector k,
                                   const Mapping &forwardMapping, const Mapping &backwardMapping)
    : p_children{}
    , p_i{i}
    , p_j{j}
    , p_indices{}
    , p_k{k}
    , p_origin{origin}
    , p_parent{std::shared_ptr<CoordinateSystem>(&parent, [](auto){})}
    , p_type{Type::Custom}
    , p_forwardMapping{forwardMapping}
    , p_backwardMapping{backwardMapping}
{
    if(!isIncludedInTree(parent))
        throw std::runtime_error("The parent coordiante system has to be part of the coordinate system tree.");

    if (!p_parent)
        throw std::runtime_error("Cannot have a coordinate system without parent.");
    if (!p_forwardMapping)
        throw std::runtime_error("Cannot have a coordinate system without forward mapping.");
    if (!p_backwardMapping)
        throw std::runtime_error("Cannot have a coordinate system without forward mapping.");

    p_axes[0] = Axis();
    p_axes[1] = Axis();
    p_axes[2] = Axis();
}

CoordinateSystem &CoordinateSystem::Global() {
    static CoordinateSystem global;
    return global;
}

std::string CoordinateSystem::tree() {
    return Global().branchInfo(0);
}

bool CoordinateSystem::isIncludedInTree(CoordinateSystem &cs) {
    return Global().includes(cs) || cs.isGlobal();
}

const CoordinateSystem::Vector &CoordinateSystem::axesDirectionVector(const char index) const {
    switch (index) {
    case 0: return p_i;
    case 1: return p_j;
    case 2: return p_k;
    }
    throw std::out_of_range("Index of the axes direction have to be less than three.");
}

CoordinateSystem::Point CoordinateSystem::convert(const Point &point, const CoordinateSystem &source) const {
    if (this == &source)
        return point;
    if(includes(source)) { // if source is on this branch.
        for (const auto& child : p_children) {
            if (child.get() == &source) {
                return child->p_backwardMapping(point, child->p_origin, child->p_i, child->p_j, child->p_k);
            } else if (child->includes(source)) {
                return child->p_backwardMapping(child->convert(point, source), child->p_origin, child->p_i, child->p_j, child->p_k);
            }
        }
    }
    if(source.includes(*this)) { // if this is on the source branch.
        for (const auto& child : source.p_children) {
            if (child.get() == this) {
                return p_forwardMapping(point, p_origin, p_i, p_j, p_k);
            } else if (child->includes(*this)) {
                return child->p_forwardMapping(child->convert(point, source), child->p_origin, child->p_i, child->p_j, child->p_k);
            }
        }
    }
    // Every coordinate system in under global coordinate system branch.
    const auto pointInGCS = Global().convert(point, source);
    return convert(pointInGCS, Global());
}

size_t CoordinateSystem::gridSize(const bool includeChildrenGrids) const {
    if (includeChildrenGrids) {
        return gridSize(false) + std::accumulate(p_children.begin(), p_children.end(), 0, [](const size_t sum, const CoordinateSystemPtr& cs) {
            return sum + cs->gridSize(true);
        });
    }
    return p_axes[0].size() * p_axes[1].size() * p_axes[2].size();
}

size_t CoordinateSystem::gridSize(const unsigned char index) const {
    if (index > 3)
        throw std::range_error("The grid index cannot be begger than 3.");
    return p_axes[index].size();
}

CoordinateSystem::Points CoordinateSystem::getAllGridPoints(const bool includeChildrenGrids) const {
    Points gridPoints;
    Vector localPoint;

    // Extracting the local grid points.
    for(const auto& x0: p_axes[0].values()) {
        localPoint[0] = x0;
        for(const auto& x1: p_axes[1].values()){
            localPoint[1] = x1;
            for(const auto& x2: p_axes[2].values()){
                localPoint[2] = x2;
                gridPoints.emplace_back(localPoint);
            }
        }
    }

    if (includeChildrenGrids) {
        // Extracting the children grid points.
        const auto extractChildGridPoint = [] (const auto& child) {
            auto points = child->getAllGridPoints(true);
            std::transform(points.begin(), points.end(), points.begin(), [&child](const Point& point) {
                return child->p_backwardMapping(point, child->p_origin, child->p_i, child->p_j, child->p_k);
            });
            return points;
        };

        for(const auto& child: p_children) {
            if (child->gridSize(true)) {
                const auto& points = extractChildGridPoint(child);
                gridPoints.insert(gridPoints.end(), points.begin(), points.end());
            }
        }
    }

    return gridPoints;
}

CoordinateSystem::Points CoordinateSystem::getAllPoints(const bool includeChildrenPoints) const {
    using Points = std::vector<Point>;
    Points gridPoints;

    // Extracting the local points.
    gridPoints.reserve(p_indices.size());
    for(const auto& linearIndex: p_indices) {
        const auto index = convertInverce(linearIndex);
        gridPoints.emplace_back(Point{p_axes[0].at(index[0]), p_axes[1].at(index[1]), p_axes[2].at(index[2]) });
    }

    if (includeChildrenPoints) {
        // Extracting the children points.
        const auto extractChildPoints = [] (const auto& child) {
            auto points = child->getAllPoints(true);
            std::transform(points.begin(), points.end(), points.begin(), [&child](const Point& point) {
                return child->p_backwardMapping(point, child->p_origin, child->p_i, child->p_j, child->p_k);
            });
            return points;
        };

        for(const auto& child: p_children) {
            if (child->pointsSize(true)) {
                const auto& points = extractChildPoints(child);
                gridPoints.insert(gridPoints.end(), points.begin(), points.end());
            }
        }
    }

    return gridPoints;
}

CoordinateSystem::Point CoordinateSystem::gridPointAt(const Index &index) const {
    return gridPointAt(convert(index));
}

CoordinateSystem::Point CoordinateSystem::gridPointAt(const LinearIndex &linearIndex) const {
    const auto positionVector = composePositionVectorAt(convertInverce(linearIndex));
    if (positionVector)
        return *positionVector;
    throw std::out_of_range("The linear index is out of range.");
}

CoordinateSystem::Points CoordinateSystem::getNeighborPoints(const Point &centre, const Vector radiusVector) const {
    constexpr auto include = [](const Point&, const Point&) { return true; };
    return getNeighborPoints(centre, radiusVector, include);
}

CoordinateSystem::Points CoordinateSystem::getNeighborPoints(const Point &centre, const Vector radiusVector, const std::function<bool (Point, Point)> &include) const {
    return getNeighborPoints(centre, radiusVector, include, centre);
}

CoordinateSystem::Points CoordinateSystem::getNeighborPoints(const Point &searchcentre, const Vector radiusVector, const std::function<bool (Point, Point)> &include, const Point &neighborhoodCentre) const {
    using Points = std::vector<Point>;
    Points neighbors;
    if (pointsSize()) {
        const auto& posVector = searchcentre.positionVector();
        const auto range_0 = p_axes[0].neighborRange(posVector[0], radiusVector[0]);
        const auto range_1 = p_axes[1].neighborRange(posVector[1], radiusVector[1]);
        const auto range_2 = p_axes[2].neighborRange(posVector[2], radiusVector[2]);

        if (range_0 && range_1 && range_2) {
            Point neighbor;
            Index gridIndex;
            size_t itr0, itr1, itr2;
            for(itr0 = range_0->first; itr0 <= range_0->second; itr0++ ) {
                gridIndex[0] = itr0;
                for(itr1 = range_1->first; itr1 <= range_1->second; itr1++) {
                    gridIndex[1] = itr1;
                    for(itr2 = range_2->first; itr2 <= range_2->second; itr2++) {
                        gridIndex[2] = itr2;
                        if (p_indices.find(convert(gridIndex)) != p_indices.end()) {
                            neighbor.positionVector()[0] = p_axes[0].at(gridIndex[0]);
                            neighbor.positionVector()[1] = p_axes[1].at(gridIndex[1]);
                            neighbor.positionVector()[2] = p_axes[2].at(gridIndex[2]);
                            if (include(neighborhoodCentre, neighbor))
                                neighbors.emplace_back(neighbor);
                        }
                    }
                }
            }
        }
    }
    return neighbors;
}

CoordinateSystem::LinearIndices CoordinateSystem::getNeighborPointIndices(const Point &searchCentre, const Vector radiusVector, const std::function<bool (const Point&, const Point&)> &include, const Point &neighborhoodCentre) const
{
    LinearIndices indices;
    if (pointsSize()) {
        const auto& posVector = searchCentre.positionVector();
        const auto range_0 = p_axes[0].neighborRange(posVector[0], radiusVector[0]);
        const auto range_1 = p_axes[1].neighborRange(posVector[1], radiusVector[1]);
        const auto range_2 = p_axes[2].neighborRange(posVector[2], radiusVector[2]);

        if (range_0 && range_1 && range_2) {
            Point neighbor;
            Index gridIndex;
            LinearIndex gridLinearIndex;
            size_t itr0, itr1, itr2;
            for(itr0 = range_0->first; itr0 <= range_0->second; itr0++ ) {
                gridIndex[0] = itr0;
                for(itr1 = range_1->first; itr1 <= range_1->second; itr1++) {
                    gridIndex[1] = itr1;
                    for(itr2 = range_2->first; itr2 <= range_2->second; itr2++) {
                        gridIndex[2] = itr2;
                        gridLinearIndex = convert(gridIndex);
                        if (p_indices.find(gridLinearIndex) != p_indices.end()) {
                            neighbor.positionVector()[0] = p_axes[0].at(gridIndex[0]);
                            neighbor.positionVector()[1] = p_axes[1].at(gridIndex[1]);
                            neighbor.positionVector()[2] = p_axes[2].at(gridIndex[2]);
                            if (include(neighborhoodCentre, neighbor))
                                indices.insert(gridLinearIndex);
                        }
                    }
                }
            }
        }
    }
    return indices;
}

CoordinateSystem::OptionalPoint CoordinateSystem::getPoint(const size_t &index) const {
    if (index < p_indices.size()) {
        auto itr = p_indices.begin();
        std::advance(itr, index);
        return getPointAt(*itr);
    }
    throw std::range_error("Index is out of IndexRange.");
}

CoordinateSystem::OptionalPoint CoordinateSystem::getPointAt(const Index &index) const {
    return getPointAt(convert(index));
}

CoordinateSystem::OptionalPoint CoordinateSystem::getPointAt(const LinearIndex &linearIndex) const {
    if (p_indices.find(linearIndex) == p_indices.end())
        return nullptr;

    const auto positionVector = composePositionVectorAt(convertInverce(linearIndex));
    if (positionVector)
        return std::make_shared<Point>(*positionVector);

    return nullptr;
}

bool CoordinateSystem::isGlobal() const {
    if (p_parent == nullptr) {
        if (this == &Global()) {
            return true;
        }
        throw std::logic_error("The coordinate system has no parrent but it is not the Global coordinate system.");
    }
    return false;
}

const CoordinateSystem::Point &CoordinateSystem::origin() const {
    return p_origin;
}

CoordinateSystem &CoordinateSystem::appendCustomLocal(const Point &origin, const Vector &i, const Vector j, const Vector k, const Mapping &forwardMapping, const Mapping &backwardMapping) {
    p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, i, j, k, forwardMapping, backwardMapping));
    p_children.back()->p_type = Custom;
    return *p_children.back();
}

CoordinateSystem &CoordinateSystem::appendLocal(const Type type) {
    return appendLocal(type, Point{space::consts::o3D}, space::consts::i3D, space::consts::j3D, space::consts::k3D);
}

CoordinateSystem &CoordinateSystem::appendLocal(const Type type, const Vector &i, const Vector j, const Vector k) {
    return appendLocal(type, Point{space::consts::o3D}, i, j, k);
}

CoordinateSystem &CoordinateSystem::appendLocal(const Type type, const Point &origin, const Vector &i, const Vector j, const Vector k) {
    using namespace convertors;
    switch (p_type) {
    case Cartesian: {
        switch (type) {
        case Cartesian:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, i.unit(), j.unit(), k.unit(), cartesian::toCartesian(), cartesian::toCartesianInverse()));
            break;
        case Cylindrical:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, i.unit(), j.unit(), k.unit(), cartesian::toCylindrical(), cartesian::toCylindricalInverse()));
            break;
        case Spherical:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, i.unit(), j.unit(), k.unit(), cartesian::toSpherical(), cartesian::toSphericalInverse()));
            break;
        case Custom:
            throw std::logic_error("The custom coordinate system should be created by using constructors.");
        default:
            throw std::logic_error("Unknown coordinate system type.");
        }
        break;
    }
    case Cylindrical: {
        switch (type) {
        case Cartesian:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, cylindrical::unit(i), cylindrical::unit(j), cylindrical::unit(k),
                                                                    cylindrical::toCartesian(), cylindrical::toCartesianInverse()));
            break;
        case Cylindrical:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, cylindrical::unit(i), cylindrical::unit(j), cylindrical::unit(k),
                                                                    cylindrical::toCylindrical(), cylindrical::toCylindricalInverse()));
            break;
        case Spherical:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, cylindrical::unit(i), cylindrical::unit(j), cylindrical::unit(k),
                                                                    cylindrical::toSpherical(), cylindrical::toSphericalInverse()));
            break;
        case Custom:
            throw std::logic_error("The custom coordinate system should be created by using constructors.");
        default:
            throw std::logic_error("Unknown coordinate system type.");
        }
        break;
    }
    case Spherical: {
        switch (type) {
        case Cartesian:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, spherical::unit(i), spherical::unit(j), spherical::unit(k),
                                                                    spherical::toCartesian(), spherical::toCartesianInverse()));
            break;
        case Cylindrical:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, spherical::unit(i), spherical::unit(j), spherical::unit(k),
                                                                    spherical::toCylindrical(), spherical::toCylindricalInverse()));
            break;
        case Spherical:
            p_children.push_back(std::make_shared<CoordinateSystem>(*this, origin, spherical::unit(i), spherical::unit(j), spherical::unit(k),
                                                                    spherical::toSpherical(), spherical::toSphericalInverse()));
            break;
        case Custom:
            throw std::logic_error("The custom coordinate system should be created by using constructors.");
        default:
            throw std::logic_error("Unknown coordinate system type.");
        }
        break;
    }
    case Custom:
        throw std::logic_error("The custom coordinate system should be created by using constructors.");
    default:
        throw std::logic_error("Unknown coordinate system type.");
    }
    p_children.back()->p_type = type;
    return *p_children.back();
}

Axis *CoordinateSystem::axes() {
    return p_axes;
}

CoordinateSystem::Vector &CoordinateSystem::axesDirectionVector(const char index) {
    if (!isGlobal()) {
        switch (index) {
        case 0: return p_i;
        case 1: return p_j;
        case 2: return p_k;
        }
        throw std::out_of_range("Index of the axes direction have to be less than three.");
    }
    throw std::runtime_error("Cannot edit the global coordinate system's axes direction.");
}

void CoordinateSystem::clearPoints() {
    p_indices.clear();
}

void CoordinateSystem::clearGrids() {
    p_indices.clear();
    p_axes[0].clear();
    p_axes[1].clear();
    p_axes[2].clear();
}

void CoordinateSystem::exclude(const Point &point) noexcept {
    exclude(point.positionVector());
}

void CoordinateSystem::exclude(const Vector &vector) noexcept {
    try {
        Index index;
        for (unsigned char i = 0; i < 3; i++) {
            auto value = p_axes[i].closestTo(vector[i]);
            if (value && *value == vector[i]) {
                index[i] = *p_axes[i].closestIndexTo(vector[i]);
            }
        }
        p_indices.erase(convert(index));
    }  catch (...) {
    }
}

void CoordinateSystem::exclude(std::function<bool (const Point &)> remove, const bool excludeFromChildrenGrids) {
    Vector localPoint;
    Index localIndex;

    // Extracting the local grid points.
    const auto size0 = p_axes[0].size(), size1 = p_axes[1].size(), size2 = p_axes[2].size();
    size_t itr0, itr1, itr2;
    for(itr0 = 0; itr0 < size0; itr0++) {
        localIndex[0] = itr0;
        localPoint[0] = p_axes[0].at(itr0);
        for(itr1 = 0; itr1 < size1; itr1++) {
            localIndex[1] = itr1;
            localPoint[1] = p_axes[1].at(itr0);
            for(itr2 = 0; itr2 < size2; itr2++) {
                localIndex[2] = itr2;
                localPoint[2] = p_axes[2].at(itr0);
                if (remove(localPoint))
                    p_indices.insert(convert(localIndex));
            }
        }
    }

    if (excludeFromChildrenGrids) {
        for(const auto& child: p_children) {
            if (child->pointsSize(true))
                child->exclude(remove, true);
        }
    }
}

void CoordinateSystem::include(const Point &point) {
    include(point.positionVector());
}

void CoordinateSystem::include(const Vector &vector) {
    size_t sizes[3];
    bool forwardDirections[3] = {false, false, false};
    Index index;
    for (unsigned char i = 0; i < 3; i++) {
        sizes[i] = p_axes[i].size();
        index[i] = p_axes[i].include(vector[i]);
        const auto newSize = p_axes[i].size();
        if (sizes[i] != newSize && index[i] != newSize - 1)
            forwardDirections[i] = true;
    }

    if (forwardDirections[0] || forwardDirections[1] || forwardDirections[2])
        forwardLinearIndices(index, forwardDirections);
    p_indices.insert(convert(index));
}

void CoordinateSystem::include(std::function<bool (const Point &)> add, const bool includeToChildrenGrids) {
    Vector localPoint;
    Index localIndex;

    // Extracting the local grid points.
    const auto size0 = p_axes[0].size(), size1 = p_axes[1].size(), size2 = p_axes[2].size();
    size_t itr0, itr1, itr2;
    for(itr0 = 0; itr0 < size0; itr0++) {
        localIndex[0] = itr0;
        localPoint[0] = p_axes[0].at(itr0);
        for(itr1 = 0; itr1 < size1; itr1++) {
            localIndex[1] = itr1;
            localPoint[1] = p_axes[1].at(itr1);
            for(itr2 = 0; itr2 < size2; itr2++) {
                localIndex[2] = itr2;
                localPoint[2] = p_axes[2].at(itr2);
                if (add(localPoint))
                    p_indices.insert(convert(localIndex));
            }
        }
    }

    const auto convertor = [&add](const CoordinateSystemPtr& cs) {
        return [&add, &cs](const Point& point) {
            return add(cs->p_backwardMapping(point, cs->p_origin, cs->p_i, cs->p_j, cs->p_k));
        };
    };
    if (includeToChildrenGrids) {
        for(const auto& child: p_children) {
            if (child->gridSize(true))
                child->include(convertor(child), true);
        }
    }
}

CoordinateSystem::Point &CoordinateSystem::origin() {
    if (!isGlobal())
        return p_origin;
    throw std::runtime_error("Cannot edit the global coordinate system's origin.");
}

size_t CoordinateSystem::pointsSize(const bool includeChildrenPoints) const {
    if (includeChildrenPoints) {
        return p_indices.size() + std::accumulate(p_children.begin(), p_children.end(), 0, [](const size_t sum, const CoordinateSystemPtr& cs) {
                   return sum + cs->pointsSize(true);
               });
    }
    return p_indices.size();
}

CoordinateSystem::CoordinateSystem()
    : p_children{}
    , p_i{space::consts::i3D}
    , p_k{space::consts::k3D}
    , p_origin{Point{space::consts::o3D}}
    , p_parent{}
    , p_type{Cartesian}
    , p_forwardMapping{nullptr}
    , p_backwardMapping{nullptr}
{}

CoordinateSystem::CoordinateSystem(const CoordinateSystem &other) = default;

CoordinateSystem &CoordinateSystem::operator =(CoordinateSystem &) = default;

CoordinateSystem::LinearIndex CoordinateSystem::c1() {
    static const LinearIndex ans = cbrt(std::numeric_limits<LinearIndex>::max());
    return ans;
}

CoordinateSystem::LinearIndex CoordinateSystem::c2() {
    static const LinearIndex ans = pow(cbrt(std::numeric_limits<LinearIndex>::max()), 2);
    return ans;
}

CoordinateSystem::LinearIndex CoordinateSystem::convert(const Index &index) {
    return index[0] + c1() * index[1] + c2() * index[2];
}

CoordinateSystem::Index CoordinateSystem::convertInverce(const LinearIndex index) {
    const auto z = index / c2();
    const auto y = (index - c2() * z) / c1();
    return {index - c1() * y - c2() * z, y, z};
}

std::string CoordinateSystem::branchInfo(const size_t tab) const {
    const auto tabs = [](const size_t tab) {
        std::string ans = "";
        for(size_t i = 0; i < tab; i++)
            ans = ans + "  ";
        return ans;
    };

    const auto getPoint = [](const CoordinateSystem& cs, const LinearIndex linearIndex) {
        std::string ans = " { id: " + std::to_string(linearIndex);
        ans = ans + ", index: " + std::string(convertInverce(linearIndex));
        ans = ans + ", location: " + std::string(*cs.getPointAt(linearIndex)) + " }";
        return ans;
    };
    const auto getPoints = [&tabs, &tab, &getPoint](const CoordinateSystem& cs) {
        std::string ans = tabs(tab + 1) + "Points: {";
        const auto begin = cs.p_indices.begin();
        for (auto itr = begin; itr != cs.p_indices.end(); itr++) {
            ans = ans + "\n" + tabs(tab + 2) + std::to_string(std::distance(begin, itr)) + ": " + getPoint(cs, *itr);
        }
        ans = ans + "\n" + tabs(tab + 1) + "},\n";
        return ans;
    };

    const auto getType = [](const CoordinateSystem& cs) {
        switch (cs.p_type) {
        case Custom:        return "Custom";
        case Cartesian:     return "Cartesian";
        case Cylindrical:   return "Cylindrical";
        case Spherical:     return "Spherical";
        default:            break;
        }
        return "Unknown";
    };
    const auto getInfo = [&tabs, &tab, &getType](const CoordinateSystem& cs) {
        std::string ans = tabs(tab + 1) + "Info: {\n";
        ans = ans + tabs(tab + 2) + "id: " + std::to_string(reinterpret_cast<size_t>(&cs)) + ",\n";
        ans = ans + tabs(tab + 2) + "type: " + getType(cs) + ",\n";
        ans = ans + tabs(tab + 2) + "number of grid points: " + std::to_string(cs.gridSize()) + ",\n";
        ans = ans + tabs(tab + 2) + "number of points: " + std::to_string(cs.p_indices.size()) + ",\n";
        ans = ans + tabs(tab + 2) + "number of locals: " + std::to_string(cs.p_children.size()) + ",\n";
        ans = ans + tabs(tab + 1) + "},\n";
        return ans;
    };

    std::string ans = tabs(tab) + ((isGlobal())? "GlobalCoordinateSystem: {\n" : "LocalCoordinateSystem: {\n");
    ans = ans + getInfo(*this);

    if (!p_indices.empty())
        ans = ans + getPoints(*this);

    if (!p_children.empty()) {
        ans = ans + tabs(tab + 1) + "Children: {\n";
        for (const auto& child : p_children) {
            ans = ans + child->branchInfo(tab + 2);
        }
        ans = ans + tabs(tab + 1) + "}\n";
    }

    ans = ans + tabs(tab) + "}\n";
    return ans;
}

CoordinateSystem::OptionalVector CoordinateSystem::composePositionVectorAt(const Index &index) const {
    if (!gridSize())
        return nullptr;

    const auto x0 = p_axes[0].at(index[0]);
    const auto x1 = p_axes[1].at(index[1]);
    const auto x2 = p_axes[2].at(index[2]);
    return std::make_shared<Vector>(Vector{x0, x1, x2});
}

void CoordinateSystem::forwardLinearIndices(const Index &index, bool *directions) {
    LinearIndices removeables;
    unsigned char i;
    for( auto itr = p_indices.rbegin(); itr != p_indices.rend(); itr++ ) {
        const auto oldIndex = convertInverce(*itr);
        LinearIndex newLinearIndex = *itr;
        for (i = 0; i < 3; i++) {
            if (directions[i] && index[i] <= oldIndex[i]) {
                if (i == 0)
                    newLinearIndex += 1;
                else if (i == 1)
                    newLinearIndex += c1();
                else
                    newLinearIndex += c2();
            }
        }
        if (newLinearIndex != *itr) {
            removeables.insert(*itr);
            removeables.erase(newLinearIndex);
            p_indices.insert(newLinearIndex);
        }
    }
    for(auto itr = removeables.begin(); itr != removeables.end(); itr++) {
        p_indices.erase(*itr);
    }
}

bool CoordinateSystem::includes(const CoordinateSystem &cs) const {
    for(const auto& child_ptr : p_children)
        if(&(*child_ptr) == &cs)
            return true;
    for(const auto& child_ptr : p_children)
        if(child_ptr->includes(cs))
            return true;
    return false;
}

} // namespace rbs::coordinate_system

rbs::exporting::vtk::convertors::VTKVertexes rbs::exporting::vtk::convertors::convertToVertexes(const coordinate_system::CoordinateSystem::Points &points) {
    std::vector<std::vector<double> > vertexes;
    vertexes.reserve(points.size());
    std::transform(points.begin(), points.end(), std::back_inserter(vertexes), [](const coordinate_system::CoordinateSystem::Point& point) -> VTKVertex {
        const auto& vector = point.positionVector();
        return { vector[0], vector[1], vector[2] };
    });
    return vertexes;
}

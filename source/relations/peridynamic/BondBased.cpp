//
//  BondBased.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "BondBased.h"

namespace rbs::relations::peridynamic {

BondBased::BondBased(BondBased::BondForceRelationship bondForceRelationship, BondBased::VolumeCorrection volumeCurrection, BondBased::PDPart &part, const bool override)
    : Base(0, part, [](const SimulationTime&, PDPart&) {} )
    , p_bondForceRelationship{ bondForceRelationship }
    , p_dt{ 0 }
    , p_includeMaximumStretch{ false }
    , p_lastTime{ 0 }
    , p_maximumStretchIncluded{ false }
    , p_time{ Analyse::time() }
    , p_volumeCurrention{ volumeCurrection }
{
    setFeeder( p_time );
    setRelationship([&, override](const SimulationTime& time, PDPart& part) {
        using Logger = report::Logger;

        p_dt = time - p_lastTime;
        p_lastTime = time;

        const auto start_t = clock();

        auto& logger = Logger::centre();
        logger.log(Logger::Broadcast::Process, "Applying bond-based peridynamic to \"" + part.name() + "\" Part.");

        updateForces(part.neighborhoods(), override);
        updatePartDamages(part);
        updateMaximumStretch(part);

        const size_t problemSize = part.neighborhoods().size();
        const long dist = clock() - start_t;
        const auto duration = double(dist) / CLOCKS_PER_SEC;
        const auto durationText = report::date_time::duration::formated(duration, 3);
        if (problemSize) {
            logger.log(Logger::Broadcast::ProcessStart, std::to_string(problemSize) + " Node" + ((problemSize == 1)? " ": "s ") + " exported.");
            logger.log(Logger::Broadcast::ProcessStart, "");
            logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText)  + ".");
            logger.log(Logger::Broadcast::ProcessTiming, (dist) ? std::to_string( dist / problemSize ) + " CPU clock per Node." : "Less than a CPU clock per Node.");
            logger.log(Logger::Broadcast::ProcessTiming, report::date_time::duration::formated(duration / problemSize, 3) + " per Node.");
            logger.log(Logger::Broadcast::ProcessEnd, "");
            logger.log(Logger::Broadcast::ProcessEnd, "");
        } else {
            logger.log(Logger::Broadcast::Warning, "No Node found.");
            logger.log(Logger::Broadcast::ProcessStart, "");
            logger.log(Logger::Broadcast::ProcessTiming, "Done in " + durationText + ".");
            logger.log(Logger::Broadcast::ProcessEnd, "");
        }
    });
}

BondBased::BondBased(BondBased::BondForceRelationship bondForceRelationship, BondBased::VolumeCorrection volumeCurrection, BondBased::PDPart &centrePart, const BondBased::PDPart &neighborPart, const bool override)
    : Base(0, centrePart, [](const SimulationTime&, PDPart&) {} )
    , p_bondForceRelationship{ bondForceRelationship }
    , p_dt{ 0 }
    , p_includeMaximumStretch{ false }
    , p_lastTime{ 0 }
    , p_maximumStretchIncluded{ false }
    , p_time{ Analyse::time() }
    , p_volumeCurrention{ volumeCurrection }
{
    setFeeder( p_time );
    setRelationship([&, override](const SimulationTime& time, PDPart& centrePart) {
        using Logger = report::Logger;

        p_dt = time - p_lastTime;
        p_lastTime = time;

        const auto start_t = clock();

        auto& logger = Logger::centre();
        logger.log(Logger::Broadcast::Process, "Applying bond-based peridynamic to bonds having centre on \"" + centrePart.name() + "\" Part and end on \"" + neighborPart.name() + "\" Part.");

        auto& neighborhoods = centrePart.neighborhoods(neighborPart);
        updateForces(neighborhoods, override);
        updatePartDamages(centrePart);
        updateMaximumStretch(centrePart);

        const size_t problemSize = neighborhoods.size();
        const long dist = clock() - start_t;
        const auto duration = double(dist) / CLOCKS_PER_SEC;
        const auto durationText = report::date_time::duration::formated(duration, 3);
        if (problemSize) {
            logger.log(Logger::Broadcast::ProcessStart, std::to_string(problemSize) + " Node" + ((problemSize == 1)? " ": "s ") + " exported.");
            logger.log(Logger::Broadcast::ProcessStart, "");
            logger.log(Logger::Broadcast::ProcessTiming, "Done in " + ((durationText.empty())? " less than a nanosecond" : durationText)  + ".");
            logger.log(Logger::Broadcast::ProcessTiming, (dist) ? std::to_string( dist / problemSize ) + " CPU clock per Node." : "Less than a CPU clock per Node.");
            logger.log(Logger::Broadcast::ProcessTiming, report::date_time::duration::formated(duration / problemSize, 3) + " per Node.");
            logger.log(Logger::Broadcast::ProcessEnd, "");
            logger.log(Logger::Broadcast::ProcessEnd, "");
        } else {
            logger.log(Logger::Broadcast::Warning, "No Node found.");
            logger.log(Logger::Broadcast::ProcessStart, "");
            logger.log(Logger::Broadcast::ProcessTiming, "Done in " + durationText + ".");
            logger.log(Logger::Broadcast::ProcessEnd, "");
        }
    });
}

BondBased BondBased::BrittleFracture(const double maxStretch, const double materialConstant,
                                     const double gridSpacing, const double horizonRadius,
                                     BondBased::PDPart &part, const bool override)
{
    return BrittleFracture(maxStretch, materialConstant,
                           gridSpacing, horizonRadius,
                           part, override,
                           [](const auto&, const auto&) { return true;});
}

BondBased BondBased::BrittleFracture(const double maxPositiveStretch, const double maxNegativeStretch, const double materialConstant,
                                     const double gridSpacing, const double horizonRadius,
                                     BondBased::PDPart &part, const bool override)
{
    return BrittleFracture(maxPositiveStretch, maxNegativeStretch, materialConstant,
                           gridSpacing, horizonRadius,
                           part, override,
                           [](const auto&, const auto&) { return true; });
}

BondBased BondBased::BrittleFracture(const double maxStretch, const double materialConstant,
                                     const double gridSpacing, const double horizonRadius,
                                     BondBased::PDPart &part, const bool override,
                                     const BondBased::IsInFracturableArea &isInFracturableArea)
{
    const auto max = maxStretch > 0 ? maxStretch : -maxStretch;
    return BrittleFracture(max, -max, materialConstant, gridSpacing, horizonRadius, part, override, isInFracturableArea);
}

BondBased BondBased::BrittleFracture(const double maxPositiveStretch, const double maxNegativeStretch, const double materialConstant,
                                     const double gridSpacing, const double horizonRadius,
                                     BondBased::PDPart &part, const bool override,
                                     const BondBased::IsInFracturableArea &isInFracturableArea)
{
    if (materialConstant < 0 || space::isZero(materialConstant))
        throw std::out_of_range("The material constant of bond-based peridynmaic must be bigger than zero.");
    if (gridSpacing < 0 || space::isZero(gridSpacing))
        throw std::out_of_range("The grid spacing of bond-based peridynmaic Part must be bigger than zero.");
    if (horizonRadius < 0 || space::isZero(horizonRadius))
        throw std::out_of_range("The horizon radius of bond-based peridynmaic Part must be bigger than zero.");

    const auto upperBond = horizonRadius + gridSpacing;
    const auto lowerBond = horizonRadius - gridSpacing;
    return BondBased(
        [materialConstant, maxPositiveStretch, maxNegativeStretch, isInFracturableArea](const SimulationTime, const Vector& initial, const Vector& deformation, const HorizonPtr& centerHorizon, const HorizonPtr& neighborHorizon) -> Vector{
            if (deformation.isZero() || initial.isZero()) {
                return space::consts::o3D;
            }

            bool isElastic = !isInFracturableArea(centerHorizon, neighborHorizon);
            if (!isElastic && centerHorizon->hasStatus(neighborHorizon, Property::Damage)) {
                return space::consts::o3D;
            }

            const auto initialLength = initial.length();
            const auto currentBondVec = initial + deformation;
            const auto bondStretch = (currentBondVec.length() - initialLength) / initialLength;

            if (centerHorizon->centre()->has(Property::MaximumStretch)) {
                if (abs(bondStretch) > abs(centerHorizon->centre()->at(Property::MaximumStretch).value<double>())) {
                    centerHorizon->centre()->at(Property::MaximumStretch) = bondStretch;
                }
            }

            if (isElastic || ( maxNegativeStretch <= bondStretch && bondStretch <= maxPositiveStretch ))
                return materialConstant * bondStretch * (initial + deformation).unit();
            centerHorizon->setStatus(neighborHorizon, Property::Damage, int(1));
            return space::consts::o3D;
        },
        [gridSpacing, horizonRadius, upperBond, lowerBond](const SimulationTime, const Vector& initial, const Vector&) -> double {
            const double initialDistance = initial.length();
            if (initialDistance < lowerBond) {
                return 1.;
            } else if (initialDistance < upperBond) {
                return 0.5 + (horizonRadius - initialDistance) / gridSpacing;
            }
            return 0;
        },
        part,
        override);
}

BondBased BondBased::Elastic(const double materialConstant,
                             const double gridSpacing, const double horizonRadius,
                             BondBased::PDPart &part, const bool override) {
    if (materialConstant < 0 || space::isZero(materialConstant))
        throw std::out_of_range("The material constant of bond-based peridynmaic must be bigger than zero.");
    if (gridSpacing < 0 || space::isZero(gridSpacing))
        throw std::out_of_range("The grid spacing of bond-based peridynmaic Part must be bigger than zero.");
    if (horizonRadius < 0 || space::isZero(horizonRadius))
        throw std::out_of_range("The horizon radius of bond-based peridynmaic Part must be bigger than zero.");

    const auto upperBond = horizonRadius + gridSpacing / 2;
    const auto lowerBond = horizonRadius - gridSpacing / 2;
    return BondBased(
        [materialConstant](const SimulationTime, const Vector& initial, const Vector& deformation, const HorizonPtr&centerHorizon, const HorizonPtr&) -> Vector{
            if (deformation.isZero() || initial.isZero()) {
                return space::consts::o3D;
            }

            const auto initialLength = initial.length();
            const auto currentBondVec = initial + deformation;
            const auto stretch = (currentBondVec.length() - initialLength) / initialLength;

            if (centerHorizon->centre()->has(Property::MaximumStretch)) {
                if (abs(stretch) > abs(centerHorizon->centre()->at(Property::MaximumStretch).value<double>())) {
                    centerHorizon->centre()->at(Property::MaximumStretch) = stretch;
                }
            }

            return materialConstant * stretch * currentBondVec.unit();
        },
        [gridSpacing, horizonRadius, upperBond, lowerBond](const SimulationTime, const Vector& initial, const Vector&) -> double {
            const double initialDistance = initial.length();
            if (initialDistance < lowerBond) {
                return 1.;
            } else if (initialDistance < upperBond) {
                return 0.5 + (horizonRadius - initialDistance) / gridSpacing;
            }
            return 0;
        },
        part,
        override);
}

BondBased BondBased::Elastic(const double materialConstant,
                             const double neighborPartGridSpacing, const double partHorizonRadius,
                             BondBased::PDPart &part, const BondBased::PDPart &neighborPart, const bool override) {
    static std::map<double, double> computedVolumeCurrentions;

    if (materialConstant < 0 || space::isZero(materialConstant))
        throw std::out_of_range("The material constant of bond-based peridynmaic must be bigger than zero.");
    if (neighborPartGridSpacing < 0 || space::isZero(neighborPartGridSpacing))
        throw std::out_of_range("The grid spacing of bond-based peridynmaic Part must be bigger than zero.");
    if (partHorizonRadius < 0 || space::isZero(partHorizonRadius))
        throw std::out_of_range("The horizon radius of bond-based peridynmaic Part must be bigger than zero.");

    const auto upperBond = partHorizonRadius + neighborPartGridSpacing / 2;
    const auto lowerBond = partHorizonRadius - neighborPartGridSpacing / 2;
    return BondBased(
        [materialConstant](const SimulationTime, const Vector& initial, const Vector& deformation, const HorizonPtr& centerHorizon, const HorizonPtr&) -> Vector{
            if (deformation.isZero() || initial.isZero()) {
                return space::consts::o3D;
            }

            const auto initialLength = initial.length();
            const auto currentBondVec = initial + deformation;
            const auto stretch = (currentBondVec.length() - initialLength) / initialLength;

            if (centerHorizon->centre()->has(Property::MaximumStretch)) {
                if (abs(stretch) > abs(centerHorizon->centre()->at(Property::MaximumStretch).value<double>())) {
                    centerHorizon->centre()->at(Property::MaximumStretch) = stretch;
                }
            }

            return materialConstant * stretch * currentBondVec.unit();
        },
        [neighborPartGridSpacing, partHorizonRadius, upperBond, lowerBond](const SimulationTime, const Vector& initial, const Vector&) -> double {
            const double initialDistance = initial.length();
            if (initialDistance < lowerBond) {
                return 1.;
            } else if (initialDistance < upperBond) {
                return 0.5 + (partHorizonRadius - initialDistance) / neighborPartGridSpacing;
            }
            return 0;
        },
        part,
        neighborPart,
        override);
}

void BondBased::includeMaximumStretch(const bool include) {
    p_includeMaximumStretch = include;
}

void BondBased::setBondForceRelationship(const BondBased::BondForceRelationship &relationship) {
    p_bondForceRelationship = relationship;
}

void BondBased::updateCentreForce(const BondBased::HorizonPtr &horizon, const bool override) const {
    const auto& centre = horizon->centre();
    const auto centreInitialPosition = centre->initialPosition().value<space::Point<3> >();
    const auto centreDisp = (centre->has(Property::Displacement)) ? centre->at(Property::Displacement).value<space::vec3>() : space::consts::o3D;
    auto force = space::consts::o3D;
    const auto& neighbors = horizon->neighbors();
    std::for_each(neighbors.begin(), neighbors.end(), [&](const PDPart::NeighborhoodPtr& neighbor) {
        const auto& neighborCentre = neighbor->centre();
        const auto neighborDisp = (neighborCentre->has(Property::Displacement)) ? neighborCentre->at(Property::Displacement).value<space::vec3>() : space::consts::o3D;
        const auto initialBondVector = neighborCentre->initialPosition().value<space::Point<3> >().positionVector() - centreInitialPosition.positionVector();
        const auto deformation = neighborDisp - centreDisp;

        if (!neighborCentre->has(Property::Volume))
            throw std::runtime_error("The bond-based peridynmaic cannot proceed without volume at all of the Nodes.");

        force += 0.5 * neighborCentre->at(Property::Volume).value<double>()
                * p_volumeCurrention(p_time, initialBondVector, deformation)
                * p_bondForceRelationship(p_time, initialBondVector, deformation, horizon, neighbor);
    });

    if (override) {
        if (centre->has(Property::Force)) {
            centre->at(Property::Force) = force;
        } else {
            // has no force means the force is zero, thus only override it if the computed force is not zero.
            if (!force.isZero()) {
                centre->at(Property::Force) = force;
            }
        }
    } else { // appending the force
        if (!force.isZero()) { // if the computed force is not zero
            if (centre->has(Property::Force)) {
                centre->at(Property::Force) = centre->at(Property::Force).value<Vector>() + force;
            } else {
                centre->at(Property::Force) = force;
            }
        } // else the state of node force does not need to be changed.
    }
}

void BondBased::updateForces(const configuration::Part::NeighborhoodPtrs &horizons, const bool override) const {
    std::for_each(horizons.begin(), horizons.end(), [this, override](const PDPart::NeighborhoodPtr& neighborhood) {
        updateCentreForce(neighborhood, override);
    });
}

void BondBased::updatePartDamages(BondBased::PDPart &part) const {
    const auto& horizons = part.neighborhoods();
    std::for_each(horizons.begin(), horizons.end(), [](const PDPart::NeighborhoodPtr& neighborhood) {
        const auto& center = neighborhood->centre();
        const auto& neighbors = neighborhood->neighbors();
        int damage = 0;
        std::for_each(neighbors.begin(), neighbors.end(), [&damage,  &neighborhood](const PDPart::NeighborhoodPtr& neighbor) {
            damage += (neighborhood->hasStatus(neighbor, Property::Damage))? 1 : 0;
        });

        if (center->has(Property::Damage) || damage != 0) {
            center->at(Property::Damage) = double(damage) / double(neighbors.size());
        }
    });
}

void BondBased::updateMaximumStretch(BondBased::PDPart &part) {
    if (p_includeMaximumStretch != p_maximumStretchIncluded) {
        const auto& neighborhoods = part.neighborhoods();
        const auto include = p_includeMaximumStretch;
        std::for_each(neighborhoods.begin(), neighborhoods.end(), [include](const PDPart::NeighborhoodPtr& neighborhood) {
            if (include) {
                if (!neighborhood->centre()->has(Property::MaximumStretch)) {
                    neighborhood->centre()->at(Property::MaximumStretch) = 0.;
                }
            } else {
                neighborhood->centre()->erase(Property::MaximumStretch);
            }
        });
        p_maximumStretchIncluded = p_includeMaximumStretch;
    }
}

} // namespace rbs::relations::peridynamic

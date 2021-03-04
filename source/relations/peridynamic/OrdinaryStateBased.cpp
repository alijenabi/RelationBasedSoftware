//
//  OrdinaryStateBased.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "OrdinaryStateBased.h"

namespace rbs::relations::peridynamic {

OrdinaryStateBased::OrdinaryStateBased(OrdinaryStateBased::BondForceRelationship bondForceRelationship,
                                       OrdinaryStateBased::VolumeCorrection volumeCurrection,
                                       OrdinaryStateBased::PDPart &part, const bool override)
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
        logger.log(Logger::Broadcast::Process, "Applying ordinary state-based peridynamic to \"" + part.name() + "\" Part.");

        if (space::isZero(p_dt)) {
            const auto start_t = clock();

            logger.log(Logger::Broadcast::ProcessStart, "Computing and updating nodal weighted volumes.");

            updateWeightedVolumes(part);

            logger.log(Logger::Broadcast::ProcessEnd, "");

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
        }

        logger.log(Logger::Broadcast::ProcessStart,  "Computing and updating nodal dilatations.");
        updateDilatation(part);

        logger.log(Logger::Broadcast::Process,  "Computing and updating nodal forces.");
        updateForces(part.neighborhoods(), override);
        updatePartDamages(part);
        updateMaximumStretch(part);

        logger.log(Logger::Broadcast::ProcessEnd, "");

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

OrdinaryStateBased OrdinaryStateBased::BrittleFracture(const double maxStretch,
                                                       const double bulkModulus, const double shearModulus,
                                                       const double gridSpacing, const double horizonRadius,
                                                       OrdinaryStateBased::PDPart &part, const bool override) {
    return BrittleFracture(maxStretch,
                           bulkModulus, shearModulus,
                           gridSpacing, horizonRadius,
                           part, override,
                           [](const auto&, const auto&) { return true;});
}

OrdinaryStateBased OrdinaryStateBased::BrittleFracture(const double maxPositiveStretch, const double maxNegativeStretch,
                                                       const double bulkModulus, const double shearModulus,
                                                       const double gridSpacing, const double horizonRadius,
                                                       OrdinaryStateBased::PDPart &part, const bool override) {
    return BrittleFracture(maxPositiveStretch, maxNegativeStretch,
                           bulkModulus, shearModulus,
                           gridSpacing, horizonRadius,
                           part, override,
                           [](const auto&, const auto&) { return true; });
}

OrdinaryStateBased OrdinaryStateBased::BrittleFracture(const double maxStretch,
                                                       const double bulkModulus, const double shearModulus,
                                                       const double gridSpacing, const double horizonRadius,
                                                       OrdinaryStateBased::PDPart &part, const bool override,
                                                       const OrdinaryStateBased::IsInFracturableArea &isInFracturableArea) {
    const auto max = maxStretch > 0 ? maxStretch : -maxStretch;
    return BrittleFracture(max, -max, bulkModulus, shearModulus, gridSpacing, horizonRadius, part, override, isInFracturableArea);
}

OrdinaryStateBased OrdinaryStateBased::BrittleFracture(const double maxPositiveStretch, const double maxNegativeStretch,
                                                       const double bulkModulus, const double shearModulus,
                                                       const double gridSpacing, const double horizonRadius,
                                                       OrdinaryStateBased::PDPart &part, const bool override,
                                                       const OrdinaryStateBased::IsInFracturableArea &isInFracturableArea) {
    if (bulkModulus < 0 || space::isZero(bulkModulus))
        throw std::out_of_range("The bulk modulus of ordinary state-based peridynmaic must be bigger than zero.");
    if (shearModulus < 0 || space::isZero(shearModulus))
        throw std::out_of_range("The shear modulus of ordinary state-based peridynmaic must be bigger than zero.");
    if (gridSpacing < 0 || space::isZero(gridSpacing))
        throw std::out_of_range("The grid spacing of ordinary state-based peridynmaic Part must be bigger than zero.");
    if (horizonRadius < 0 || space::isZero(horizonRadius))
        throw std::out_of_range("The horizon radius of ordinary state-based peridynmaic Part must be bigger than zero.");

    const auto upperBond = horizonRadius + gridSpacing / 2;
    const auto lowerBond = horizonRadius - gridSpacing / 2;
    return OrdinaryStateBased(
        [bulkModulus, shearModulus, maxPositiveStretch, maxNegativeStretch, isInFracturableArea](const SimulationTime, const Vector& initial, const Vector& deformation, const HorizonPtr& centerHorizon, const HorizonPtr& neighborHorizon) -> Vector{
            if (initial.isZero()) {
                return space::consts::o3D;
            }

            bool isElastic = !isInFracturableArea(centerHorizon, neighborHorizon);
            if (!isElastic && centerHorizon->hasStatus(neighborHorizon, Property::Damage)) {
                return space::consts::o3D;
            }

            const auto initialLength = initial.length();
            const auto currentBondVec = initial + deformation;
            const auto e = currentBondVec.length() - initialLength;
            const auto stretch = e / initialLength;

            if (centerHorizon->centre()->has(Property::MaximumStretch)) {
                if (abs(stretch) > abs(centerHorizon->centre()->at(Property::MaximumStretch).value<double>())) {
                    centerHorizon->centre()->at(Property::MaximumStretch) = stretch;
                }
            }

            if (isElastic || ( maxNegativeStretch <= stretch && stretch <= maxPositiveStretch )) {
                const auto th_c = centerHorizon->centre()->at(Property::Dilatation).value<double>();
                const auto th_n = neighborHorizon->centre()->at(Property::Dilatation).value<double>();
                const auto m_c = centerHorizon->centre()->at(Property::WeightedVolume).value<double>();
                const auto m_n = neighborHorizon->centre()->at(Property::WeightedVolume).value<double>();

                return ((3 * bulkModulus - 5 * shearModulus) * (th_c / m_c + th_n / m_n) * initialLength
                        + 15 * shearModulus * (1 / m_c + 1 / m_n) * e)
                       * currentBondVec.unit();
            }
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

OrdinaryStateBased OrdinaryStateBased::Elastic(const double bulkModulus, const double shearModulus,
                                               const double gridSpacing, const double horizonRadius,
                                               OrdinaryStateBased::PDPart &part, const bool override) {
    if (bulkModulus < 0 || space::isZero(bulkModulus))
        throw std::out_of_range("The bulk modulus of ordinary state-based peridynmaic must be bigger than zero.");
    if (shearModulus < 0 || space::isZero(shearModulus))
        throw std::out_of_range("The shear modulus of ordinary state-based peridynmaic must be bigger than zero.");
    if (gridSpacing < 0 || space::isZero(gridSpacing))
        throw std::out_of_range("The grid spacing of ordinary state-based peridynmaic Part must be bigger than zero.");
    if (horizonRadius < 0 || space::isZero(horizonRadius))
        throw std::out_of_range("The horizon radius of ordinary state-based peridynmaic Part must be bigger than zero.");

    const auto upperBond = horizonRadius + gridSpacing / 2;
    const auto lowerBond = horizonRadius - gridSpacing / 2;
    return OrdinaryStateBased(
        [bulkModulus, shearModulus](const SimulationTime, const Vector& initial, const Vector& deformation, const HorizonPtr& centerHorizon, const HorizonPtr& neighborHorizon) -> Vector{
            if (initial.isZero()) {
                return space::consts::o3D;
            }

            const auto initialLength = initial.length();
            const auto currentBondVec = initial + deformation;
            const auto e = currentBondVec.length() - initialLength;
            const auto stretch = e / initialLength;

            if (centerHorizon->centre()->has(Property::MaximumStretch)) {
                if (abs(stretch) > abs(centerHorizon->centre()->at(Property::MaximumStretch).value<double>())) {
                    centerHorizon->centre()->at(Property::MaximumStretch) = stretch;
                }
            }

            const auto th_c = centerHorizon->centre()->at(Property::Dilatation).value<double>();
            const auto th_n = neighborHorizon->centre()->at(Property::Dilatation).value<double>();
            const auto m_c = centerHorizon->centre()->at(Property::WeightedVolume).value<double>();
            const auto m_n = neighborHorizon->centre()->at(Property::WeightedVolume).value<double>();

            return ((3 * bulkModulus - 5 * shearModulus) * (th_c / m_c + th_n / m_n) * initialLength
                       + 15 * shearModulus * (1 / m_c + 1 / m_n) * e)
                   * currentBondVec.unit();
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

void OrdinaryStateBased::includeMaximumStretch(const bool include) {
    p_includeMaximumStretch = include;
}

void OrdinaryStateBased::setBondForceRelationship(const OrdinaryStateBased::BondForceRelationship &relationship) {
    p_bondForceRelationship = relationship;
}

void OrdinaryStateBased::updateCentreForce(const OrdinaryStateBased::HorizonPtr &horizon, const bool override) const {
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
            throw std::runtime_error("The ordinary state-based peridynmaic cannot proceed without volume at all of the Nodes.");

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

void OrdinaryStateBased::updateDilatation(OrdinaryStateBased::PDPart &part) const {
    const auto& horizons = part.neighborhoods();
    std::for_each(horizons.begin(), horizons.end(), [&](const PDPart::NeighborhoodPtr& horizon) {
        const auto& centre = horizon->centre();
        const auto centreInitialPosition = centre->initialPosition().value<space::Point<3> >();
        const auto centreDisp = (centre->has(Property::Displacement)) ? centre->at(Property::Displacement).value<space::vec3>() : space::consts::o3D;

        auto dilatation = 0.;
        const auto& neighborHorizons = horizon->neighbors();
        std::for_each(neighborHorizons.begin(), neighborHorizons.end(), [&](const PDPart::NeighborhoodPtr& neighborHorizon) {
            const auto& neighborCentre = neighborHorizon->centre();
            const auto neighborDisp = (neighborCentre->has(Property::Displacement)) ? neighborCentre->at(Property::Displacement).value<space::vec3>() : space::consts::o3D;
            const auto initialBondVector = neighborCentre->initialPosition().value<space::Point<3> >().positionVector() - centreInitialPosition.positionVector();
            const auto deformation = neighborDisp - centreDisp;

            const auto bondLength = initialBondVector.length();
            dilatation += bondLength
                    * (deformation + initialBondVector).length()
                    * neighborCentre->at(Property::Volume).value<double>()
                    * p_volumeCurrention(p_time, initialBondVector, deformation);
        });

        centre->at(Property::Dilatation) = 3 * dilatation / centre->at(Property::WeightedVolume).value<double>();
    });
}

void OrdinaryStateBased::updateForces(const configuration::Part::NeighborhoodPtrs &horizons, const bool override) const {
    std::for_each(horizons.begin(), horizons.end(), [this, override](const PDPart::NeighborhoodPtr& neighborhood) {
        updateCentreForce(neighborhood, override);
    });
}

void OrdinaryStateBased::updatePartDamages(OrdinaryStateBased::PDPart &part) const {
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

void OrdinaryStateBased::updateWeightedVolumes(OrdinaryStateBased::PDPart &part) const {
    const auto& horizons = part.neighborhoods();
    std::for_each(horizons.begin(), horizons.end(), [&](const PDPart::NeighborhoodPtr& horizon) {
        const auto& centre = horizon->centre();
        const auto centreInitialPosition = centre->initialPosition().value<space::Point<3> >();
        const auto centreDisp = (centre->has(Property::Displacement)) ? centre->at(Property::Displacement).value<space::vec3>() : space::consts::o3D;

        auto weightedVolume = 0.;
        const auto& neighborHorizons = horizon->neighbors();
        std::for_each(neighborHorizons.begin(), neighborHorizons.end(), [&](const PDPart::NeighborhoodPtr& neighborHorizon) {
            const auto& neighborCentre = neighborHorizon->centre();
            const auto neighborDisp = (neighborCentre->has(Property::Displacement)) ? neighborCentre->at(Property::Displacement).value<space::vec3>() : space::consts::o3D;
            const auto initialBondVector = neighborCentre->initialPosition().value<space::Point<3> >().positionVector() - centreInitialPosition.positionVector();
            const auto deformation = neighborDisp - centreDisp;

            const auto bondLengthSquared = initialBondVector.lengthSquared();
            weightedVolume += bondLengthSquared
                    * neighborCentre->at(Property::Volume).value<double>()
                    * p_volumeCurrention(p_time, initialBondVector, deformation);
        });

        centre->at(Property::WeightedVolume) = weightedVolume;
    });
}

void OrdinaryStateBased::updateMaximumStretch(OrdinaryStateBased::PDPart &part) {
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

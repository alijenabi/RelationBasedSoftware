//
//  BondBased.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef BONDBASED_H
#define BONDBASED_H

#include "Property.h"
#include "../Relation.h"
#include "../../Analyse.h"
#include "../../configuration/Part.h"
#include "../../report/Logger.h"
#include <algorithm>

/**
 * Provides the specified version of the RBS objects for the Peridynamic simulation.
 */
namespace rbs::relations::peridynamic {

/**
 * @brief Provides easy to use relation for implementing bond-based peridynamic simulations.
 */
class BondBased : public Relation<double, configuration::Part>
{
public:
    using SimulationTime = double;
    using Horizon = configuration::Neighborhood;
    using PDPart = configuration::Part;
    using HorizonPtr = PDPart::NeighborhoodPtr;
    using IsInFracturableArea = std::function<bool(const HorizonPtr&, const HorizonPtr&)>;
    using Base = Relation<double, PDPart>;

    using Vector = space::vec3;

    /**
     * @brief Defines the relation between the bond ant its force vector.
     * @note The vectors are in local coordinate system.
     * @note The result of this function will be multiply by the VolumeCurrention of the bond end.
     * @param 1st The time of the simulation.
     * @param 2nd The bond initial vector.
     * @param 3rd The bond current vector.
     * @param 4rd The center horizon.
     * @param 5rd The neighbor horizon.
     * @return The new bond force.
     */
    using BondForceRelationship = std::function<Vector(const SimulationTime, const Vector&, const Vector&, const HorizonPtr&, const HorizonPtr&)>;

    /**
     * @brief Defines the volume currention for each bond.
     * @param 1st The time of the simulation.
     * @param 2nd The bond initial vector.
     * @param 3rd The bond current vector.
     * @return The volume currention for the given bond.
     */
    using VolumeCorrection = std::function<double(const SimulationTime, const Vector&, const Vector&)>;

    /**
     * @brief Constructs a relation that can apply the bond-based peridynamic to any PDPart.
     * @note The PDPart denotes a Part that its nodes' property indexes (e.g., Velocity) are compatible with the defined index on rbs::relations::peridynamic::Property.
     * @param bondForceRelationship A fucntion that defines the relation between the bond and its force vector.
     *                              @param 1st The time of the simulation.
     *                              @param 2nd The bond initial vector.
     *                              @param 3rd The bond current vector.
     *                              @param 4th The last bond force.
     *                              @return The new bond force.
     * @param volumeCurrection A function that defines the volume currention for each bond.
     *                         @param 1st The time of the simulation.
     *                         @param 2nd The bond initial vector.
     *                         @param 3rd The bond current vector.
     *                         @return The volume currention for the given bond.
     * @param part The PD part.
     * @param override Whether to override the forces (true), or append to the existing forces (false).
     */
    explicit BondBased(BondForceRelationship bondForceRelationship, VolumeCorrection volumeCurrection, PDPart& part, const bool override);

    /**
     * @brief Constructs a relation that can apply the bond-based peridynamic to connection between two PDPart.
     * @note The PDPart denotes a Part that its nodes' property indexes (e.g., Velocity) are compatible with the defined index on rbs::relations::peridynamic::Property.
     * @param bondForceRelationship A fucntion that defines the relation between the connection bond ant its force vector.
     *                              @param 1st The time of the simulation.
     *                              @param 2nd The bond initial vector.
     *                              @param 3rd The bond current vector.
     *                              @param 4th The last bond force.
     *                              @return The new bond force.
     * @param volumeCurrection A function that defines the volume currention for each connection bond.
     *                         @param 1st The time of the simulation.
     *                         @param 2nd The bond initial vector.
     *                         @param 3rd The bond current vector.
     *                         @return The volume currention for the given bond.
     * @param centrePart The PDPart that owns the centre of the connection's bonds.
     * @param neighborPart The PDPart that owns the end Nodes of the connection's bonds.
     * @param override Whether to override the forces (true), or append to the existing forces (false).
     */
    explicit BondBased(BondForceRelationship bondForceRelationship, VolumeCorrection volumeCurrection, PDPart& centrePart, const PDPart& neighborPart, const bool override);

    /**
     * @brief The defualt copy constructor.
     */
    BondBased(const BondBased&) = default;

    /**
     * @brief Generates a relation that can apply a partially brittle fracturable bond-based peridynamic applicable to any PDPart.
     * @warning This can only be applied to the PDParts with uniform cubic subdomains.
     * @note The behaviour is unknown if the maxStretch is zero or negative.
     * @param maxStretch The maximum stretch that the bond will be able to carry in both the tensile and the pressure directions.
     * @param materialConstant The bond-based peridynamic mateial constant.
     * @param gridSpacing The distance between Nodes.
     * @param horizonRadius The horizon radius.
     * @param part The PD part.
     * @param override Whether to override the forces (true), or append to the existing forces (false).
     * @throws If one of the materialConstant, gridSpacing, or horizonRadius is given zero or negative value.
     */
    static BondBased BrittleFracture(const double maxStretch, const double materialConstant,
                                     const double gridSpacing, const double horizonRadius,
                                     PDPart& part, const bool override);

    /**
     * @brief Generates a relation that can apply a partially brittle fracturable bond-based peridynamic applicable to any PDPart.
     * @warning This can only be applied to the PDParts with uniform cubic subdomains.
     * @note The behaviour is unknown if the maxPositiveStretch is negative, or maxNegativeStretch is positive.
     * @param maxPositiveStretch The maximum positive (tension) stretch that the bond will be able to carry.
     * @param maxNegativeStretch The maximum negative (pressor) stretch that the bond will be able to carry.
     * @param materialConstant The bond-based peridynamic mateial constant.
     * @param gridSpacing The distance between Nodes.
     * @param horizonRadius The horizon radius.
     * @param part The PD part.
     * @param override Whether to override the forces (true), or append to the existing forces (false).
     * @throws If one of the materialConstant, gridSpacing, or horizonRadius is given zero or negative value.
     */
    static BondBased BrittleFracture(const double maxPositiveStretch, const double maxNegativeStretch, const double materialConstant,
                                     const double gridSpacing, const double horizonRadius,
                                     PDPart& part, const bool override);

    /**
     * @brief Generates a relation that can apply a partially brittle fracturable bond-based peridynamic applicable to any PDPart.
     * @warning This can only be applied to the PDParts with uniform cubic subdomains.
     * @note The behaviour is unknown if the maxStretch is zero or negative.
     * @param maxStretch The maximum stretch that the bond will be able to carry in both the tensile and the pressure directions.
     * @param materialConstant The bond-based peridynamic mateial constant.
     * @param gridSpacing The distance between Nodes.
     * @param horizonRadius The horizon radius.
     * @param part The PD part.
     * @param override Whether to override the forces (true), or append to the existing forces (false).
     * @param isInFracturableArea A fuction the recives the center neighorbood and end neighbordood and returns
     *                               true if the bond is inside the fracturable area and false if it is not.
     * @throws If one of the materialConstant, gridSpacing, or horizonRadius is given zero or negative value.
     */
    static BondBased BrittleFracture(const double maxStretch, const double materialConstant,
                                     const double gridSpacing, const double horizonRadius,
                                     PDPart& part, const bool override,
                                     const IsInFracturableArea& isInFracturableArea);

    /**
     * @brief Generates a relation that can apply a partially brittle fracturable bond-based peridynamic applicable to any PDPart.
     * @warning This can only be applied to the PDParts with uniform cubic subdomains.
     * @note The behaviour is unknown if the maxPositiveStretch is negative, or maxNegativeStretch is positive.
     * @param maxPositiveStretch The maximum positive (tension) stretch that the bond will be able to carry.
     * @param maxNegativeStretch The maximum negative (pressor) stretch that the bond will be able to carry.
     * @param materialConstant The bond-based peridynamic mateial constant.
     * @param gridSpacing The distance between Nodes.
     * @param horizonRadius The horizon radius.
     * @param part The PD part.
     * @param override Whether to override the forces (true), or append to the existing forces (false).
     * @param isInFracturableArea A fuction the recives the center neighorbood and end neighbordood and returns
     *                               true if the bond is inside the fracturable area and false if it is not.
     * @throws If one of the materialConstant, gridSpacing, or horizonRadius is given zero or negative value.
     */
    static BondBased BrittleFracture(const double maxPositiveStretch, const double maxNegativeStretch, const double materialConstant,
                                     const double gridSpacing, const double horizonRadius,
                                     PDPart& part, const bool override,
                                     const IsInFracturableArea& isInFracturableArea);

    /**
     * @brief Generates a relation that can apply the elastic bond-based peridynamic to any PDPart.
     * @warning This only aplicable on the PDParts with uniform cubic subdomains.
     * @note For 3D problems materialConstant     = 3 * youngsModulus / (M_PI * pow(horizonRadius, 4) * (1 - 2 * poissonRasio));
     *                                            = 18 * bulkModulus  / (M_PI * pow(horizonRadius, 4));
     * @note For 2D plane stress materialConstant = 6 * youngsModulus / (M_PI * pow(horizonRadius, 3) * (1 - poissonRasio) * plateThickness);
     *                                            = 12 * bulkModulus  / (M_PI * pow(horizonRadius, 3) * plateThickness);
     * @note For 2D plane strain materialConstant = 6 * youngsModulus / (M_PI * pow(horizonRadius, 3) * (1 + poissonRasio) * (1 - 2 * poissonRasio) * plateThickness);
     * @note For 1D problems materialConstant     = 2 * youngsModulus / (pow(horizonRadius, 2) * crossSectionArea);
     * @param materialConstant The bond-based peridynamic mateial constant.
     * @param gridSpacing The distance between Nodes.
     * @param horizonRadius The horizon radius.
     * @param part The PD part.
     * @param override Whether to override the forces (true), or append to the existing forces (false).
     */
    static BondBased Elastic(const double materialConstant,
                             const double gridSpacing, const double horizonRadius,
                             PDPart& part, const bool override);

    /**
     * @brief Generates a relation that can apply the elastic bond-based peridynamic to connection between two PDPart.
     * @warning This relation will not overridee the forces of the PDPart but instead adds the new bond forces to the existed Node force.
     * @warning This only aplicable on the PDParts with uniform cubic subdomains.
     * @param materialConstant The bond-based peridynamic mateial constant.
     * @param neighborPartGridSpacing The distance between Nodes in the neighborhood Part.
     * @param partHorizonRadius The horizon radius of the centre Part.
     * @param centrePart The PDPart that owns the centre of the connection's bonds.
     * @param neighborPart The PDPart that owns the end Nodes of the connection's bonds.
     * @param override Whether to override the forces (true), or append to the existing forces (false).
     */
    static BondBased Elastic(const double materialConstant,
                             const double neighborPartGridSpacing, const double partHorizonRadius,
                             PDPart& part, const PDPart& neighborPart, const bool override);

    /**
     * @brief Defines whether to include the maximum stretch to the nodes or not.
     */
    void includeMaximumStretch(const bool include = true);

    /**
     * @brief Sets the bonnd force relationship.
     */
    void setBondForceRelationship(const BondForceRelationship& relationship);

private:

    /**
     * @brief Computes bond-based peridynamic force at the centre of the horizon (i.e., neighborhood) and updates it.
     * @warning If the computed bond-based peridynamic force is zero and the centre of the horizon has no force, nothing will happen.
     * @param horizon The neighborhood inside the PDPart.
     * @param override Whether to override the force or add to it.
     */
    void updateCentreForce(const HorizonPtr& horizon, const bool override) const;

    /**
     * @brief Computes bond-based peridynamic force at the given horizon centers (i.e., neighborhood) and updates it.
     * @warning If the computed bond-based peridynamic force is zero and the centre of the horizon has no force, nothing will happen.
     * @param horizons The neighborhoods inside the PDPart.
     * @param override Whether to override the force or add to it.
     */
    void updateForces(const PDPart::NeighborhoodPtrs& horizons, const bool override) const;

    /**
     * @brief Updates the damage of the Part Nodes by avaraging the horizon broken bonds.
     */
    void updatePartDamages(PDPart& part) const;

    /**
     * @brief Updates the maximum stretch existance on the nodes.
     */
    void updateMaximumStretch(PDPart& part);

    BondForceRelationship p_bondForceRelationship;  // Denotes how to compute the force for each bond.
    SimulationTime p_dt;                            // The time step.
    bool p_includeMaximumStretch;                   // Whether to incude the maximum stretch to the nodes or not.
    SimulationTime p_lastTime;                      // The last time.
    bool p_maximumStretchIncluded;                  // Whether the maximum stretch is already included or not.
    const SimulationTime& p_time;                   // The reference to global Analyses::Time.
    VolumeCorrection p_volumeCurrention;            // Denotes how to compute the volume currention for each bond.
};

} // namespace rbs::relations::peridynamic

#endif // BONDBASED_H

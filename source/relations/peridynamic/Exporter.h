//
//  Exporter.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef EXPORTER_H
#define EXPORTER_H

#include "Property.h"
#include "../Relation.h"
#include "../../Analyse.h"
#include "../../configuration/Part.h"
#include "../../exporting/VTK.h"

/**
 * Provides the specified version of the RBS objects for the Peridynamic simulation.
 */
namespace rbs::relations::peridynamic {

class Exporter : public Relation<double, configuration::Part >
{
public:

    using SimulationTime = double;
    using BoundaryPart = configuration::Part;
    using Base = Relation<double, configuration::Part>;

    /**
     * @brief A function defining when should the exportation take place.
     * @param 1st The current symulation time.
     * @param 2nd The current time step.
     * @return True if the exportation should take place, false otherwise.
     */
    using ExportingCondition = std::function<bool(const SimulationTime, const size_t)>;

    using Properties = std::set<Property>;
    using PDPart = configuration::Part;

    /**
     * @brief Denotes the configuration target.
     */
    enum Target {
        InitialConfiguration,
        CurrentConfiguration
    };

    /**
     * @brief The constructor.
     * @param properties The properties that should be exported to the exported file.
     * @param target The exportation target, IOW, the points positing.
     * @param part The part to be exported.
     * @param path The path for the file to be exported to it.
     * @param name The name of the exported file.
     */
    explicit Exporter(const Properties& properties,
                      const Target target,
                      PDPart& part,
                      const std::string& path,
                      const std::string& name);

    /**
     * @brief Sets the exportation condition.
     * @param condition The exportation condition explains when the exportation should take place.
     */
    void setCondition(const ExportingCondition& condition);

private:

    /**
     * @brief Converts the property to std string.
     */
    std::string to_string(const Property property) const;

    /**
     * @brief Converts the property to Attribute type.
     */
    exporting::vtk::AttType to_attType(const Property property) const;

    /**
     * @brief Appends attributes of the part to the file.
     */
    void appendAttributes(PDPart& part);

    /**
     * @brief Appends points of the part to the file.
     */
    void appendPoints(PDPart& part);

    /**
     * @brief Appends scalar attributes of the part to the file.
     * @throws If the property is not an scalar attribute.
     */
    void appendScalarAttributes(PDPart& part, const Property property);

    /**
     * @brief Appends vector attributes of the part to the file.
     * @throws If the property is not an vector attribute.
     */
    void appendVectorAttributes(PDPart& part, const Property property);

    /**
     * @brief Sets up attributes.
     */
    void setupAttributes();

    SimulationTime p_dt;                // The time step.
    ExportingCondition p_exporting;     // A function defineing the time of exportation.
    SimulationTime p_lastTimeStep;      // The last time step time.
    exporting::VTKFileSeries p_files;   // The VTK file list for exporting into it.
    size_t p_fileIndex;
    const Properties p_properties;      // The list of the properties to export.
    const Target p_target;              // The list of the targets.
    const SimulationTime& p_time;       // The reference to global Analyses::Time.
    size_t p_timeStep;                  // The current number of timestep.

};

} // namespace rbs::relations::peridynamic

#endif // EXPORTER_H

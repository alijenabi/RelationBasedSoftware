//
//  Exporter.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 23.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Exporter.h"

namespace rbs::relations::peridynamic {

Exporter::Exporter(const Exporter::Properties &properties, const Exporter::Target target, Exporter::PDPart &part, const std::string &path, const std::string &name)
    : Base(0, part, [](const SimulationTime&, PDPart&) {} )
    , p_dt{ 0 }
    , p_exporting{ [](const auto, const auto) { return true; } }
    , p_lastTimeStep{ 0 }
    , p_files{ exporting::VTKFileSeries(path, name, "vtk", 5000) }
    , p_fileIndex{ 0 }
    , p_properties{ properties }
    , p_target{ target }
    , p_time{ Analyse::time() }
    , p_timeStep{ 0 }
{
    setFeeder( p_time );
    setRelationship([&](const SimulationTime& time, PDPart& part) {
        p_dt = time - p_lastTimeStep;
        p_lastTimeStep = time;
        if (p_dt > 0 || p_timeStep == 0) {
            if (p_exporting(p_time, p_timeStep)) {
                using Logger = report::Logger;
                const auto start_t = clock();

                auto& logger = Logger::centre();
                logger.log(Logger::Broadcast::Process, "Exporting \"" + part.name() + "\" Part neighbors.");
                logger.log(Logger::Broadcast::ProcessStart, "to: " + p_files.fullName(p_fileIndex,true));
                logger.log(Logger::Broadcast::ProcessEnd, "");

                setupAttributes();
                appendPoints(part);
                appendAttributes(part);

                p_files.at(p_fileIndex).assemble(true);
                p_fileIndex++;

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
            p_timeStep++;
        }
    });
}

void Exporter::setCondition(const Exporter::ExportingCondition &condition) {
    p_exporting = condition;
}

std::string Exporter::to_string(const Property property) const {
    switch (property) {
    case Property::InitialPosition:  return "InitialPosition";
    case Property::CurrentPosition:  return "CurrentPosition";
    case Property::Displacement:     return "Displacement";
    case Property::Velocity:         return "Velocity";
    case Property::Acceleration:     return "Acceleration";
    case Property::Force:            return "Force";
    case Property::BodyForce:        return "Body-Force";
    case Property::Density:          return "Density";
    case Property::Volume:           return "Volume";
    case Property::Damage:           return "Damage";
    case Property::MaximumStretch:   return "Maximum-Stretch";
    case Property::WeightedVolume:   return "Weighted-Volume";
    case Property::Dilatation:       return "Dilatation";
    }
    throw std::runtime_error("Unknown property.");
}

exporting::vtk::AttType Exporter::to_attType(const Property property) const {
    using T = exporting::vtk::AttType;
    switch (property) {
    case Property::InitialPosition:  return T::Vector;
    case Property::CurrentPosition:  return T::Vector;
    case Property::Displacement:     return T::Vector;
    case Property::Velocity:         return T::Vector;
    case Property::Acceleration:     return T::Vector;
    case Property::Force:            return T::Vector;
    case Property::BodyForce:        return T::Vector;
    case Property::Density:          return T::Scalar;
    case Property::Volume:           return T::Scalar;
    case Property::Damage:           return T::Scalar;
    case Property::MaximumStretch:   return T::Scalar;
    case Property::WeightedVolume:   return T::Scalar;
    case Property::Dilatation:       return T::Scalar;
    }
    throw std::runtime_error("Unknown property.");
}

void Exporter::appendAttributes(Exporter::PDPart &part) {
    using T = exporting::vtk::AttType;
    for(const auto property : p_properties) {
        if (to_attType(property) == T::Scalar) {
            appendScalarAttributes(part, property);
        } else if (to_attType(property) == T::Vector) {
            appendVectorAttributes(part, property);
        }
    }
}

void Exporter::appendPoints(Exporter::PDPart &part) {
    const auto& neighborhoods = part.neighborhoods();
    std::vector<space::Point<3> > points;
    points.reserve(neighborhoods.size());
    if (part.local().isGlobal()) {
        const auto& GCS = coordinate_system::CoordinateSystem::Global();
        if (p_target == CurrentConfiguration) {
            std::transform(neighborhoods.begin(), neighborhoods.end(), std::back_inserter(points), [&GCS, &part](const PDPart::NeighborhoodPtr& neighborhood) {
                const auto& centre = *neighborhood->centre();
                const auto disp = (centre.has(Property::Displacement))? centre.at(Property::Displacement).value<space::vec3>() : space::consts::o3D;
                return GCS.convert(centre.initialPosition().value< space::Point<3> >() + disp, part.local());
            });
        } else {
            std::transform(neighborhoods.begin(), neighborhoods.end(), std::back_inserter(points), [&GCS, &part](const PDPart::NeighborhoodPtr& neighborhood) {
                const auto& centre = *neighborhood->centre();
                return GCS.convert(centre.initialPosition().value< space::Point<3> >(), part.local());
            });
        }
    } else {
        if (p_target == CurrentConfiguration) {
            std::transform(neighborhoods.begin(), neighborhoods.end(), std::back_inserter(points), [](const PDPart::NeighborhoodPtr& neighborhood) {
                const auto& centre = *neighborhood->centre();
                const auto disp = (centre.has(Property::Displacement))? centre.at(Property::Displacement).value<space::vec3>() : space::consts::o3D;
                return centre.initialPosition().value< space::Point<3> >() + disp;
            });
        } else {
            std::transform(neighborhoods.begin(), neighborhoods.end(), std::back_inserter(points), [](const PDPart::NeighborhoodPtr& neighborhood) {
                const auto& centre = *neighborhood->centre();
                return centre.initialPosition().value< space::Point<3> >();
            });
        }
    }
    p_files.at(p_fileIndex).appendCell(exporting::VTKCell::PolyVertex, exporting::vtk::convertors::convertToVertexes(points));
}

void Exporter::appendScalarAttributes(Exporter::PDPart &part, const Property property) {
    if (to_attType(property) != exporting::vtk::AttType::Scalar)
        throw std::runtime_error("The " + to_string(property) + "is not a scalar proprty.");
    const auto& neighborhoods = part.neighborhoods();
    std::vector<double> atts;
    atts.reserve(neighborhoods.size());
    std::transform(neighborhoods.begin(), neighborhoods.end(), std::back_inserter(atts), [&property](const PDPart::NeighborhoodPtr& neighborhood) {
        const auto& centre = *neighborhood->centre();
        return (centre.has(property))? centre.at(property).value<double>() : 0;
    });
    p_files.at(p_fileIndex)[to_string(property)].appendAttribute(atts);
}

void Exporter::appendVectorAttributes(Exporter::PDPart &part, const Property property) {
    if (to_attType(property) != exporting::vtk::AttType::Vector)
        throw std::runtime_error("The " + to_string(property) + "is not a vector proprty.");
    const auto& neighborhoods = part.neighborhoods();
    std::vector<double> atts;
    atts.reserve(neighborhoods.size() * 3);
    for(const auto& neighborhood : neighborhoods) {
        const auto& centre = *neighborhood->centre();
        const auto& pos = (centre.has(property)) ? centre.at(property).value<space::vec3>() : space::consts::o3D;
        atts.emplace_back(pos[0]);
        atts.emplace_back(pos[1]);
        atts.emplace_back(pos[2]);
    }
    p_files.at(p_fileIndex)[to_string(property)].appendAttribute(atts);
}

void Exporter::setupAttributes() {
    for(const auto property : p_properties) {
        const auto name = to_string(property);
        p_files.at(p_fileIndex)[name].setSource(exporting::vtk::AttSource::Point);
        p_files.at(p_fileIndex)[name].setAttributeType(to_attType(property));
    }
}



} // namespace rbs::relations::peridynamic

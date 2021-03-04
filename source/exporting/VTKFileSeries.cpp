//
//  VTKFileSeries.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "VTKFileSeries.h"

namespace rbs::exporting {

VTKFileSeries::VTKFileSeries(const std::string &basename,
                             const std::string &extension,
                             const Index allowedSize,
                             const bool override,
                             const bool fillVocations)
    : file_system::FileSeries<VTKFile>(basename, extension, allowedSize, override, fillVocations)
    , p_isSharedConfigIsInteranl{false}
    , p_sharedConfiguration{}
{
}

VTKFileSeries::VTKFileSeries(const std::string &path,
                             const std::string &base_name,
                             const std::string &extension,
                             const Index allowedSize,
                             const bool override,
                             const bool fillVocations)
    : VTKFileSeries(path + base_name, extension, allowedSize, override, fillVocations)
{
}

VTKFileSeries::~VTKFileSeries()
{
    if (p_isSharedConfigIsInteranl && p_isSharedConfigIsInteranl)
        p_sharedConfiguration->remove();
}

bool VTKFileSeries::isShareingConfiguration() const
{
    return static_cast<bool>(p_sharedConfiguration);
}

void VTKFileSeries::lockSharedConfiguration() const
{
    if (p_sharedConfiguration) {
        p_sharedConfiguration->lock();
    } else {
        throw std::runtime_error("The configuration is not shared, you can not block it.");
    }
}

VTKFile &VTKFileSeries::at(const Index index)
{
    if(p_sharedConfiguration) {
        const auto file = std::make_shared<VTKFile>(FileSeries::at(index));
        file->shareConfiguration(p_sharedConfiguration);
        return *file;
    }
    return FileSeries::at(index);
}

VTKFile &VTKFileSeries::next()
{
    if(p_sharedConfiguration) {
        const auto file = std::make_shared<VTKFile>(FileSeries::next());
        file->shareConfiguration(p_sharedConfiguration);
        return *file;
    }
    return FileSeries::next();
}

void VTKFileSeries::shareConfiguration()
{
    p_isSharedConfigIsInteranl = true;
    p_sharedConfiguration = std::make_shared<VTKFile>(name(0,true) + "_shared_configuration", extension());
}

void VTKFileSeries::shareConfiguration(const std::shared_ptr<VTKFile> file)
{
    p_isSharedConfigIsInteranl = false;
    p_sharedConfiguration = file;
}

} // namespace rbs::exporting

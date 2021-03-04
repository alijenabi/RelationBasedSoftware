TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(release, debug|release) {
    message(Release)
}

CONFIG(debug, debug|release) {
    message(Debug)
}

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.16

SOURCES += \
    Analyse.cpp \
    WaveDispersionAndPropagation.cpp \
    configuration/Neighborhood.cpp \
    configuration/Node.cpp \
    configuration/Part.cpp \
    coordinate_system/Axis.cpp \
    coordinate_system/CoordinateSystem.cpp \
    coordinate_system/convertors.cpp \
    coordinate_system/grid.cpp \
    exporting/CSVFile.cpp \
    exporting/TextFile.cpp \
    exporting/VTKFile.cpp \
    exporting/VTKFileSeries.cpp \
    exporting/file_system/BaseFile.cpp \
    exporting/file_system/FileRegistar.cpp \
    exporting/file_system/SingleFile.cpp \
    exporting/vtk/VTKAttributeSection.cpp \
    exporting/vtk/VTKCell.cpp \
    exporting/vtk/VTKCellSection.cpp \
    exporting/vtk/VTKCellTypeSection.cpp \
    exporting/vtk/VTKSection.cpp \
    exporting/vtk/VTKPointSection.cpp \
    geometry/Combined.cpp \
    geometry/PointStatus.cpp \
    geometry/Primary.cpp \
    geometry/SetOperation.cpp \
    relations/peridynamic/BondBased.cpp \
    relations/peridynamic/BoundaryDomain.cpp \
    relations/peridynamic/Exporter.cpp \
    relations/peridynamic/OrdinaryStateBased.cpp \
    relations/peridynamic/time_integrations/PDEuler.cpp \
    relations/peridynamic/time_integrations/PDVelocityVerletAlgorithm.cpp \
    relations/solid/Rotation.cpp \
    relations/solid/Translation.cpp \
    report/Logger.cpp \
    report/date_time.cpp \
    report/logging/templates/Standard.cpp \
    report/text_editing.cpp \
    variables/Variant.cpp

HEADERS += \
    Analyse.h \
    configuration/Neighborhood.h \
    configuration/Node.h \
    configuration/Part.h \
    coordinate_system/Axis.h \
    coordinate_system/CoordinateSystem.h \
    coordinate_system/convertors.h \
    coordinate_system/grid.h \
    exporting/CSV.h \
    exporting/CSVFile.h \
    exporting/CSVFileSeries.h \
    exporting/TextFile.h \
    exporting/TextFileSeries.h \
    exporting/VTK.h \
    exporting/VTKFile.h \
    exporting/VTKFileSeries.h \
    exporting/file_system/BaseFile.h \
    exporting/file_system/FileRegistar.h \
    exporting/file_system/FileSeries.h \
    exporting/file_system/FileSeriesImp.h \
    exporting/file_system/SingleFile.h \
    exporting/vtk/VTKAttributeSection.h \
    exporting/vtk/VTKCell.h \
    exporting/vtk/VTKCellSection.h \
    exporting/vtk/VTKCellTypeSection.h \
    exporting/vtk/VTKSection.h \
    exporting/vtk/VTKPointSection.h \
    geometry/Combined.h \
    geometry/Geometry.h \
    geometry/PointStatus.h \
    geometry/Primary.h \
    geometry/SetOperation.h \
    relations/Applicable.h \
    relations/peridynamic.h \
    relations/peridynamic/BondBased.h \
    relations/peridynamic/BoundaryDomain.h \
    relations/peridynamic/Exporter.h \
    relations/peridynamic/OrdinaryStateBased.h \
    relations/peridynamic/Property.h \
    relations/Relation.h \
    relations/RelationImp.h \
    relations/peridynamic/time_integrations/PDEuler.h \
    relations/peridynamic/time_integrations/PDVelocityVerletAlgorithm.h \
    relations/solid.h \
    relations/solid/Rotation.h \
    relations/solid/Translation.h \
    report/Logger.h \
    report/date_time.h \
    report/logging/templates/Interface.h \
    report/logging/templates/Standard.h \
    report/text_editing.h \
    space/Index.h \
    space/IndexImp.h \
    space/Point.h \
    space/PointImp.h \
    space/Space.h \
    space/SpaceImp.h \
    space/Vector.h \
    space/VectorImp.h \
    variables/Variant.h

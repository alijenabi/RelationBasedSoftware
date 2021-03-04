//
//  CSVCell.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "VTKCell.h"
#include <numeric>

namespace rbs::exporting {

VTKCell::Code VTKCell::code(VTKCell::Type type)
{
    using CT = VTKCell::Type;
    switch (type) {
    case CT::Vertex:                return 1;
    case CT::PolyVertex:            return 2;
    case CT::Line:                  return 3;
    case CT::PolyLine:              return 4;
    case CT::Triangle:              return 5;
    case CT::TriangleStrip:         return 6;
    case CT::Polygon:               return 7;
    case CT::Pixel:                 return 8;
    case CT::Quad:                  return 9;
    case CT::Tetra:                 return 10;
    case CT::Voxel:                 return 11;
    case CT::Hexahedron:            return 12;
    case CT::Wedge:                 return 13;
    case CT::Pyramid:               return 14;
    case CT::QuadraticEdge:         return 21;
    case CT::QuadraticTriangle:     return 22;
    case CT::QuadraticQuad:         return 23;
    case CT::QuadraticTetra:        return 24;
    case CT::QuadraticHexahedron:   return 25;
    case CT::QuadraticPyramid:      return 27;
    case CT::QuadraticWedge:        return 31;
    }
    throw std::range_error("The cell type is out of known range.");
}

bool VTKCell::hasFixedNumberOfPoints(VTKCell::Type type)
{
    return minPoints(type) == maxPoints(type);
}

VTKCell::PointsPerCell VTKCell::maxPoints(VTKCell::Type type)
{
    using CT = VTKCell::Type;
    switch (type) {
    case CT::Vertex:                return 1;
    case CT::Line:                  return 2;
    case CT::Triangle:
    case CT::QuadraticEdge:         return 3;
    case CT::Pixel:
    case CT::Quad:
    case CT::Tetra:                 return 4;
    case CT::Pyramid:               return 5;
    case CT::Wedge:
    case CT::QuadraticTriangle:     return 6;
    case CT::Voxel:
    case CT::Hexahedron:
    case CT::QuadraticQuad:         return 8;
    case CT::QuadraticTetra:        return 10;
    case CT::QuadraticWedge:        return 12;
    case CT::QuadraticPyramid:      return 13;
    case CT::QuadraticHexahedron:   return 20;
    case CT::PolyVertex:
    case CT::PolyLine:
    case CT::TriangleStrip:
    case CT::Polygon:
        return std::numeric_limits<PointsPerCell>::max();
    }
    throw std::range_error("The cell type is out of known range.");
}

VTKCell::PointsPerCell VTKCell::minPoints(VTKCell::Type type)
{
    using CT = VTKCell::Type;
    switch (type) {
    case CT::Vertex:
    case CT::PolyVertex:            return 1;
    case CT::Line:
    case CT::PolyLine:              return 2;
    case CT::Triangle:
    case CT::TriangleStrip:
    case CT::Polygon:
    case CT::QuadraticEdge:         return 3;
    case CT::Pixel:
    case CT::Quad:
    case CT::Tetra:                 return 4;
    case CT::Pyramid:               return 5;
    case CT::Wedge:
    case CT::QuadraticTriangle:     return 6;
    case CT::Voxel:
    case CT::Hexahedron:
    case CT::QuadraticQuad:         return 8;
    case CT::QuadraticTetra:        return 10;
    case CT::QuadraticWedge:        return 12;
    case CT::QuadraticPyramid:      return 13;
    case CT::QuadraticHexahedron:   return 20;
    }
    throw std::range_error("The cell type is out of known range.");
}

} // namespace rbs::exporting

//
//  CSVCell.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 16.04.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VTKCELL_H
#define VTKCELL_H

/**
 * The VTKPointSection class provides the means to handle exporting Points to the vtk file.
 */
namespace rbs::exporting {

/**
 * @brief The VTKCell class provides the means to handle diferent type of the vtk cells and their limitations.
 * @note The class is not default constructable.
 * @note The class is not copiable or moveable.
 */
class VTKCell
{
public:

    using Code = unsigned long;             // Each vtk cell a special code define by VTK legacy format.
    using PointsPerCell = unsigned long;    // Each vtk cell has an acceptable number of points.

    /**
     * @brief The Type of the cells defined by the VTK legacy format.
     */
    enum Type {
        Vertex,
        PolyVertex,
        Line,
        PolyLine,
        Triangle,
        TriangleStrip,
        Polygon,
        Pixel,
        Quad,
        Tetra,
        Voxel,
        Hexahedron,
        Wedge,
        Pyramid,
        QuadraticEdge,
        QuadraticTriangle,
        QuadraticQuad,
        QuadraticTetra,
        QuadraticHexahedron,
        QuadraticWedge,
        QuadraticPyramid
    };

    /**
     * @note The class is not default constructable.
     * @note The class is not copiable or moveable.
     */
    explicit VTKCell() = delete;

    /**
     * @brief Gives the code of a given cell type.
     * @note Defined by VTK format.
     * @param type The cell type.
     * @return The relative cell type code.
     * @throws runtime error if the type is unknown.
     */
    static Code code(Type type);

    /**
     * @brief Clears whether the number of points per cell are fixed or can be varied.
     * @param type The cell type.
     * @return True if the number of points per cell is fixed, false otherwise.
     */
    static bool hasFixedNumberOfPoints(Type type);

    /**
     * @brief The maximum number of points that the cell type can have.
     * @note Defined by VTK format.
     * @param type A cell type.
     * @return The maximum points per cell, PointNumber.max() if there is no limit.
     * @throws runtime error if the type is unknown.
     */
    static PointsPerCell maxPoints(Type type);

    /**
     * @brief The minimum number of points that the cell type can have.
     * @note Defined by VTK format.
     * @param type A cell type.
     * @return The minimum points per cell.
     * @throws runtime error if the type is unknown.
     */
    static PointsPerCell minPoints(Type type);

};

} // namespace rbs::exporting

#endif // VTKCELL_H

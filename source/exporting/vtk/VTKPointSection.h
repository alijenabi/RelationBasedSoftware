//
//  VTKPointSection.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 11.07.2020.
//  Copyright © 2020 Ali Jenabidehkordi. All rights reserved.
//

#ifndef VTKPOINTSECTION_H
#define VTKPOINTSECTION_H

#include "VTKSection.h"
#include <vector>

/**
 * The exporting namespace provides the means to handle exporting simulation data to files.
 */
namespace rbs::exporting {

/**
 * @brief The VTKPointSection class provides the means to handle exporting Points to the tempvtk file.
 */
class VTKPointSection: public VTKSection
{
public:

    using Dimension = double;
    using Point = std::vector<Dimension>;
    using PointIndex = unsigned long;
    using Size = unsigned long;

    /**
     * @brief The constructor.
     * @param name The name of the parent VTK file (the path to the file included).
     */
    explicit VTKPointSection(const std::string &name);

    /**
     * @brief The constructor.
     * @param path The directory path of the file.
     * @param name The name of the parent VTK file.
     */
    explicit VTKPointSection(const std::string &path, const std::string &name);

    /**
     * @brief Gives the number of points inside the body.
     */
    Size pointCount() const;

    /**
     * @brief Gives the header of the points section.
     */
    virtual std::string header() const override;

    /**
     * @brief Appends a point to the body of the section.
     * @note VTK format only accepts 3D points.
     * @param x The x of the point.
     * @param y The y of the point.
     * @param z The z of the point.
     * @return The index of the added point to the body.
     */
    PointIndex appendPoint(Dimension x, Dimension y, Dimension z);

    /**
     * @brief Appends a point to the body of the section.
     * @note VTK format only accepts 3D points.
     * @param dimensions The dimensions of the point.
     * @return The index of the added point to the body.
     */
    PointIndex appendPoint(const Point & dimensions);

    /**
     * @brief Appends points to the body of the section.
     * @param dimensions The dimension of the point in sereis (i.g., x_0, y_0, z_0, x_1, y_1, ...).
     * @return The indices of the points.
     * @throws If the dimensions size is not a factor of 3.
     */
    std::vector<PointIndex> appendPoints(std::vector<Dimension> dimensions);

    /**
     * @brief Appends points to the body of the section.
     * @note VTK format only accepts 3D points.
     * @tparam T A type with a member signicure as: const std::vector<Dimension>& dimensions() const;
     * @param points The points to append to the body.
     * @return The indices of the points.
     * @throws If the member return std::vector of other lenght than 3.
     */
    template<typename T>
    std::vector<PointIndex> appendPoints(std::vector<T> points) {
        std::vector<PointIndex> ans;
        ans.reserve(points.size());
        for(const auto point: points)
            ans.emplace_back(appendPoint(point));
        return ans;
    }

protected:

    /**
     * @brief Gives the last index of the point inside body.
     */
    PointIndex currentPointIndex() const;

};

} // namespace rbs::exporting

#endif // VTKPOINTSECTION_H

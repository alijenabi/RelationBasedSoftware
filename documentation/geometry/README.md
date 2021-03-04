<a name="Top"/><br>
##### Table of Contents  
* [Geometry](#Geometry)                                                                       <br>
  + [Primary Geometry](#PrimaryGeometry)                                                      <br>
    - [Point](#PrimaryGeometryStaticConstructorsPoint)                                        <br>
    - [Line](#PrimaryGeometryStaticConstructorsLine)                                          <br>
    - [Plane](#PrimaryGeometryStaticConstructorsPlane)                                        <br>
    - [Sphere, Cylinder, and Plate](#PrimaryGeometryStaticConstructorsSphereCylinderPlate)    <br>
    - [Half space and full space](#PrimaryGeometryStaticConstructorsHalfspaceAndSpace)        <br>
  + [Combined Geometry](#CombinedGeometry)                                                    <br>
    - [Static Constructors](#CombinedGeometryStaticConstructors)                              <br>
  + [Complex Geometries](#ComplexGeometries)                                                  <br>
    - [Ellipsoid](#Ellipsoid)                                                                 <br>
    - [Elliptical Paraboloid Plate](#EllipticalParaboloidPlate)                               <br>

<a name="Geometry"/><br>
# Geometry

Two distinct geometries, namely, primary- and combined-geometries, are available. The `geometry::Primary` contains a compulsory position vector, an optional unit vector, and a thickness, which allows us to define points, lines, planes, spheres, infinite-cylindrical-bars, and infinite-plates in 3D space. Note that the geometries do not have any coordinate system, but they are developed on the cartesian coordinate system assumption. For instance, a Line depending on its unit vector may present a circle, an ellipse, or a coil in a cylindrical coordinate system. The following UML diagram illustrates the architecture for `geometry::Primary`, where the distance, point status, intersection, and projection are the key features of this class. Note that the intersection and projection of two `geometry::Primary` may not be presentable by another Primary- or Combined- Geometry, for example, the intersection of two spheres or projection of a sphere on a plane; in such a situation, the class member returns an `geometry::Primary::Empty()`.

<p align="center">
  <img src="images/Primary/UML.png" width="40%"> 
  <br>Figure 1: The geometry::Primary UML diagram.<br>
</p>

The `geometry::Combined` is a combination of two other geometries with a unique set operation (i.e., `+` operation or union; `∗` operation or intersection; `−` operation or difference; and `ˆ` operation or symmetric-difference) between them. As illustrated in Figure 2, a `Geometry` interface is required to make this possible. Thus the combined- and primary-geometry should implement the `Geometry` interface; then, the `geometry::Combined` will be able to store two or more Geometries along with the set operation. Thus a combined-geometry will be able to combine other combined- or primary-geometries together to present a new shape in 3D.

<p align="center">
  <img src="images/Combined/UML.png" width="60%"> 
  <br>Figure 2: The geometry::Combined UML diagram.<br>
</p>

[back to top of the page.](#Top)<br>
<a name="PrimaryGeometry"/><br>
## Primary Geometry

Easy to use static constructors provided for all of the Primary geometries. The following table illustrates the possible Primary geometries and the parameters that are required to construct them. 

| Point                                              | Line                                                 | Plane                                             | 
|:--------------------------------------------------:|:----------------------------------------------------:|:-------------------------------------------------:|
| <img src="images/Primary/Point.png" width="100%">  | <img src="images/Primary/Line.png" width="100%">     | <img src="images/Primary/Plane.png" width="100%"> |
| Sphere                                             | Cylinder                                             | Plate                                             | 
| <img src="images/Primary/Sphere.png" width="100%"> | <img src="images/Primary/Bar.png" width="100%">      | <img src="images/Primary/Plate.png" width="100%"> |

[back to top of the page.](#Top)
<a name="PrimaryGeometryStaticConstructors"/><br>
### Static Constructors
<a name="PrimaryGeometryStaticConstructorsPoint"/><br>
#### Point
`Primary::Point` can be constructed from  its explicit coordinates, its position vector, or its position (point in 3D space).

```C++
using namespace rbs;

auto Point_1 = geometry::Primary::Point({2, 1.3, 1});
auto Point_2 = geometry::Primary::Point(space::Vector<3>{1.2, 3, 5});
auto Point_3 = geometry::Primary::Point(space::Point<3>{0, 2.3, 5});
```

The `Origin()` will create a `geometry::Primary::Point` at `{0, 0, 0}`.
```C++
auto origin = geometry::Primary::Origin();
```

[back to top of the page.](#Top)<br>
<a name="PrimaryGeometryStaticConstructorsLine"/><br>
#### Line
`Primary::Line` can be constructed with a point in 3D space on the line and a vector specifying the direction of the line or two positions (points in 3D space) that the line passes through or two `Primary::Point`s. Note that all the `Primary::Line`s below will evaluate to equal.

```C++
auto Line_1 = geometry::Primary::Line(space::Point<3>{2, 1.3, 1}, space::Vector<3>{-0.8, 1.7, 4});
auto Line_2 = geometry::Primary::Line(space::Point<3>{2, 1.3, 1}, space::Point<3>{1.2, 3, 5});
auto Line_3 = geometry::Primary::Line(Point_1, Point_2);
auto Line_4 = geometry::Primary::Line(Point_2, Point_1);
```

`Primary::Line`s that are lying on the `{1, 0, 0}`, `{0, 1, 0}` , `{0, 0, 1}` and pathing though the origin (`{0, 0, 0}`) can respectively be created as
```C++
auto XAxis = geometry::Primary::XAxisLine();
auto YAxis = geometry::Primary::YAxisLine();
auto ZAxis = geometry::Primary::ZAxisLine();
```

[back to top of the page.](#Top)<br>
<a name="PrimaryGeometryStaticConstructorsPlane"/><br>
#### Plane
`Primary::Plane` can be constructed by a position (a point in 3D space) on it and a normal vector; three positions (points in 3D space); three `Primary::Point`s; a `Primary::Point` and a `Primary::Line`; and two `Primary::Line`s that intersect. Thus all the following `Primary::Plane` will evaluate to be equal.

```C++
const auto normalVector =  space::Vector<3>::crossProduct(space::Vector<3>{-1, -1, 4}, space::Vector<3>{-0.8, 1.7, 4});
auto Plane_1 = geometry::Primary::Plane(space::Point<3>{0, 2.3, 5}, normalVector);
auto Plane_2 = geometry::Primary::Plane(space::Point<3>{2, 1.3, 1}, space::Point<3>{1.2, 3, 5}, space::Point<3>{0, 2.3, 5});
auto Plane_3 = geometry::Primary::Plane(Point_3, Line_1);
auto Plane_4 = geometry::Primary::Plane(Line_1, Point_3);

const auto lineOnPlane = geometry::Primary::Line(Point_1, Point_3);
auto Plane_5 = geometry::Primary::Plane(Line_1, lineOnPlane);
auto Plane_6 = geometry::Primary::Plane(lineOnPlane, Line_1);
```

Static constructors for the cartesian coordinate system's planes are also available as 

```C++
auto XYPlane = geometry::Primary::XYPlane();
auto YXPlane = geometry::Primary::YXPlane();
auto YZPlane = geometry::Primary::YZPlane();
auto ZYPlane = geometry::Primary::ZYPlane();
auto XZPlane = geometry::Primary::XZPlane();
auto ZXPlane = geometry::Primary::ZXPlane();
```

[back to top of the page.](#Top)<br>
<a name="PrimaryGeometryStaticConstructorsSphereCylinderPlate"/><br>
#### Sphere, Cylinder, and Plate
Static `Primary::Sphere`, `Primary::Cylinder`, and `Primary::Plate` constructors available similar to their pairs without thickness (i.e., `Primary::Point`, `Primary::Line`, and `Primary::Plane` respectively). The `Primary::Sphere`, `Primary::Cylinder`, and `Primary::Plate` constructors have an extra input variable `thickness` at the end of the function input list. For instance

```C++
auto Shpere_1 = geometry::Primary::Sphere({2, 1.3, 1}, 5.5);
auto Bar_1 = geometry::Primary::Bar(space::Point<3>{2, 1.3, 1}, space::Vector<3>{-0.8, 1.7, 4}, 2);
auto Plate_1 = geometry::Primary::Plate(space::Point<3>{0, 2.3, 5}, normalVector, 1.);
```

One can also use a point in 3D space for the center and a point in 3D space on the sphere; a `Primary::Point` and a radius; and A center `Primary::Point` and a `Primary::Point` on the sphere too. All the `Primary::Sphere`s below are equal to the `Sphere_1` above.
    
```C++
auto Sphere_2 = geometry::Primary::Sphere(space::Point<3>{2, 1.3, 1}, space::Point<3>{2, 1.3, 6.5});
auto Shpere_3 = geometry::Primary::Sphere(Point_1, 5.5);

const auto pointOnSphere = geometry::Primary::Point(Point_1.positionVector() + space::Vector<3>{0, 0, 5});
auto Sphere_4 = geometry::Primary::Sphere(Point_1, pointOnSphere);
```

`Primary::Bar` has one extra static constructor that uses a `Primary::Line` as the center line of the infinite-cylindrical-bar and a radius.
```C++
auto Bar_2 = geometry::Primary::Bar(Line_1, 2);
```
`Bar_1` and `Bar_2` are also equal.

Similarly, the `Primary::Plane` has a static constructor that uses a `Primary::Plane` as the base and a thickness. 
```C++
auto Plate_2 = geometry::Primary::Plate(XYPlane, 1);
auto Plate_3 = geometry::Primary::Plate(YXPlane, 1);
```

Note that `Plate_2` and `Plate_3` are not equal since they have the same base, but they have an opposite thickness direction. See the schematic plate image in the table above. However, `Plate_2` and `Plate_4` will be evaluated to equal.

```C++
auto Plate_4 = geometry::Primary::Plate(geometry::Primary::Plane({0, 1, 0}, {0, 0, 1}), -1);
```

[back to top of the page.](#Top)<br>
<a name="PrimaryGeometryStaticConstructorsHalfspaceAndSpace"/><br>
#### Half space and full space

`geometry::Primary` provides static constructors for creating half-space and whole 3D space. These become handy when creating `geometry::Combined`s.
```C++
auto Space = geometry::Primary::Space();
auto HalfSpace = geometry::Primary::HalfSpace({0, 0, 0}, {0, 0, 1}); 
```
`HalfSpace` denotes the space of all points with positive `Z` in a cartesian or spherical coordinate system.

[back to top of the page.](#Top)<br>
<a name="CombinedGeometry"/><br>
## Combined Geometry

By combining two Primary, two Combined, or a Primary and a Combined, we can create a new Combined. For instance, 
```C++
using Operation = geometry::SetOperation; 
const double innerRadius = 1.;
const double thickness = 0.1;

const auto innerBar = geometry::Primary::Bar({0,0,0}, {0,0,1}, innerRadius); 
const auto outerBar = geometry::Primary::Bar({0,0,0}, {0,0,1}, innerRadius + thickness);
const auto infinitePipe = geometry::Combined(outerBar, Operation::Difference, innerBar);

const auto bottomCutter = geometry::Primary::HalfSpace({0,0,0}, {0,0,-1});
const auto upperCutter = geometry::Primary::HalfSpace({0,0,1}, {0,3,4});
const auto cutter = geometry::Combined(upperCutter, Operation::Union, upperCutter);

const auto shape = geometry::Combined(infinitePipe, Operation::Difference, cutter);
```
presents the following shape.

<p align="center">
  <img src="images/Combined/PipeSegment.png" width="200px"> 
  <br>Figure 3: A cut of a pipe geometry created by Combined geometry.<br>
</p>

[back to top of the page.](#Top)<br>
<a name="CombinedGeometryStaticConstructors"/><br>
### Static Constructors

The following table presents all of the static constructors available.

```C++
using Point = space::Point<3>;
using Vector = space::Vector<3>;
using OptionalVector = const std::shared_ptr<Vector>&
```

|  Schematic Diagram                                                | Static Constructor                                                                        | 
|:-----------------------------------------------------------------:|:-----------------------------------------------------------------------------------------:|
| <img src="images/Combined/Line_1.png" width="420px">              | Line(Point point, Vector directionVector)                                                 |
| <img src="images/Combined/Line_2.png" width="420px">              | Line(Point point1, Point point2) <br> Line(Primary point1, Primary point2)                |
| <img src="images/Combined/LineRay.png" width="420px">             | LineRay(Point point, Vector directionVector)                                              |
| <img src="images/Combined/LineSegment_1.png" width="420px">       | LineSegment(Point startingPoint, Point endPoint)                                          |
| <img src="images/Combined/LineSegment_2.png" width="420px">       | LineSegment(Point midPoint, Vector radiusVector)                                          |
| <img src="images/Combined/Point.png" width="420px">               | Point(std::initializer_list<double> values) <br> Point(Vector positionVector) <br> Point(Point point) |
| <img src="images/Combined/Circle.png" width="420px">              | Circle(Point center, Vector planeNormal, Vector radiusVector)                             |
| <img src="images/Combined/EquilateralTriangle_1.png" width="420px"> | EquilateralTriangle(Point center, Point vertex, Vector vectorOnPlane)                   |
| <img src="images/Combined/EquilateralTriangle_2.png" width="420px"> | EquilateralTriangle(Point center, Vector planeNormal, Vector radiusVector)              |
| <img src="images/Combined/Parallelogram.png" width="420px">       | Parallelogram(Point vertex, Vector vector1, Vector vector2)                               |
| <img src="images/Combined/Plane_1.png" width="420px">             | Plane(Point point, Vector normalVector)                                                   |
| <img src="images/Combined/Plane_2.png" width="420px">             | Plane(Point point1, Point point2, Point point3)                                           |
| <img src="images/Combined/Plane_3.png" width="420px">             | Plane(Primary line1, Primary line2)                                                       |
| <img src="images/Combined/Plane_4.png" width="420px">             | Plane(Primary point, Primary line)                                                        |
| <img src="images/Combined/Rectangle_1.png" width="420px">         | Rectangle(Point vertex, Point vertex2, Point vertex3)                                     |
| <img src="images/Combined/Rectangle_2.png" width="420px">         | Rectangle(Point vertex, Vector vector1, Vector vector2)                                   |
| <img src="images/Combined/Rhombus_1.png" width="420px">           | Rhombus(Point vertex, Vector vector1, Vector vector2)                                     |
| <img src="images/Combined/Rhombus_2.png" width="420px">           | Rhombus(Point vertex1, Point vertex2, Point vertex3)                                      |
| <img src="images/Combined/RegularPolygon_1.png" width="420px">    | RegularPolygon(Point pointCenter, Vector planeNormal, Vector radiusVector, unsigned long numberOfVertices) |
| <img src="images/Combined/RegularPolygon_2.png" width="420px">    | RegularPolygon(Point pointCenter, Point vertex, Vector vectorOnPlane, unsigned long numberOfVertices) |
| <img src="images/Combined/Square_1.png" width="420px">            | Square(Point center, Point vertex, Vector vectorOnPlane)                                  |
| <img src="images/Combined/Square_2.png" width="420px">            | Square(Point center, Vector planeNormal, Vector radiusVector)                             |
| <img src="images/Combined/Square_3.png" width="420px">            | Square(Point vertex1, Point vertex2, Point vertex3)                                       |
| <img src="images/Combined/Trapezoid.png" width="420px">           | Trapezoid(Point vertex1, Point vertex2, Vector vector1, Vector vector2)                   |
| <img src="images/Combined/Triangle.png" width="420px">            | Triangle(Point vertex1, Point vertex2, Point vertex3)                                     |
| <img src="images/Combined/Bar_1.png" width="420px">               | Bar(Point centerPoint, Vector directionVector, double radius)                             |
| <img src="images/Combined/Bar_2.png" width="420px">               | Bar(Primary centerLine, double radius)                                                    |
| <img src="images/Combined/Cube_1.png" width="420px">              | Cube(Point sideCenter, Vector sideNormalVector, Vector radiusVector)                      |
| <img src="images/Combined/Cube_2.png" width="420px">              | Cube(Point vertex, Vector vector1, Vector vector2, Vector vector3, double dimension)      |
| <img src="images/Combined/Cuboid_1.png" width="420px">            | Cuboid(Point vertex,  Vector vector1, Vector vector2, Vector vector3)                     |
| <img src="images/Combined/Cuboid_2.png" width="420px">            | Cuboid(Point vertex, Point adjacentVertex1, Point adjacentVertex2, Point adjacentVertex3) |
| <img src="images/Combined/Cylinder.png" width="420px">            | Cylinder(Point sideCenter1, Point sideCenter2, double radius)                             |
| <img src="images/Combined/EquilateralTrianglePrism.png" width="420px"> | EquilateralTrianglePrism(Point sideCenter, Vector sideNormalVector, Vector radiusVector, double height) |
| <img src="images/Combined/Parallelepiped_1.png" width="420px">    | Parallelepiped(Point vertex,  Vector vector1, Vector vector2, Vector vector3)             |
| <img src="images/Combined/Parallelepiped_2.png" width="420px">    | Parallelepiped(Point vertex, Point adjacentVertex1, Point adjacentVertex2, Point adjacentVertex3) |
| <img src="images/Combined/Plate_1.png" width="420px">             | Plate(Point point, Vector normalVector, double thickness)                                 |
| <img src="images/Combined/Plate_2.png" width="420px">             | Plate(Primary plane, double thickness)                                                    |
| <img src="images/Combined/Pyramid.png" width="420px">             | Pyramid(Point baseCenter, Vector baseNormal, Point tip, Vector radiusVector, unsigned int numberOfVertices) |
| <img src="images/Combined/RegularPrism.png" width="420px">        | RegularPrism(Point sideCenter1, Point sideCenter2, Vector radiusVector,unsigned int numberOfVertices) |
| <img src="images/Combined/RegularPyramid.png" width="420px">      | Pyramid(Point baseCenter, Vector heightVector, Vector radiusVector, unsigned int numberOfVertices) |
| <img src="images/Combined/SquarePyramid.png" width="420px">       | RegularSquarePyramid(Point baseCenter, Vector heightVector, Vector radiusVector) <br> SquarePyramid(Point baseCentre, Vector baseNormalVector, Point tip, Vector radiusVector)                                                                        |
| <img src="images/Combined/RegularTetrahedron.png" width="420px">  | RegularTetrahedron(Point baseCenter, Vector heightVector, Vector radiusVector)            |
| <img src="images/Combined/Sphere_1.png" width="420px">            | Sphere(Point center, Point aPointOnShpere)                                                |
| <img src="images/Combined/Sphere_2.png" width="420px">            | Sphere(Point center, double radius)                                                       |
| <img src="images/Combined/TruncatedCylinder.png" width="420px">   | TruncatedCylinder(Point sideCentre1, Point sideCentre2, double radius, OptionalVector sideVector1 = nullptr,  OptionalVector sideVector2 = nullptr) |
| <img src="images/Combined/Tetrahedron.png" width="420px">          | Tetrahedron(Point baseCenter, Vector baseNormalVector, Point tip, Vector radiusVector)   |


[back to top of the page.](#Top)<br>
<a name="ComplexGeometries"/><br>
# Complex Geometries

There are two possible ways to create complex geometries that cannot be presented by the `geometry::Primary` or `geometry::Combined`.
  First, we can introduce a new `geometry::Complex` class that inherits the `geometry::Geometry` interface and implements the new form of geometries.
  Second, using a combination of the `geometry::Primary` and `geometry::Combined` with the coordinate systems.
The first approach should be considered if the new `geometry::Complex` class would cover a wide range of known or most likely to be used in engineering. However, the second approach is more preferred if the geometry is unique in terms of mathematical parameters.
Here we create an ellipsoid and an elliptical paraboloid plate to present the second approach.

<a name="Ellipsoid"/><br>
## Ellipsoid
First, we define a new custom local coordinate system by appending one to the Global coordinate system. The new coordinate system is cartesian, where its X-axis unit vector (i.e., i vector) has a length of 2.
```C++
using namespace coordinate_system;
auto& globalCS = CoordinateSystem::Global();
auto& localCS = globalCS.appendCustomLocal({0, 0, 0},                                    // origin
                                           {2, 0, 0}, {0, 1, 0}, {0, 0, 1},              // i, j, k vectors
                                           convertors::cartesian::toCartesian(),         // mapping
                                           convertors::cartesian::toCartesianInverse()); // inverse mapping
```

Next, we create a sphere that later we will use inside the local coordinate system.
```C++
auto sphere = geometry::Primary::Sphere({0, 0, 0}, 2.5);
```

By meshing the local coordinate system and include the points inside the sphere locally, we would have our ellipsoid in the global coordinate system. 
```C++
grid::cartesian::uniformAroundOrigin(space::consts::one3D * 0.05, space::consts::one3D * sphere.thickness() + 0.05, localCS.axes());

localCS.include([sphere](const space::Point<3>& point) {
    return sphere.pointStatus(point) != geometry::PointStatus::Outside;
});
```

To present this, we can export the local and global coordinate systems as
```C++
const auto& localPoints = localCS.getAllPoints();

auto file_local = exporting::VTKFile(path, "local", "vtk");
file_local.appendCell(exporting::vtk::Cell::PolyVertex, exporting::vtk::convertors::convertToVertexes(localPoints));
file_local.assemble();

auto file_global = exporting::VTKFile(path, "global", "vtk");
for(const auto& localPoint : localPoints) {
    const auto globalPoint = globalCS.convert(localPoint, localCS);
    file_global.appendCell(exporting::vtk::Cell::Vertex, exporting::vtk::convertors::convertToVertexes({globalPoint}));
}
file_global.assemble();
```

which result to the following configurations.

|  Local Coordinate Sytem                                     | Global Coordinate Sytem                                      | 
|:-----------------------------------------------------------:|:------------------------------------------------------------:|
| <img src="images/Complex/ellipsoid_local.png" width="80%">  | <img src="images/Complex/ellipsoid_global.png" width="100%"> |

Note that the ellipsoid's points in the global coordinate system have twice of distance in the X-direction than in other directions. To make them the same, we should change the meshing to
```C++
grid::cartesian::uniformAroundOrigin(space::Vector<3>{0.5, 1, 1} * 0.05, space::consts::one3D * sphere.thickness() + 0.05, localCS.axes());
```

[back to top of the page.](#Top)<br>
<a name="EllipticalParaboloidPlate"/><br>
## Elliptical Paraboloid Plate

Unlike the [ellipsoid](#Ellipsoid)  example, we cannot convert any `geometry::Primary` or `geometry::Combined` to an elliptical paraboloid plate by changing the coordinate system unit vectors. Here we need to create a new coordinate system with implementing required mapping and inverse mapping functions.

First, we need to define the mapping and inverse mapping. 
```C++
using Point = space::Point<3>;
using Vector = space::Vector<3>;

const auto mapping =  [](const Point& point, const Point& origin, const Vector& i,  const Vector& j, const Vector& k) {
    const auto& positionVector = point.positionVector(); // The position vector of the point in parent coordinate system.
    return Point{
        positionVector[0],
        positionVector[1],
        0
    };
};

const auto inverseMapping = [](const Point& point, const Point& origin, const Vector& i, const Vector& j, const Vector& k) {
    const auto& positionVector = point.positionVector(); // The position vector of the point in child coordinate system.
    return Point{
        positionVector[0],
        positionVector[1],
        (pow(positionVector[0], 2) + pow(positionVector[1] / 2, 2))
    };
};
```
Note that one may consider the rotation and transition of the local coordinate system, here, I ignored those for the sake of simplicity.

Next, we define a new custom local coordinate system by appending one to the Global coordinate system. The new coordinate system is a custom coordinate system, where its origin and unit vector are the same as the global coordinate system, while its mapping and inverse mapping are the above functions.
```C++
using namespace coordinate_system;
auto& globalCS = CoordinateSystem::Global();
auto& localCS = globalCS.appendCustomLocal(space::consts::o3D,
                                           space::consts::i3D, space::consts::j3D, space::consts::k3D,
                                           mapping, inverseMapping);
```

Next, we create a plate that later we will use inside the local coordinate system.
```C++
const auto plateThickness = 0.04;
auto plate = geometry::Primary::Plate({0, 0, -plateThickness / 2}, space::consts::k3D, plateThickness);
```

By meshing the local coordinate system and include the points inside the sphere locally, we would have our ellipsoid in the global coordinate system. 
```C++
grid::cartesian::uniformAroundOrigin(space::consts::one3D * 0.02, {5, 10, plateThickness}, localCS.axes());

localCS.include([plate](const space::Point<3>& point) 
    return plate.pointStatus(point) != geometry::PointStatus::Outside;
});
```

To present this, we can export the local and global coordinate systems as
```C++
const auto& localPoints = localCS.getAllPoints();

auto file_local = exporting::VTKFile(path, "local", "vtk");

file_local.appendCell(exporting::vtk::Cell::PolyVertex, exporting::vtk::convertors::convertToVertexes(localPoints));
file_local.assemble();

auto file_global = exporting::VTKFile(path, "global", "vtk");
for(const auto& localPoint : localPoints) {
    const auto globalPoint = globalCS.convert(localPoint, localCS);
    file_global.appendCell(exporting::vtk::Cell::Vertex, exporting::vtk::convertors::convertToVertexes({globalPoint}));
}
file_global.assemble();
```

which result to the following configurations.

<p align="center">
  <img src="images/Complex/ellipticalParaboloidPlate.png" width="40%"> 
  <br>Figure 4: The local and global elliptical paraboloid plate.<br>
</p>

[back to top of the page.](#Top)<br>
Copyright (c) 2021-present, Ali Jenabidehkordi

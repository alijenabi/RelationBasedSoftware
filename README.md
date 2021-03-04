
# Relation-Based Software (RBS)

![size](https://img.shields.io/github/repo-size/alijenabi/RelationBasedSoftware)
![license](https://img.shields.io/badge/license-MIT-green)

This is the implementation of a software architecture that I developed during my Ph.D. program. The architecture is developed to be open enough that researchers can be expanding it to their newly developed method or extend the previous method farther without the need to reinvent the wheel. Unlike most other libraries and codes, I tried not to create new scripting or language but rather keep naming and style close to the mechanical and civil engineering understanding. 
The key highlight of the code is perhaps the Relations; they allow the researcher to change one datatype by the change of the other datatype. For instance, a constitutive model will change nodes' properties when the simulation time changes. 

## Am I in the right place?

If you answer one of the following questions with NO, this code probably is no help to you.
* Do you have knowledge of numerical simulations?
* Can you imagine using C++17 for your models?
* You appreciate to have one of the followings for your simulation:
  - multiple neighborhood searches, dynamic neighborhood search, or fast neighborhood search in the initial configuration.
  - creating geometry and meshing with code rather than importing them form CAD software. 
  - using Bond-Based and Ordinary-State-Based Peridynamics.
  - using Solid transition and rotation.
  - ability to create directional bonds with different properties connectiong to bodies.
  - ability to export the initial and current configuration to VTK (i.e., [ParaView](https://www.paraview.org)).

## Prerequisites
The code uses some of the `C++17` features, thus a compatible compiler is required.

## Installing
No installation required, simply download or fork the code and start using it.

## Using
The best way to start using this code is to follow one of the simulations below.
* [Elastic Wave Propagation in Plate](simulations/Elastic%20Wave%20Propagation%20in%20Plate/Elastic%20Wave%20Propagation%20in%20Plate.md)
* [Plate with a pre-existing crack](simulations/Plate%20with%20a%20pre-existing%20crack/PlateWithPreExistingCrack.md) 

I intend to add more examples in the future.

## Documentation
Since this code is open-source, everyone can access the implementation. Thus, no web-based documentation is intended, while the code is well documented using the  [Doxygen](https://www.doxygen.nl/index.html), redundancies are removed. For instance, you do not need to have a @return for getters and setters.   

Additionally, some of the new topics by this code are explained in detail. Use the following links to navigate to those topics. I will add more to this list if I see that users have difficulty understanding a topic.
* [Configuration](documentation/configuration/README.md)
* [Coordinate System](documentation/coordintate_system/README.md)
* [Geometry](documentation/geometry/README.md)
* [Relation](documentation/relations/README.md)

## Contributing
To contribute to `RBS`, follow these steps:

1. Fork this repository. 
2. Create a branch: `git checkout -b '<feature/mycoolfeature>'`.
3. Make your changes and commit them: `git commit -m '<your_commit_message>'`
4. Push to the original branch: `git push origin '<feature/mycoolfeature>'`
5. Create the pull request.

Alternatively see the GitHub documentation on [creating a pull request](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request).

## License
[MIT](http://opensource.org/licenses/MIT)

Copyright (c) 2020-present, Ali Jenabidehkordi

# ASCENT
### An open source C++ architecture for time-based simulations.
#### Developed by [Anyar, Inc.](http://www.anyarinc.com/)

Ascent is a module-based simulation architecture that can handle extremely complex and dynamic systems. 
The architecture is built around a powerful ordinary differential equation solver, but Ascent can do far more than solve differential equations.

### [Ascent Website](http://anyarinc.github.io/ascent/)

### Why build simulations with Ascent?
- **Modular**: Share and reuse modules.
- **Object Oriented**: Polymorphic module handling.
- **Automatic Simulation Ordering**: Ascent automatically orders the flow of the simulation, which allows a simulation designer to develop and solve highly modular and complex systems.
- **Asynchronous Sampling and Event Scheduling**
- **Run-Time Dynamic Systems**: Allows dynamic module creation, deletion, linking, and ordering, all properly handled for correct numerical integration.
- **Fast Running**: Insofar as to not sacrifice dynamic behavior.
- **Simulators Can Run On Separate Threads**
- **Integrators**: Runge Kutta, Dormand Prince, and multiple real-time predictor-correctors. Some integrators support adaptive stepping.
- **Built In Variable Tracking**: Easily record and output time history of integers, doubles, vectors, and even custom data types.
- **ChaiScript Embedded Scripting Language**: Easily connect, initialize and run your modules from a powerful scripting engine.
- **Eigen C++ Linear Algebra Library**: Ascent utilizes the mature Eigen library, providing straightforward matrix and vector handling.
- **Easy To Begin!**: Only two classes (asc::Module and asc::Link) are required to solve most differential equations or systems.

### Building The Ascent Library
- Clone Ascent
- Install the [CMake](https://cmake.org/) GUI and use it to generate a build directory within the Ascent directory
- Or, if you're on Windows, just double click the build.bat file to build Visual Studio 14 2015 Win64 project and libraries.
_You can edit the batch file and change the generator to another compatable CMake generator._

Ascent requires a C++14 compliant compiler.

_master_ [![Build Status](https://travis-ci.org/AnyarInc/ascent.svg?branch=master)](https://travis-ci.org/AnyarInc/ascent)     _develop_ [![Build Status](https://travis-ci.org/AnyarInc/ascent.svg?branch=develop)](https://travis-ci.org/AnyarInc/ascent)  

### [Ascent Community](https://github.com/Ascent-Community)

Ascent was designed to allow any modules built on Ascent to be easily shared and interfaced with other Ascent modules.  
_Share your open source modules and libraries built on Ascent in the Ascent-Community repositories._

### [Support](http://anyarinc.github.io/ascent/support.html)

[![Join the chat at https://gitter.im/AnyarInc/ascent](https://badges.gitter.im/AnyarInc/ascent.svg)](https://gitter.im/AnyarInc/ascent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

### Contributions are Welcome!

- [Q&A forum](https://groups.google.com/forum/#!forum/Ascent-Users)
- [Report an issue](https://github.com/AnyarInc/ascent/issues)

Please report bugs, offer fixes, contribute modules, ask questions and answer questions. The more the merrier!

For those interested in contributing to the core of Ascent, see [For Architects of Ascent](https://github.com/AnyarInc/Ascent/wiki/For-Architects-of-Ascent), which explains more details of architecture design/goals.

### Projects using Ascent
- Anyar, Inc. uses Ascent for high fidelity, six degree of freedom missile modeling.
- Anyar, Inc. maintains viscoelastic human injury blunt and blast trauma software built on Ascent.  
_Share your projects using Ascent._
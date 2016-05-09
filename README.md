# ASCENT <a href="http://anyarinc.github.io/ascent/"> <img src="http://www.iconsdb.com/icons/preview/color/299BFF/house-xxl.png" width="25"></a>

[![Build Status](https://travis-ci.org/AnyarInc/ascent.svg?branch=master)](https://travis-ci.org/AnyarInc/ascent) [![Join the chat at https://gitter.im/AnyarInc/ascent](https://badges.gitter.im/AnyarInc/ascent.svg)](https://gitter.im/AnyarInc/ascent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
### An open source C++ architecture for time-based simulations.

Ascent is a module-based simulation architecture that can handle extremely complex and dynamic systems. 
The architecture is built around a powerful ordinary differential equation solver, but Ascent can do far more than solve differential equations.

### Why build simulations with Ascent?
- **Modular**: Ascent was designed to allow any modules built on Ascent to be easily shared and interfaced with other Ascent modules.  
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
- **Community**: Share your open source modules and libraries built on Ascent in the Ascent-Community repositories

### Features

***
## Prerequisites
- **C++14 compliant compiler**
- [CMake](https://cmake.org/download/) Cross-Platform build system (Required)  <a href="http://anyarinc.github.io/ascent/"> <img src="http://www.iconsdb.com/icons/preview/color/299BFF/data-transfer-download-xxl.png" width="20"></a> 
- [Eigen](http://eigen.tuxfamily.org/) C++ Linear Algebra Library (included)
- [ChaiScript](http://chaiscript.com/) Embedded Scripting Language (included)


## Setup
> Note: Edit the batch file to change the default generator (_Visual Studio 14 2015 Win64_) to another compatable CMake generator.

```
git clone https://github.com/AnyarInc/ascent.git
cd ascent
build
```
***
## Support

- [Ascent Website](http://anyarinc.github.io/ascent/)
- [Gitter](https://gitter.im/AnyarInc/ascent)
- [Q&A forum](https://groups.google.com/forum/#!forum/Ascent-Users)
- [Support Page](http://anyarinc.github.io/ascent/support.html)

## Contributions are Welcome!

- [Report an issue](https://github.com/AnyarInc/ascent/issues)
- [Ascent Community](https://github.com/Ascent-Community)
- [For Architects of Ascent](https://github.com/AnyarInc/Ascent/wiki/For-Architects-of-Ascent)


## Developed by [Anyar, Inc.](http://www.anyarinc.com/)
- high fidelity, six degree of freedom missile modeling
- viscoelastic human injury 
- blunt and blast trauma

![alt text][logo]
[logo]: http://www.anyarinc.com/wp-content/uploads/2015/06/anyar-logo-1.png "Anyar, Inc."
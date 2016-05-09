# ASCENT <a href="http://anyarinc.github.io/ascent/"> <img src="http://www.iconsdb.com/icons/preview/color/299BFF/house-xxl.png" width="25"></a>

[![Build Status](https://travis-ci.org/AnyarInc/ascent.svg?branch=master)](https://travis-ci.org/AnyarInc/ascent) [![Join the chat at https://gitter.im/AnyarInc/ascent](https://badges.gitter.im/AnyarInc/ascent.svg)](https://gitter.im/AnyarInc/ascent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Ascent is a modern, open source, C++ architecture for time and module based simulations. Built around a powerful ordinary differential equation solver, Ascent manages complex and dynamic systems, numerical integration, and the complete simulation lifecycle.

## Why build simulations with Ascent?
- **Modular**: Interface with and share modules built on Ascent. 
- **Object Oriented**: Polymorphic module handling.
- **Fast**: Dynamic run-time behavior from back-end C++ libraries.
- **Thread Safe**: Run simulators on separate threads for maximum performance.
- **Easy To Begin!**: Only two classes (asc::Module and asc::Link) are required to solve most differential equations or systems.
- **Community**: Share your open source modules and libraries built on Ascent in the Ascent-Community repositories.

## Features
- **Automatic Simulation Ordering**: Develop and solve modular, complex systems with automatic ordering of simulation flow.
- **Run-Time Dynamic Systems**: Allows dynamic module creation, deletion, linking, and ordering for numerical integration.
- **Variable Tracking**: Record time history of integers, doubles, vectors, and custom data types.
- **Asynchronous Sampling**
- **Event Scheduling**
- **Integrators**
    - Runge Kutta
    - Dormand Prince
    - multiple real-time predictor-correctors. 
    - Some integrators support adaptive stepping


***
## Prerequisites
- **C++14 compliant compiler**
- [CMake](https://cmake.org/download/) Cross-Platform build system (<font color="red">Required</font>)  <a href="http://anyarinc.github.io/ascent/"> <img src="http://www.iconsdb.com/icons/preview/color/299BFF/data-transfer-download-xxl.png" width="20"></a> 
- [Eigen](http://eigen.tuxfamily.org/) C++ Linear Algebra Library (<font color="green">Included</font>)
- [ChaiScript](http://chaiscript.com/) Embedded Scripting Language (<font color="green">Included</font>)



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

<a href="http://anyarinc.com"> <img src="http://www.anyarinc.com/wp-content/uploads/2015/06/anyar-logo-1.png"></a>
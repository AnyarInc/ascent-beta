# ASCENT-beta <a href="http://anyarinc.github.io/ascent-beta/"> <img src="http://www.iconsdb.com/icons/preview/color/299BFF/house-xxl.png" width="25"></a>

[![Build Status](https://travis-ci.org/AnyarInc/ascent.svg?branch=master)](https://travis-ci.org/AnyarInc/ascent) [![Join the chat at https://gitter.im/AnyarInc/ascent](https://badges.gitter.im/AnyarInc/ascent.svg)](https://gitter.im/AnyarInc/ascent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

### This version of Ascent, which is now known as Ascent-beta (previously Ascent), is being replaced by a new Ascent framework. The new version of Ascent is built from the ground up to be the fastest and most versatile modular simulation engine and Ordinary Differential Equation solver that we know of.
### Now live here: [Ascent](https://github.com/AnyarInc/Ascent)
### Benefits of the new Ascent architecture:
- **Optimized For Maximum Speed**: We've seen up to 12X speed improvements.
- **Header Only**
- **Solve State Space Systems**
- **Better Scripting**
- **Better Multi-Threading**
- **And More...**

## Ascent-beta
Ascent is a modern, open source, C++ simulation architecture and engine. Built around a powerful ordinary differential equation solver, Ascent manages complex and dynamic systems, numerical integration, and the complete simulation lifecycle.

## <a href="http://anyarinc.github.io/ascent-beta/">Website</a>

## Why build simulations with Ascent?
- **Modular**: Interface with and share modules built on Ascent. 
- **Object Oriented**: Polymorphic module handling.
- **Fast**: Optimized for dynamic systems.
- **Community**: Share your open source modules and libraries built on Ascent in the Ascent-Community repositories.
- **Easy To Begin!** Solve most differential equations or systems with two classes (asc::Module and asc::Link).

## Features
- **Automatic Simulation Ordering**: Develop and solve modular, complex systems with automatic ordering of simulation flow.
- **Run-Time Dynamic Systems**: Allows dynamic module creation, deletion, linking, and ordering.
- **Variable Tracking**: Record time history of intrinsic and custom data types.
- **Asynchronous Sampling**
- **Asynchronous Event Scheduling**
- **Integrators**
    - Runge Kutta (2nd, 4th, Merson)
    - Dormand Prince (45, 87, with adaptive stepping)
    - Multiple real-time predictor-correctors


***
## Prerequisites
- **C++14 compliant compiler**
- [CMake](https://cmake.org/download/) Cross-Platform build system (<font color="red">Required</font>)  <a href="https://cmake.org/download/"> <img src="http://www.iconsdb.com/icons/preview/color/299BFF/data-transfer-download-xxl.png" width="20"></a>
- [ChaiScript](http://chaiscript.com/) Embedded Scripting Language (<font color="green">Included</font>)
- [Eigen](http://eigen.tuxfamily.org/) Linear Algebra Library (<font color="green">Included</font>)
- [jsoncons](https://github.com/danielaparker/jsoncons) JSON Construction/Parsing (<font color="green">Included</font>)

> Clone recursively to include dependencies.

## Setup
> To change the default generator (_Visual Studio 14 2015 Win64_), edit _build.bat_ to another compatible CMake generator.

```
git clone https://github.com/AnyarInc/ascent.git
cd ascent
build
```
***
## Support

- [Gitter](https://gitter.im/AnyarInc/ascent)
- [Support Page](http://anyarinc.github.io/ascent/support.html)
- [Wiki](https://github.com/AnyarInc/ascent/wiki)

## Contributions are Welcome!

- [Report an issue](https://github.com/AnyarInc/ascent/issues)
- [Ascent Community](https://github.com/Ascent-Community)
- [For Architects of Ascent](https://github.com/AnyarInc/Ascent/wiki/For-Architects-of-Ascent)

***
## Developed by [Anyar, Inc.](http://www.anyarinc.com/) for
- High fidelity, six degree of freedom missile modeling
- Viscoelastic human injury models (blunt and blast trauma)

<a href="http://anyarinc.com"> <img src="http://www.anyarinc.com/wp-content/uploads/2015/06/anyar-logo-1.png"></a>

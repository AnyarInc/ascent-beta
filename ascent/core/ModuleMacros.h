// Copyright (c) 2015 - 2016 Anyar, Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#define EIGEN_MPL2_ONLY // Ensure that Eigen license is MPL2 compatible.

// Check 32bit vs 64bit for Eigen alignment
#if _WIN32 || _WIN64
#if _WIN64
#define ENV64BIT
#else
#define ENV32BIT
#endif
#endif

#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENV64BIT
#else
#define ENV32BIT
#endif
#endif

#ifdef ENV32BIT
#define EIGEN_DONT_ALIGN_STATICALLY
#endif

#include "ascent/core/LinkBase.h"
#include "ascent/core/State.h"
#include "ascent/core/Type.h"
#include "ascent/core/Vars.h"

/** This macro allows a child of Module to call Module methods from ChaiScript.
* Example: Spring Constructor
* @code
* Spring::Spring(size_t sim) : asc::Module(sim)
* {
*     ascModule(Spring);
* }
* @endcode
* The above code registers the Spring class with ChaiScript, allowing a Spring instance to call Module methods like run(), track(), outputTrack(), etc.
* This registration also allows asc::Link<Spring> intances in ChaiScript to access their internal module pointer.
* Example: ChaiScript
* @code
* var spring = [some user registered function that returns an asc::Link<Spring>]
* print(spring.module.force);
* @endcode
*/
#define ascModule(asc_module)\
if (!chai.modules.count(#asc_module)) {\
   chai.add(chaiscript::base_class<asc::Module, std::decay<decltype(*this)>::type>());\
   chai.add(chaiscript::base_class<asc::LinkBase, asc::Link<std::decay<decltype(*this)>::type>>());\
   chai.modules.insert(#asc_module);\
}

/** The ascVar macro registers a variable with its respective class for ChaiScript and defines the variable for tracking.
* This allows the variable to be tracked in code or in ChaiScript and allow it to be accessed and set in ChaiScript.
*/
#define ascVar(x) define(#x, x);\
if (!chai.registered(typeid(*this).name(), #x)) {\
   chai.add(chaiscript::fun(static_cast<std::decay<decltype(x)>::type (ascNS::*)>(&ascNS::x)), #x);\
   chai.chai_rg[typeid(*this).name()].push_back(#x);\
}

/** Intended to be hidden from the user, these conversions provide assignment between LinkBase classes and allow an uncontained module to be assigned to a Link container.
*/
namespace asc {
   namespace hidden {
      inline void assignModule(asc::LinkBase& link_base, asc::Module& module) { link_base.assign(module); }
      inline void assignLinkBase(asc::LinkBase& lhs, asc::LinkBase& rhs) { lhs.assignLinkBase(rhs); }
   }
}

/** ascLink registers an asc::Link<T> member with its respective class for ChaiScript.
* This allows a Module in ChaiScript to access the Link members within the class.
*/
#define ascLink(x) \
if (!chai.registered(typeid(*this).name(), #x)) {\
   chai.add(chaiscript::fun(static_cast<std::decay<decltype(x)>::type (ascNS::*)>(&ascNS::x)), #x);\
   chai.chai_rg[typeid(*this).name()].push_back(#x);\
}\
defineLink(#x, x);
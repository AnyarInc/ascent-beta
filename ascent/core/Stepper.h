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

// Stepper must contain only references so that it can be effectively copied.

#include <stddef.h> // needed for size_t in LLVM (Xcode)

namespace asc
{
   class Stepper
   {
   public:
      Stepper(double& EPS, double& dtp, double& dt, double& t, double& t1, size_t& kpass, bool& integrator_initialized) :
         EPS(EPS), dtp(dtp), dt(dt), t(t), t1(t1), kpass(kpass), integrator_initialized(integrator_initialized) {}

      double& EPS;
      double& dtp; // base time step of run loop
      double& dt; // time step (can be changed by sampling and events)
      double& t; // time
      double& t1; // intended end time of next timestep
      size_t& kpass; // internal integrator pass

      bool& integrator_initialized; // whether or not the integration scheme has been initialized (i.e. for a predictor-corrector or DOPRI45), not used for basic schemes like RK4
   };
}
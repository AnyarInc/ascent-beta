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

// Six pass, fifth-order Dormand-Prince algorithm

#include "ascent/core/StateStepper.h"

namespace asc
{
   class DOPRI45 : public StateStepper
   {
   public:
      DOPRI45(Stepper &stepper) : StateStepper(x, xd, stepper) {}
      DOPRI45(double &x, double &xd, Stepper &stepper) : StateStepper(x, xd, stepper) {}

      DOPRI45* factory(double &x, double &xd) { return new DOPRI45(x, xd, static_cast<Stepper&>(*this)); }

      void propagate();
      void updateClock();
      double optimalTimeStep();
      bool adaptiveFSAL() { return true; }

      double t0;
      double xd0, xd1, xd2, xd3, xd4, xd5;
   };
}
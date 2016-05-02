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

// Two pass (stage) integrator with fourth order error term.

// Real Time (RT) Adam's Moulton predictor-corrector integration
// Source: R.M. Howe. A new family of real-time predictor-corrector integration algorithms. The University of Michigan. September 1991.

#include "ascent/integrators/RK4.h"

#include <memory>

namespace asc
{
   class RTAM4 : public StateStepper
   {
   public:
      RTAM4(Stepper &stepper) : StateStepper(x, xd, stepper), initializer(new RK4(stepper)) {}
      RTAM4(double &x, double &xd, Stepper &stepper) : StateStepper(x, xd, stepper), initializer(new RK4(x, xd, stepper)) {}

      RTAM4* factory(double &x, double &xd) { return new RTAM4(x, xd, static_cast<Stepper&>(*this)); }

      void propagate();
      void updateClock();

      std::unique_ptr<RK4> initializer;
      unsigned init_step = 0; // initialization step counter
      double xd0;
      double xd_1; // -1, previous time step derivative
      double xd_2; // -2, two steps back
      double xd_3; // -3, three steps back
   };
}
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

// P-2/PC-3/C-3 algorithm, which has the same error coefficient and order as the P-3/PC-3/C-3 predictor-corrector, but is more stable

// Real Time (RT) Adam's Moulton predictor-corrector integration
// Source: R.M. Howe. A new family of real-time predictor-corrector integration algorithms. The University of Michigan. September 1991.

#include "ascent/integrators/RK4.h"

#include <memory>

namespace asc
{
   class PC233 : public StateStepper
   {
   public:
      PC233(Stepper &stepper) : StateStepper(x, xd, stepper), initializer(new RK4(stepper)) {}
      PC233(double &x, double &xd, Stepper &stepper) : StateStepper(x, xd, stepper), initializer(new RK4(x, xd, stepper)) {}

      PC233* factory(double &x, double &xd) { return new PC233(x, xd, static_cast<Stepper&>(*this)); }

      void propagate();
      void updateClock();

      std::unique_ptr<RK4> initializer;
      double xd0;
      double xd_1; // -1, previous time step derivative
   };
}
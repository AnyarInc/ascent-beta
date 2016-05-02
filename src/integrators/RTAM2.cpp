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

#include "ascent/integrators/RTAM2.h"

using namespace asc;
using namespace std;

void RTAM2::propagate()
{
   if (!integrator_initialized)
   {
      if (0 == kpass) // if first time derivative is calculated
         xd_1 = xd;

      initializer->propagate();
   }
   else
   {
      switch (kpass)
      {
      case 0:
         x0 = x;
         x = x0 + dt / 8.0 * (5.0*xd - xd_1); // X(n + 1/2), half step computation
         xd_1 = xd; // current derivative value will be past derivative value
         break;
      case 1:
         x = x0 + dt * xd;
         break;
      }
   }
}

void RTAM2::updateClock()
{
   // Called once per integration stage
   // Do not perform operations specific to a state here

   if (!integrator_initialized)
   {
      initializer->updateClock();

      if (0 == kpass)
         integrator_initialized = true;
   }
   else
   {
      if (kpass == 0)
         t += dt / 2.0;
      else if (kpass == 1)
         t = t1;

      ++kpass;
      kpass = kpass % 2;
      if (kpass == 0)
         t1 = floor((t + EPS) / dtp + 1) * dtp;
   }
}
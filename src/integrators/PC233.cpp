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

#include "ascent/integrators/PC233.h"

using namespace asc;
using namespace std;

void PC233::propagate()
{
   if (!integrator_initialized)
   {
      if (0 == kpass) // if first time derivative is calculated
         xd_1 = xd;

      initializer->propagate();
   }
   else
   {
      static const double c0 = 1.0 / 18.0;
      static const double c1 = 1.0 / 54.0;
      static const double c2 = 1.0 / 4.0;

      switch (kpass)
      {
      case 0:
         x0 = x;
         xd0 = xd;
         x = x0 + c0 * dt * (7.0*xd - xd_1); // X(n + 1/3), third step computation
         break;
      case 1:
         x = x0 + c1 * dt * (39.0*xd - 4.0*xd0 + xd_1); // X(n + 2/3), two thirds step computation
         break;
      case 2:
         x = x0 + c2 * dt * (xd0 + 3.0*xd);
         xd_1 = xd0;
         break;
      }
   }
}

void PC233::updateClock()
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
      static const double r = 1.0 / 3.0;
      if (kpass == 0)
         t += r * dt;
      else if (kpass == 1)
         t += r * dt;
      else if (kpass == 2)
         t = t1;

      ++kpass;
      kpass = kpass % 3;
      if (kpass == 0)
         t1 = floor((t + EPS) / dtp + 1) * dtp;
   }
}
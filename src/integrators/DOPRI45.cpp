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

#include "ascent/integrators/DOPRI45.h"

#include <cmath>

using namespace asc;

void DOPRI45::propagate()
{
   switch (kpass)
   {
   case 0:
      x0 = x;
      xd0 = xd;
      x = x0 + dt * (1.0 / 5.0 * xd0);
      break;
   case 1:
      xd1 = xd;
      x = x0 + dt * (3.0 / 40.0 * xd0 + 9.0 / 40.0 * xd1);
      break;
   case 2:
      xd2 = xd;
      x = x0 + dt * (44.0 / 45.0 * xd0 - 56.0 / 15.0 * xd1 + 32.0 / 9.0 * xd2);
      break;
   case 3:
      xd3 = xd;
      x = x0 + dt * (19372.0 / 6561.0 * xd0 - 25360.0 / 2187.0 * xd1 + 64448.0 / 6561.0 * xd2 - 212.0 / 729.0 * xd3);
      break;
   case 4:
      xd4 = xd;
      x = x0 + dt * (9017.0 / 3168.0 * xd0 - 355.0 / 33.0 * xd1 + 46732.0 / 5247.0 * xd2 + 49.0 / 176.0 * xd3 - 5103.0 / 18656.0 * xd4);
      break;
   case 5:
      xd5 = xd;
      x = x0 + dt * (35.0 / 384.0 * xd0 + 500.0 / 1113.0 * xd2 + 125.0 / 192.0 * xd3 - 2187.0 / 6784.0 * xd4 + 11.0 / 84.0 * xd5); // 5th Order
      break;
   }
}

void DOPRI45::updateClock()
{
   if (0 == kpass)
   {
      t0 = t;
      t = t0 + 1.0 / 5.0 * dt;
   }
   else if (1 == kpass)
      t = t0 + 3.0 / 10.0 * dt;
   else if (2 == kpass)
      t = t0 + 4.0 / 5.0 * dt;
   else if (3 == kpass)
      t = t0 + 8.0 / 9.0 * dt;
   else if (4 == kpass)
      t = t1;
   // kpass of 5 is also t = t1

   integrator_initialized = true;

   ++kpass;
   kpass = kpass % 6;
   if (kpass == 0)
      t1 = floor((t + EPS) / dtp + 1) * dtp;
}

double DOPRI45::optimalTimeStep()
{
   double s = -1.0; // optimal time interval, return a negative value if a computation cannot be performed because of a lack of error

   if (tolerance > 0.0)
   {
      // After the next update() call we have the next derivative to compute the 4th order solution and thus an error.
      // However, this optimalTimeStep() call needs to happen between update() and propagate(), unlike the DOPRI87 method.
      double x4th = x0 + dt * (5179.0 / 57600.0 * xd0 + 7571.0 / 16695.0 * xd2 + 393.0 / 640.0 * xd3 - 92097.0 / 339200.0 * xd4 + 187.0 / 2100.0 * xd5 + 1.0 / 40.0 * xd);
      double error = std::abs(x4th - x);
      if (error > 0.0)
         s = 0.9 * tolerance / error;
      else
         s = 2.0;

      //s = pow((tolerance*dt / (2.0*error)), (1.0 / 5.0)); // optimal time interval
   }

   return s*dt;
}
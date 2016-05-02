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

#include "ascent/integrators/RTAM4.h"

using namespace asc;
using namespace std;

void RTAM4::propagate()
{
   if (!integrator_initialized)
   {
      if (0 == kpass && 0 == init_step)
         xd_1 = xd;
      else if (0 == kpass && 1 == init_step)
      {
         xd_2 = xd_1;
         xd_1 = xd;
      }
      else if (0 == kpass && 2 == init_step)
      {
         xd_3 = xd_2;
         xd_2 = xd_1;
         xd_1 = xd;
      }

      initializer->propagate();
   }
   else
   {
      switch (kpass)
      {
      case 0:
         x0 = x;
         xd0 = xd;
         x = x0 + dt / 384.0 * (297.0*xd - 187.0*xd_1 + 107.0*xd_2 - 25.0*xd_3); // X(n + 1/2), half step computation
         break;
      case 1:
         x = x0 + dt / 30.0 * (36.0*xd - 10.0*xd0 + 5.0*xd_1 - xd_2);
         xd_3 = xd_2;
         xd_2 = xd_1;
         xd_1 = xd0;
         break;
      }
   }
}

void RTAM4::updateClock()
{
   // Called once per integration pass

   if (!integrator_initialized)
   {
      initializer->updateClock();

      if (0 == kpass)
         ++init_step;

      if (3 == init_step)
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
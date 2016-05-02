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

#include "ascent/integrators/RKMM.h"

using namespace asc;

void RKMM::propagate()
{
   switch (kpass)
   {
   case 0:
      x0 = x;
      k1 = dt * xd;
      x = x0 + 1.0 / 3.0 * k1;
      break;
   case 1:
      k2 = dt * xd;
      x = x0 + 1.0 / 6.0 * k1 + 1.0 / 6.0 * k2;
      break;
   case 2:
      k3 = dt * xd;
      x = x0 + 1.0 / 8.0 * k1 + 3.0 / 8.0 * k3;
      break;
   case 3:
      k4 = dt * xd;
      x = x0 + 1.0 / 2.0 * k1 - 3.0 / 2.0 * k3 + 2.0 * k4;
      break;
   case 4:
      k5 = dt * xd;
      x = x0 + 1.0 / 6.0 * (k1 + 4.0 * k4 + k5);
      break;
   }
}

void RKMM::updateClock()
{
   if (kpass == 0)
      t += dt / 3;
   else if (kpass == 2)
      t += 1.0 / 6.0 * dt;
   else if (kpass == 3)
      t = t1;

   kpass++;
   kpass = kpass % 5;
   if (kpass == 0)
      t1 = floor((t + EPS) / dtp + 1) * dtp;
}
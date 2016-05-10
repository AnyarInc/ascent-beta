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

#include "ascent/integrators/DOPRI87.h"

#include <cmath>

using namespace asc;
using namespace std;

void DOPRI87::propagate()
{
   switch (kpass)
   {
   case 0:
      x0 = x;
      xd0 = xd;
      x = x0 + dt / 18.0 * xd0;
      break;
   case 1:
      xd1 = xd;
      x = x0 + dt * (1.0 / 48.0 * xd0 + 1.0 / 16.0 * xd1);
      break;
   case 2:
      xd2 = xd;
      x = x0 + dt * (1.0 / 32.0 * xd0 + 3.0 / 32.0 * xd2);
      break;
   case 3:
      xd3 = xd;
      x = x0 + dt * (5.0 / 16.0 * xd0 - 75.0 / 64.0 * xd2 + 75.0 / 64.0 * xd3);
      break;
   case 4:
      xd4 = xd;
      x = x0 + dt * (3.0 / 80.0 * xd0 + 3.0 / 16.0 * xd3 + 3.0 / 20.0 * xd4);
      break;
   case 5:
      xd5 = xd;
      x = x0 + dt * (29443841.0 / 614563906.0 * xd0 + 77736538.0 / 692538347.0 * xd3 - 28693883.0 / 1125000000.0 * xd4 + 23124283.0 / 1800000000.0 * xd5);
      break;
   case 6:
      xd6 = xd;
      x = x0 + dt * (16016141.0 / 946692911.0 * xd0 + 61564180.0 / 158732637.0 * xd3 + 22789713.0 / 633445777.0 * xd4 + 545815736.0 / 2771057229.0 * xd5 - 180193667.0 / 1043307555.0 * xd6);
      break;
   case 7:
      xd7 = xd;
      x = x0 + dt * (39632708.0 / 573591083.0 * xd0 - 433636366.0 / 683701615.0 * xd3 - 421739975.0 / 2616292301.0 * xd4 + 100302831.0 / 723423059.0 * xd5 + 790204164.0 / 839813087.0 * xd6 + 800635310.0 / 3783071287.0 * xd7);
      break;
   case 8:
      xd8 = xd;
      x = x0 + dt * (246121993.0 / 1340847787.0 * xd0 - 37695042795.0 / 15268766246.0 * xd3 - 309121744.0 / 1061227803.0 * xd4 - 12992083.0 / 490766935.0 * xd5 + 6005943493.0 / 2108947869.0 * xd6 + 393006217.0 / 1396673457.0 * xd7 + 123872331.0 / 1001029789.0 * xd8);
      break;
   case 9:
      xd9 = xd;
      x = x0 + dt * (-1028468189.0 / 846180014.0 * xd0 + 8478235783.0 / 508512852.0 * xd3 + 1311729495.0 / 1432422823.0 * xd4 - 10304129995.0 / 1701304382.0 * xd5 - 48777925059.0 / 3047939560.0 * xd6 + 15336726248.0 / 1032824649.0 * xd7 - 45442868181.0 / 3398467696.0 * xd8 + 3065993473.0 / 597172653.0 * xd9);
      break;
   case 10:
      xd10 = xd;
      x = x0 + dt * (185892177.0 / 718116043.0 * xd0 - 3185094517.0 / 667107341.0 * xd3 - 477755414.0 / 1098053517.0 * xd4 - 703635378.0 / 230739211.0 * xd5 + 5731566787.0 / 1027545527.0 * xd6 + 5232866602.0 / 850066563.0 * xd7 - 4093664535.0 / 808688257.0 * xd8 + 3962137247.0 / 1805957418.0 * xd9 + 65686358.0 / 487910083.0 * xd10);
      break;
   case 11:
      xd11 = xd;
      x = x0 + dt * (403863854.0 / 491063109.0 * xd0 - 5068492393.0 / 434740067.0 * xd3 - 411421997.0 / 543043805.0 * xd4 + 652783627.0 / 914296604.0 * xd5 + 11173962825.0 / 925320556.0 * xd6 - 13158990841.0 / 6184727034.0 * xd7 + 3936647629.0 / 1978049680.0 * xd8 - 160528059.0 / 685178525.0 * xd9 + 248638103.0 / 1413531060.0 * xd10);
      break;
   case 12:
      // 8th order:
      x = x0 + dt * (14005451.0 / 335480064.0 * xd0 - 59238493.0 / 1068277825.0 * xd5 + 181606767.0 / 758867731.0 * xd6 + 561292985.0 / 797845732.0 * xd7 - 1041891430.0 / 1371343529.0 * xd8 + 760417239.0 / 1151165299.0 * xd9 + 118820643.0 / 751138087.0 * xd10 - 528747749.0 / 2220607170.0 * xd11 + 1.0 / 4.0 * xd);
      break;
   }
}

void DOPRI87::updateClock()
{
   if (0 == kpass)
   {
      t0 = t;
      t = t0 + 1.0 / 18.0 * dt;
   }
   else if (1 == kpass)
      t = t0 + 1.0 / 12.0 * dt;
   else if (2 == kpass)
      t = t0 + 1.0 / 8.0 * dt;
   else if (3 == kpass)
      t = t0 + 5.0 / 16.0 * dt;
   else if (4 == kpass)
      t = t0 + 3.0 / 8.0 * dt;
   else if (5 == kpass)
      t = t0 + 59.0 / 400.0 * dt;
   else if (6 == kpass)
      t = t0 + 93.0 / 200.0 * dt;
   else if (7 == kpass)
      t = t0 + 5490023248.0 / 9719169821.0 * dt;
   else if (8 == kpass)
      t = t0 + 13.0 / 20.0 * dt;
   else if (9 == kpass)
      t = t0 + 1201146811.0 / 1299019798.0 * dt;
   else if (10 == kpass)
      t = t1;
   // t doesn't change for kpass > 10

   ++kpass;
   kpass = kpass % 13;

   if (kpass == 0)
      t1 = floor((t + EPS) / dtp + 1) * dtp;
}

double DOPRI87::optimalTimeStep()
{
   double s = -1.0; // optimal time interval, return a negative value if a computation cannot be performed because of a lack of error

   if (tolerance > 0.0)
   {
      // 7th order:
      double x7th = x0 + dt * (13451932.0 / 455176623.0 * xd0 - 808719846.0 / 976000145.0 * xd5 + 1757004468.0 / 5645159321.0 * xd6 + 656045339.0 / 265891186.0 * xd7 - 3867574721.0 / 1518517206.0 * xd8 + 465885868.0 / 322736535.0 * xd9 + 53011238.0 / 667516719.0 * xd10 + 2.0 / 45.0 *xd11);
      double error = abs(x - x7th);
      double temp = 1.25*pow((error / tolerance), (1.0 / 8.0));
      if (temp > 0.5)
         s = 1.0 / temp;
      else
         s = 2.0; // maximum stepsize increase
   }

   return s*dt;
}
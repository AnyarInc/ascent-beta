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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

namespace asc
{
   namespace Interpolation
   {
      template <typename T>
      inline double linear(const double x_target, const std::vector<T>& x, const std::vector<T>& y) // linear interpolation for doubles
      {
         size_t high = std::lower_bound(x.begin(), x.end(), x_target) - x.begin(); // std::lower_bound(...) returns iterator to first value in x that is not less than x_target

         if (high == 0)
            return y.front();

         size_t low = high - 1;

         double y0 = y[low];
         double y1 = y[high];

         double x0 = x[low];
         double x1 = x[high];

         return y0 + (y1 - y0)*(x_target - x0) / (x1 - x0); // linear interpolation (returns y_estimate)
      }

      template <typename T>
      inline double linearCheck(const double x_target, const std::vector<T>& x, const std::vector<T>& y) // includes a check for out of bounds
      {
         if (x_target < x.front())
            std::cerr << "ERROR: The access value (x_target) is less than the lowest x value." << '\n';
         else if (x_target > x.back())
            std::cerr << "ERROR: The access value (x_target) is greater than the highest x value." << '\n';
         else
            return linear(x_target, x, y);

         return std::numeric_limits<double>::quiet_NaN();
      }

      // closestNeighbor is for values such as integers that shouldn't be interpolated, but whose value closest to the desired (target) x value should be returned.
      template <typename T>
      inline T closestNeighbor(const double x_target, const std::vector<double>& x, const std::vector<T>& y)
      {
         size_t high = std::upper_bound(x.begin(), x.end(), x_target) - x.begin(); // get indice to first value greater than x_target

         if (high == 0)
            return y[high];
         else
         {
            double diff_high = fabs(x_target - x[high]);
            size_t low = high - 1;
            double diff_low = fabs(x_target - x[low]);

            if (diff_high < diff_low)
               return y[high];
            else
               return y[low];
         }
      }
   }
}
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

#include <iostream>

namespace asc
{
   namespace Integral
   {
      // x is the independant variable, y is dependent. Returns the integral estimate.
      template <typename T1, typename T2>
      inline double trapUnequal(const T1 &x, const T2 &y)
      {
         // Uses the composite trapezoidal rule to calculate the integral for unequally spaced data.
         if (x.size() != y.size())
         {
            std::cout << "ERROR: Vectors to be integrated with trapUnequal are of differing size." << '\n';
            return 0.0;
         }

         if (x.size() < 2)
            return 0.0;

         size_t n = x.size();
         size_t end = n - 1; // must end one step short because we are looking ahead one step in the loop
         double I = 0.0;

         for (size_t i = 0; i < end; ++i)
            I += (x[i + 1] - x[i]) * (y[i + 1] + y[i]) / 2.0;

         return I;
      }
   }
};
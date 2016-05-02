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

#include <deque>

// Algorithms are designed to handle std::vector and std::deque (for shifting transient problems), should also handle std::array.

// Currently divide by zero is allowed during derivative computations, because these are invalid derivatives that must be handled by the user.
// The handling of infinite solutions is specific to the given problem.

namespace asc
{
   namespace Derivative
   {
      // Generic derivative that takes a function, from boost documentation. For equally spaced points.
      template<typename T, typename function_type>
      inline T derivativeEqual(const T x, const T dx, function_type func)
      {
         // Compute d/dx[func(*first)] using a three-point central difference rule of O(dx^6).

         const T dx1 = dx;
         const T dx2 = dx1 * 2;
         const T dx3 = dx1 * 3;

         const T m1 = (func(x + dx1) - func(x - dx1)) / 2;
         const T m2 = (func(x + dx2) - func(x - dx2)) / 4;
         const T m3 = (func(x + dx3) - func(x - dx3)) / 6;

         const T fifteen_m1 = 15 * m1;
         const T six_m2 = 6 * m2;
         const T ten_dx1 = 10 * dx1;

         return ((fifteen_m1 - six_m2) + m3) / ten_dx1;
      }

      // x is the independant variable, y is dependent. Returns derivative for equally spaced points.
      template <typename T1, typename T2>
      inline double derivativeEqual(const T1 &x, const T2 &y)
      {
         size_t n = x.size();
         if (x.size() < 2)
            return 0.0;
         else if (n == 2)
            return (y.back() - y.front()) / (x.back() - x.front());
         else
         {
            double h = x[n - 1] - x[n - 2];

            double ylast = y[n - 1];
            double ymid = y[n - 2];
            double yfirst = y[n - 3];

            double dydx = (-ylast + 4.0*ymid - 3.0*yfirst) / (2.0*h);

            return dydx;
         }
      }

      // Supports std::vector and std::deque
      // x is the independant variable, y is dependent. Returns derivative for unequally spaced points. xest is the x value at which to evaluate the derivative.
      // x and y vectors don't need to be the same length if they have three elements or more each
      template <typename T1, typename T2>
      inline double derivative(const T1 &x, const T2 &y, const double xest)
      {
         size_t nx = x.size();
         size_t ny = y.size();
         if ((x.size() < 2) || (y.size() < 2))
            return 0.0;
         else if ((nx == 2) || (ny == 2))
            return (y.back() - y.front()) / (x.back() - x.front());
         else
         {
            double x2 = x[nx - 1];
            double x1 = x[nx - 2];
            double x0 = x[nx - 3];

            double y2 = y[ny - 1];
            double y1 = y[ny - 2];
            double y0 = y[ny - 3];

            double dydx = y0*(2.0 * xest - x1 - x2) / ((x0 - x1)*(x0 - x2)) + y1*(2.0 * xest - x0 - x2) / ((x1 - x0)*(x1 - x2)) + y2*(2.0 * xest - x0 - x1) / ((x2 - x0)*(x2 - x1));

            return dydx;
         }
      }

      // x is the independant variable, y is dependent. Returns derivative for unequally spaced points.
      // Uses the last x value for the prediction point.
      template <typename T1, typename T2>
      inline double derivative(const T1 &x, const T2 &y)
      {
         if (x.size() < 2)
            return 0.0;
         return derivative(x, y, x.back());
      }

      // E is intended to be an Eigen::Vector, such as Eigen::Vector3d, supports n dimensional Eigen C++ vectors
      // T1 and T2 can be std::vector or std::deque
      template <typename E, typename T1, typename T2>
      inline E vecDerivative(const T1 &t, const T2 &v)
      {
         size_t n = t.size();
         if (n < 2)
            return E::Zero();
         else if (n == 2)
            return (v[1] - v[0]) / (t[1] - t[0]);

         std::vector<std::vector<double>> dimensional_history; // each vector is for a dimension, such as x, y, z, . . . (can have more dimensions than 3), saving three time steps of history
         size_t dimensions = v[0].rows();
         dimensional_history.resize(dimensions);
         for (size_t i = 0; i < dimensions; ++i)
         {
            for (size_t j = 3; j > 0; --j) // iterate over last three states, from oldest to newest
               dimensional_history[i].push_back(v[n - j][i]); // this will push back the ith states (i.e. if i = 0, x dimension) from the last three Eigen::Vector states
         }

         E deriv;
         for (size_t i = 0; i < dimensions; ++i)
            deriv[i] = derivative(t, dimensional_history[i]); // t can contain more than 3 steps

         return deriv;
      }
   };
}
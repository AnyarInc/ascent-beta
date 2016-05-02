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
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace asc
{
   namespace Extrapolation
   {
      // x is the independant variable, y is dependent.
      // The dependent variable (y) is extrapolated at xest. y(xest) = solution
      // xest is the independent estimation point for the extrapolation.
      // The relative_error is updated by the extrapolate() function and provides the relative error of the curve fitting attempt.
      // func is the extrapolation function used (f(x)), should take a double (x) and return an Eigen::Vector
      template <typename Tx, typename Ty, typename Function>
      inline double extrapolate(const Tx &x, const Ty &y, double xest, double& relative_error, Function func)
      {
         size_t n = y.size();

         if (n < 2) // if there are less than two value then there is no use extrapolating
         {
            relative_error = 1.0;
            return 0.0;
         }

         // get shift for x elements so that the first value starts at zero, so that the extrapolation function uses its behavior starting from x = 0.0
         double x_shift = x.front();
         xest -= x_shift; // shift x estimate point (xest)

         Eigen::VectorXd xest_vec = func(xest); // used at end, and computed here to determine the number of columns in the A matrix (number of extrapolation function terms)
         size_t cols = xest_vec.size();

         // A matrix built from: y = f(x), e.g. y = A + B*x + C*x*x
         Eigen::MatrixXd A = Eigen::MatrixXd(n, cols);
         size_t i = 0;
         for (auto& value : x)
            A.row(i++) = func(value - x_shift);

         i = 0;
         Eigen::VectorXd b(n);
         for (auto& value : y)
            b(i++) = value;

         Eigen::VectorXd constants = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b); // constants A, B, C, etc. (thin U and thin V are all that is needed for least squares solving)

         if (b.norm() < 1.0e-8) // handle case where b is all zeros
         {
            relative_error = 0.0;
            return 0.0;
         }
         else
            relative_error = (A*constants - b).norm() / b.norm();

         return constants.dot(xest_vec); // example for fitting vector = 1.0 + x + x^2: return constants[0] * 1.0 + constants[1] * xest + constants[2] * xest*xest;
      }
   };
}
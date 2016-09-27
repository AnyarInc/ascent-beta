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
   namespace Fit2D
   {
      // This algorithm can almost replace the internals of the extrapolation algorithm (Extrapolation.h), but it adds an unneeded computation (for extrapolation) of func(1.0) to determine the number of columns.
      // It also doesn't have a quick return if the y vector size is less than 2, because what should be returned for the constants vector?
      // x is the independant variable, y is dependent.
      // Fits a function of the form: y = f(x), e.g. y = A + B*x + C*x*x
      // Returns a vector of linearly fit constants.
      template <typename Tx, typename Ty, typename Function>
      inline Eigen::VectorXd fit2D(const Tx &x, const Ty &y, double& relative_error, Function func)
      {
         size_t n = y.size();
         size_t nx = x.size();

         // get shift for x elements so that the first value starts at zero, so that the fitting function uses its behavior starting from x = 0.0
         double x_shift = x.front();

         Eigen::VectorXd x_test = func(1.0); // a computation at x = 1.0 is performed to determine the number of columns in the fitting function (1.0 is used rather than 0.0 to avoid dividing by zero)
         size_t cols = x_test.size();

         // A matrix built from: y = f(x), e.g. y = A + B*x + C*x*x
         Eigen::MatrixXd A = Eigen::MatrixXd(n, cols);
         for (size_t i = 0; i < nx; ++i)
            A.row(i) = func(x[i] - x_shift);

         Eigen::VectorXd b(n);
         for (size_t i = 0; i < n; ++i)
            b[i] = y[i];

         Eigen::VectorXd constants = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b); // constants A, B, C, etc. (thin U and thin V are all that is needed for least squares solving)

         if (b.norm() < 1.0e-8) // handle case where b is all zeros
            relative_error = 0.0;
         else
            relative_error = (A*constants - b).norm() / b.norm();

         return constants;
      }
   };
}
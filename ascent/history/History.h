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

// Functionality can be added to History by inheriting from this class and using handling protected x and th parameters.

#include "ascent/algorithms/Fit2D.h"
#include "ascent/algorithms/Derivative.h"
#include "ascent/algorithms/Extrapolation.h"
#include "ascent/algorithms/Integral.h"
#include "ascent/algorithms/Statistics.h"

#include <memory>

namespace asc
{
   class Simulator;

   class History
   {
   private:
      Simulator& simulator;
      void insert(const double value);

   protected:
      // using std::deque because erasing first element is far faster than std::vector
      std::deque<double> th; // time history
      std::deque<double> x; // parameter history

      Eigen::Vector3d parabolic(double x) const { return Eigen::Vector3d(1.0, x, x*x); }

   public:
      size_t steps; // The number of history steps to keep track of, derivatives only use the three most recent points at most.
      bool infinite; // Whether History should store data indefinitely.

      History(const size_t sim, const size_t steps);
      History(const size_t sim); // Constructor for recording steps indefinitely.

      void error(const std::string& description);

      void push_back(const double value);

      const double& t, dt;

      size_t size() const { return x.size(); }
      double& back() { return x.back(); }
      double& front() { return x.front(); }

      const std::deque<double>& history() const { return x; }
      const std::deque<double>& time() const { return th; }

      // Algorithms:
      // CurveFit:
      template <typename Function>
      Eigen::VectorXd fit2D(double& relative_error, Function func) const { Fit2D::fit2D(th, x, relative_error, func); }
      
      // Derivative:
      double derivative() const { return Derivative::derivative(th, x); }

      // Extrapolation:
      template <typename Function>
      double extrapolate(const double xest, double& relative_error, Function func) const { return Extrapolation::extrapolate(th, x, xest, relative_error, func); }

      double extrapParabolic(const double xest, double& relative_error) const // Extrapolate using the function: x(t) = A + B*t + C*t*t
      {
         auto func = [&](double x) { return parabolic(x); };
         return Extrapolation::extrapolate(th, x, xest, relative_error, func);
      }

      // Integral::
      double integral() const { return Integral::trapUnequal(th, x); }

      // Statistics:
      double mean() const { return Statistics::mean(x); }
      double stdDeviation() const { return Statistics::stdDeviation(x); }
   };
}
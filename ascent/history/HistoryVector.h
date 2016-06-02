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

// HistoryVector is meant to store time history of Eigen Vectors (e.g. Eigen::Vector3d, from the open source Eigen Matrix Library).
// This class also provides a number of algorithms that operate in all dimensions across the vector's history.

#include "ascent/Module.h"

#include "ascent/algorithms/Derivative.h"
#include "ascent/algorithms/StatisticsVector.h"

namespace asc
{
   // Type E is intended to be an Eigen::Vector
   template <typename E>
   class HistoryVector
   {
   private:
      Simulator& simulator;

   protected:
      // using std::deque because erasing first element is far faster than std::vector
      std::deque<double> th; // time history
      std::deque<E> x; // parameter history

   public:
      size_t steps; // The number of history steps to keep track of, derivatives only use the three most recent points at most.

      HistoryVector(const size_t sim, const size_t steps = 3) : simulator(Module::getSimulator(sim)), steps(steps), t(simulator.t), dt(simulator.dt) {}

      void error(const std::string& description) { simulator.setError(description); }

      void push_back(const E& value)
      {
         if (steps > 0)
         {
            if (x.size() >= steps) // allows number of steps to be lengthened or reduced during runtime
            {
               th.erase(th.begin());
               x.erase(x.begin());
            }

            if (th.size() > 0)
            {
               if (simulator.t <= th.back())
                  simulator.setError("Attempted push_back on History when the current time is less than or equal to the last time recorded.");
            }

            th.push_back(simulator.t);
            x.push_back(value);
         }
      }

      const double& t;
      const double& dt;

      size_t size() const { return x.size(); }
      E& back() { return x.back(); }
      E& front() { return x.front(); }

      const std::deque<E>& history() const { return x; }
      const std::deque<double>& time() const { return th; }
      std::deque<double> time(const size_t steps) const
      {
         std::deque<double> deq(th.end() - steps, th.end());
         return deq;
      }

      Eigen::VectorXd col(const size_t dim, const size_t steps) const // get time history of a specific dimension
      {
         size_t n = x.size();
         if (n > 1)
         {
            Eigen::MatrixXd data(steps, x.front().size());
            for (size_t i = 1; i <= steps; ++i)
               data.row(steps - i) = x[n - i];

            return data.col(dim);
         }
         else
         {
            Eigen::VectorXd column(1);
            column << x.back()[dim];
            return column;
         }
      }

      // Algorithms:
      // Derivative:
      E derivative() const { return Derivative::vecDerivative<E>(th, x); }

      // Statistics:
      E mean() const { return StatisticsVector::mean(x); }
      E mean(const size_t steps) const { return StatisticsVector::mean(x, steps); } // Returns mean for a given number of steps back in history.

      E stdDeviation() const { return StatisticsVector::stdDeviation(x); }
      E stdDeviation(const size_t steps) const { return StatisticsVector::stdDeviation(x, steps); }
   };
}
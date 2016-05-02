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

#include "ascent/algorithms/Statistics.h"

namespace asc
{
   namespace StatisticsVector
   {
      // Supports std::vectors or std::deques of Eigen::Vectors
      template <typename T>
      auto mean(const T &v) -> typename std::decay<decltype(v.front())>::type
      {
         return mean(v, v.size());
      }

      // Supports std::vectors or std::deques of Eigen::Vectors
      template <typename T>
      auto mean(const T &v, const size_t steps) -> typename std::decay<decltype(v.front())>::type
      {
         size_t n = v.size();
         if (n > 1)
         {
            Eigen::MatrixXd data(steps, v.front().size());
            for (size_t i = 1; i <= steps; ++i)
               data.row(steps - i) = v[n - i];

            return Eigen::VectorXd(data.colwise().mean());
         }
         else
            return v.front();
      }

      // Supports std::vectors or std::deques of Eigen::Vectors
      template <typename T>
      auto stdDeviation(const T &v) -> typename std::decay<decltype(v.front())>::type
      {
         return stdDeviation(v, v.size());
      }

      // Supports std::vectors or std::deques of Eigen::Vectors
      template <typename T>
      auto stdDeviation(const T &v, const size_t steps) -> typename std::decay<decltype(v.front())>::type
      {
         size_t n = v.size();
         if (n > 1)
         {
            Eigen::MatrixXd data(steps, v.front().size());
            for (size_t i = 1; i <= steps; ++i)
               data.row(steps - i) = v[n - i];

            std::vector<std::vector<double>> columns;
            for (int c = 0; c < data.cols(); ++c)
            {
               columns.push_back(std::vector<double>());
               for (int r = 0; r < data.rows(); ++r)
                  columns[c].push_back(data(r, c));
            }

            std::vector<double> sigmas;
            for (auto& column : columns)
               sigmas.push_back(Statistics::stdDeviation(column));

            Eigen::VectorXd result(sigmas.size());
            for (size_t i = 0; i < sigmas.size(); ++i)
               result(i) = sigmas[i];

            return result;
         }
         else
            return v.front();
      }
   }
}
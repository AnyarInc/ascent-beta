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

#include "Parameter.h"

#include <Eigen/Dense>

namespace asc
{
   class ToString
   {
   public:
      ToString();

      template <typename T>
      static std::string print(T& x)
      {
         if (print_map.count(typeid(T)))
            return print_map[typeid(T)](&x);

         // If the type hasn't been registered, then register it:
         registerType(x);

         return print_map[typeid(T)](&x);
      }

   private:
      static std::map<std::type_index, std::function<std::string(void* x)>> print_map; // a map of to_string functions for registered types

      template <typename T>
      static void registerType(T& value)
      {
         print_map[typeid(T)] = [&](void* x) { return std::to_string(*static_cast<T*>(x)); };
      }

      template <typename T>
      static void registerType(std::vector<T>& value) // value isn't needed, but it is used to distinguish between registerType functions (could use std::enable_if instead)
      {
         print_map[typeid(std::vector<T>)] = [&](void* x) {
            std::vector<T>& vec = *static_cast<std::vector<T>*>(x);

            std::string output = "";

            size_t n = vec.size();
            for (size_t i = 0; i < n; ++i)
            {
               output += std::to_string(vec[i]);
               if (i < n - 1) // not the last parameter
                  output += ", ";
            }

            return output;
         };
      }

      static void registerType(std::string& value)
      {
         print_map[typeid(std::string)] = [&](void* x) { return *static_cast<std::string*>(x); };
      }


      // For Eigen vectors of any length
      template <typename T>
      static void registerEigen()
      {
         print_map[typeid(T)] = [&](void* x) {
            T matrix = *static_cast<T*>(x);

            std::string output = "";

            size_t n = matrix.array().size();
            for (size_t i = 0; i < n; ++i)
            {
               output += std::to_string(matrix.array()(i));
               if (i < n - 1) // not the last parameter
                  output += ", ";
            }

            return output;
         };
      }

      template <typename T, int rows>
      static void registerType(Eigen::Matrix<T, rows, 1, 0, rows, 1>& value)
      {
         registerEigen<Eigen::Matrix<T, rows, 1, 0, rows, 1>>();
      }

      // For dynamic Eigen vectors
      static void registerType(Eigen::VectorXd& value)
      {
         registerEigen<Eigen::VectorXd>();
      }

      static void registerType(Eigen::Matrix2d& value) { registerEigen<Eigen::Matrix2d>(); }
      static void registerType(Eigen::Matrix3d& value) { registerEigen<Eigen::Matrix3d>(); }
      static void registerType(Eigen::Matrix4d& value) { registerEigen<Eigen::Matrix4d>(); }
      static void registerType(Eigen::Matrix<double, 6, 6>& value) { registerEigen<Eigen::Matrix<double, 6, 6>>(); }
      static void registerType(Eigen::Matrix<double, 9, 9>& value) { registerEigen<Eigen::Matrix<double, 9, 9>>(); }
      static void registerType(Eigen::MatrixXd& value) { registerEigen<Eigen::MatrixXd>(); }
   };
}
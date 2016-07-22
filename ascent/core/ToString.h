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
   namespace ToString
   {
      std::map<std::type_index, std::function<std::string(void* x)>>& printMap();

      template <typename T>
      inline typename std::enable_if<std::is_arithmetic<T>::value || std::is_integral<T>::value, void>::type registerType()
      {
         auto& print_map = printMap();
         print_map[typeid(T)] = [&](void* x) { return std::to_string(*static_cast<T*>(x)); };
      }

      template <typename T>
      inline typename std::enable_if<std::is_same<T, std::string>::value, void>::type registerType()
      {
         auto& print_map = printMap();
         print_map[typeid(T)] = [&](void* x) { return *static_cast<T*>(x); };
      }

      template <typename T>
      inline void registerVectorType()
      {
         auto& print_map = printMap();

         print_map[typeid(T)] = [&](void* x) {
            T& vec = *static_cast<T*>(x);

            std::string output = "";

            size_t n = vec.size();
            for (size_t i = 0; i < n; ++i)
            {
               output += std::to_string(vec[i]);
               if (i < n - 1) // not the last parameter
                  output += ",";
            }

            return output;
         };
      }

      template <typename T> inline typename std::enable_if<std::is_same<T, std::vector<bool>>::value, void>::type registerType() { registerVectorType<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, std::vector<size_t>>::value, void>::type registerType() { registerVectorType<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, std::vector<double>>::value, void>::type registerType() { registerVectorType<T>(); }

      template <typename T> inline typename std::enable_if<std::is_same<T, std::deque<bool>>::value, void>::type registerType() { registerVectorType<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, std::deque<size_t>>::value, void>::type registerType() { registerVectorType<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, std::deque<double>>::value, void>::type registerType() { registerVectorType<T>(); }

      // For Eigen vectors and matrices of any length
      template <typename T>
      inline void registerEigen()
      {
         auto& print_map = printMap();

         print_map[typeid(T)] = [&](void* x) {
            T matrix = *static_cast<T*>(x);

            std::string output = "";

            size_t n = matrix.array().size();
            for (size_t i = 0; i < n; ++i)
            {
               output += std::to_string(matrix.array()(i));
               if (i < n - 1) // not the last parameter
                  output += ",";
            }

            return output;
         };
      }

      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Vector2d>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Vector3d>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Vector4d>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix<double, 5, 1, 0, 5, 1>>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix<double, 6, 1, 0, 6, 1>>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix<double, 7, 1, 0, 7, 1>>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix<double, 8, 1, 0, 8, 1>>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix<double, 9, 1, 0, 9, 1>>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::VectorXd>::value, void>::type registerType() { registerEigen<T>(); }

      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix2d>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix3d>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix4d>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix<double, 6, 6>>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::Matrix<double, 9, 9>>::value, void>::type registerType() { registerEigen<T>(); }
      template <typename T> inline typename std::enable_if<std::is_same<T, Eigen::MatrixXd>::value, void>::type registerType() { registerEigen<T>(); }

      inline std::map<std::type_index, std::function<std::string(void* x)>>& printMap()
      {
         static std::map<std::type_index, std::function<std::string(void* x)>> print_map; // a map of to_string functions for registered types
         static bool initialized = false;
         if (!initialized)
         {
            initialized = true; // must be called immediately because the following functions will call printMap()

            registerType<bool>();
            registerType<int>();
            registerType<size_t>();

            registerType<float>();
            registerType<double>();

            registerType<std::string>();

            registerType<std::vector<bool>>();
            registerType<std::vector<size_t>>();
            registerType<std::vector<double>>();

            registerType<std::deque<bool>>();
            registerType<std::deque<size_t>>();
            registerType<std::deque<double>>();

            {
               using namespace Eigen;
               registerType<Vector2d>();
               registerType<Vector3d>();
               registerType<Vector4d>();

               registerType<Matrix<double, 5, 1, 0, 5, 1>>();
               registerType<Matrix<double, 6, 1, 0, 6, 1>>();
               registerType<Matrix<double, 7, 1, 0, 7, 1>>();
               registerType<Matrix<double, 8, 1, 0, 8, 1>>();
               registerType<Matrix<double, 9, 1, 0, 9, 1>>();
               registerType<VectorXd>();

               registerType<Matrix2d>();
               registerType<Matrix3d>();
               registerType<Matrix4d>();
               registerType<Matrix<double, 6, 6>>();
               registerType<Matrix<double, 9, 9>>();
               registerType<MatrixXd>();
            }
         }
         return print_map;
      }

      template <typename T>
      inline std::string print(T& x)
      {
         auto& print_map = printMap();

         if (print_map.count(typeid(T)))
            return print_map[typeid(T)](&x);

         return "Type not registered for printing: " + static_cast<std::string>(typeid(T).name());
      }
   }
}
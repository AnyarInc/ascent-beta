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

#include <map>
#include <string>
#include <typeindex>
#include <type_traits>

#include <Eigen/Dense>

#define ascType(type, identifier) ascTypeMap()[typeid(type)] = []() { return #identifier; };

inline auto& ascTypeMap()
{
   static std::map<std::type_index, std::function<std::string()>> asc_type_map;
   static bool initialized = false;
   if (!initialized)
   {
      {
         using namespace Eigen;
         asc_type_map[typeid(Vector2d)] = []() { return "Vector2d"; };
         asc_type_map[typeid(Vector3d)] = []() { return "Vector3d"; };
         asc_type_map[typeid(Vector4d)] = []() { return "Vector4d"; };
         asc_type_map[typeid(Matrix3d)] = []() { return "Matrix3d"; };
      }

      initialized = true;
   }
   return asc_type_map;
}

namespace asc
{
   template <typename T>
   struct Type 
   {
      static std::string name()
      {
         auto& type_map = ascTypeMap();

         std::type_index index = typeid(T);

         if (type_map.count(index))
            return type_map[index]();
         
         return "No type name set for " + static_cast<std::string>(index.name()) + ", use ascType() macro.";
      }
   };
}
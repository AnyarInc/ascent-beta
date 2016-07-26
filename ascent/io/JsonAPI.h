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

// JsonAPI allows module data access, assignment, and chaiscript calls via JSON script.
// The idea in input/output of variables is for the output to be used as input (and vice versa) so that Ascent simulations can directly assign data once accessed.

#define JSONCONS_NO_DEPRECATED // Don't allow deprecated function use.
#pragma warning(disable: 4996) // need to disable error for fopen used in json.hpp
#include "jsoncons/json.hpp"

#include "ascent/Link.h"
#include "ascent/algorithms/Interpolation.h"

namespace asc
{
   class JsonAPI
   {
   public:
      JsonAPI() {}

      static jsoncons::json jsonModules(); // Get all accessible modules.
      static jsoncons::json jsonVariables(); // Get all accessible variables.

      static jsoncons::json io(const std::string& input);
      static jsoncons::json io(jsoncons::json& input);
      static jsoncons::json ioFile(const std::string& filename); // filename should end with .json

   private:
      template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type>
      static double interpolate(const double x_target, const std::vector<T>& x, const std::deque<T>& y)
      {
         return Interpolation::linear(x_target, x, y);
      }

      template <typename T>
      static T interpolate(const double x_target, const std::vector<double>& x, const std::deque<T>& y)
      {
         return Interpolation::closestNeighbor(x_target, x, y);
      }

      static std::string interpolate(const double x_target, const std::vector<double>& x, const std::vector<std::string>& y) { return y.back(); }

      template <typename T>
      static bool access(jsoncons::json& obj, jsoncons::json& obj_out, Module& base)
      {
         std::string type = obj["type"].as<std::string>();
         std::string var = obj["var"].as<std::string>();

         bool success = false;
         if (typeid(T).name() == type)
         {
            success = true;
            if (obj.count("value")) // If a "value" member is specified, then we assume the user wants to set the variable.
               success = base.vars.set(var, obj["value"].as<T>());

            if (obj.count("t"))
            {
               const double t = obj["t"].as<double>();
               const auto& hist = base.history<T>(var);
               obj_out["value"] = interpolate(t, base.timeHistory(), hist);
            }

            obj_out["type"] = type;
            obj_out["value"] = base.vars.get<T>(var);
            obj_out["var"] = var;
         }

         return success;
      }

      template <typename First, typename Second, typename... Types>
      static bool access(jsoncons::json& obj, jsoncons::json& obj_out, Module& base)
      {
         if (access<First>(obj, obj_out, base))
            return true;

         return access<Second, Types...>(obj, obj_out, base);
      }
   };
}
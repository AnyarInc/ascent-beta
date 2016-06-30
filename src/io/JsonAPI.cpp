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

#include "ascent/io/JsonAPI.h"

#include "ascent/algorithms/Interpolation.h"
#include "ascent/core/ModuleCore.h"

#include <Eigen/Dense>

#include <cmath>
#include <limits>

using namespace asc;
using namespace std;

#define JSONCONS_NO_DEPRECATED // Don't allow deprecated function use.

jsoncons::json JsonAPI::jsonModules()
{
   jsoncons::json modules = jsoncons::json::array();

   for (auto& p : ModuleCore::external)
      modules.add(p.first);

   jsoncons::json output;
   output["modules"] = modules;

   return output;
}

jsoncons::json JsonAPI::jsonVariables()
{
   jsoncons::json output = jsoncons::json::array();

   for (auto& p : ModuleCore::external)
   {
      string module = p.first;
      const auto& names = p.second->vars.getNames();
      for (auto& var : names)
      {
         jsoncons::json obj;
         obj["module"] = module;
         obj["type"] = var.first;
         obj["var"] = var.second;
         output.add(std::move(obj));
      }
   }

   return output;
}

jsoncons::json JsonAPI::io(const std::string& input)
{
   jsoncons::json in;

   try
   {
      in = jsoncons::json::parse_file(input);
   }
   catch (const jsoncons::json_exception& e)
   {
      cerr << e.what() << '\n';
   }

   return io(in);
}

jsoncons::json JsonAPI::io(jsoncons::json& input)
{
   jsoncons::json output = jsoncons::json::array();

   for (size_t i = 0; i < input.size(); ++i) // for all modules
   {
      try
      {
         jsoncons::json& module = input[i];
         if (module.has_member("module")) // this object is a module
         {
            const string name = module["module"].as<string>();
            Module& base = ModuleCore::getExternal(name);

            jsoncons::json module_out;
            module_out["module"] = name;

            if (module.has_member("chaiscript"))
            {
               base.chai.eval(module["chaiscript"].as<string>());
            }
            else if (module.has_member("modules"))
            {
               // Want recursion for multiple modules deep.
               module_out["modules"] = io(module["modules"]);
            }
            else if (module.has_member("vars"))
            {
               jsoncons::json& variables = module["vars"]; // array of variables
               jsoncons::json variables_out = jsoncons::json::array();

               for (size_t v = 0; v < variables.size(); ++v) // for all variables
               {
                  jsoncons::json& variable = variables[v];
                  jsoncons::json variable_out;

                  if (variable.has_member("var"))
                  {
                     bool success = access<std::string, double, bool, int, unsigned, size_t, vector<double>, vector<bool>, vector<int>, vector<unsigned>, vector<size_t>>(variable, variable_out, base);

                     // IMPROVEMENT: Eigen handling should be inserted into access function, but this will take some thought. I'm not sure how it could be handled generically because Derived is unknown for Eigen::MatrixBase<Derived>.
                     // Important Eigen allignment note: In order to avoid allignment issues, static Eigen parameters are passed via json objects as dynamic Eigen vectors or matrices.
                     if (!success)
                     {
                        std::string type = variable["type"].as<string>();
                        std::string var = variable["var"].as<string>();

                        if (variable.has_member("value")) // If a "value" member is specified, then we assume the user wants to set the variable.
                        {
                           jsoncons::json& value = variable["value"];

                           if (typeid(Eigen::Vector2d).name() == type)
                              success = base.vars.set(var, static_cast<Eigen::Vector2d>(value.any_cast<Eigen::VectorXd>()));
                           else if (typeid(Eigen::Vector3d).name() == type)
                              success = base.vars.set(var, value.any_cast<Eigen::Vector3d>());
                           else if (typeid(Eigen::Vector4d).name() == type)
                              success = base.vars.set(var, static_cast<Eigen::Vector4d>(value.any_cast<Eigen::VectorXd>()));
                           else if (typeid(Eigen::VectorXd).name() == type)
                              success = base.vars.set(var, value.any_cast<Eigen::VectorXd>());

                           else if (typeid(Eigen::Matrix2d).name() == type)
                              success = base.vars.set(var, static_cast<Eigen::Matrix2d>(value.any_cast<Eigen::MatrixXd>()));
                           else if (typeid(Eigen::Matrix3d).name() == type)
                              success = base.vars.set(var, value.any_cast<Eigen::Matrix3d>());
                           else if (typeid(Eigen::Matrix4d).name() == type)
                              success = base.vars.set(var, static_cast<Eigen::Matrix4d>(value.any_cast<Eigen::MatrixXd>()));
                           else if (typeid(Eigen::MatrixXd).name() == type)
                              success = base.vars.set(var, value.any_cast<Eigen::MatrixXd>());
                        }

                        if (typeid(Eigen::Vector2d).name() == type)
                           variable_out.set("value", jsoncons::json::any(static_cast<Eigen::VectorXd>(base.vars.get<Eigen::Vector2d>(var))));
                        else if (typeid(Eigen::Vector3d).name() == type)
                           variable_out.set("value", jsoncons::json::any(base.vars.get<Eigen::Vector3d>(var)));
                        else if (typeid(Eigen::Vector4d).name() == type)
                           variable_out.set("value", jsoncons::json::any(static_cast<Eigen::VectorXd>(base.vars.get<Eigen::Vector4d>(var))));
                        else if (typeid(Eigen::VectorXd).name() == type)
                           variable_out.set("value", jsoncons::json::any(base.vars.get<Eigen::VectorXd>(var)));

                        else if (typeid(Eigen::Matrix2d).name() == type)
                           variable_out.set("value", jsoncons::json::any(static_cast<Eigen::MatrixXd>(base.vars.get<Eigen::Matrix2d>(var))));
                        else if (typeid(Eigen::Matrix3d).name() == type)
                           variable_out.set("value", jsoncons::json::any(base.vars.get<Eigen::Matrix3d>(var)));
                        else if (typeid(Eigen::Matrix4d).name() == type)
                           variable_out.set("value", jsoncons::json::any(static_cast<Eigen::MatrixXd>(base.vars.get<Eigen::Matrix4d>(var))));
                        else if (typeid(Eigen::MatrixXd).name() == type)
                           variable_out.set("value", jsoncons::json::any(base.vars.get<Eigen::MatrixXd>(var)));
                     }

                     variables_out.add(std::move(variable_out));
                  }
               }

               module_out["vars"] = variables_out;
            }

            output.add(std::move(module_out));
         }
      }
      catch (const jsoncons::json_exception& e)
      {
         cerr << e.what() << '\n';
      }
   }

   return output;
}

jsoncons::json JsonAPI::ioFile(const std::string& filename)
{
   jsoncons::json input;

   try
   {
      input = jsoncons::json::parse_file(filename);
   }
   catch (const jsoncons::json_exception& e)
   {
      cerr << e.what() << '\n';
   }

   return io(input);
}
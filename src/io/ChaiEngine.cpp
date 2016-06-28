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

#include "ascent/io/ChaiEngine.h"

#include <chaiscript/chaiscript_stdlib.hpp>

#include "ascent/core/LinkBase.h"
#include "ascent/Link.h"
#include "ascent/core/ModuleCore.h"

#include <Eigen/Dense>

using namespace asc;
using namespace chaiscript;

template <typename E>
void assign(E& lhs, const E& rhs) { lhs = rhs; }

template <typename E>
E cross(const E& lhs, const E& rhs) { return lhs.cross(rhs); }

template <typename E>
double dot(const E& lhs, const E& rhs) { return lhs.dot(rhs); }

template <typename LHS, typename E>
E timesL(const LHS& lhs, const E& rhs) { return lhs*rhs; }

template <typename E, typename RHS>
E timesR(const E& lhs, const RHS& rhs) { return lhs*rhs; }

ChaiEngine::ChaiEngine() : ChaiScript(Std_Lib::library())
{
   add(base_class<asc::LinkBase, asc::Link<asc::Module>>());
   add(fun(static_cast<std::shared_ptr<asc::Module> (asc::Link<asc::Module>::*)>(&asc::Link<asc::Module>::module)), "module");

   add(chaiscript::fun(static_cast<const size_t(asc::Module::*)>(&asc::Module::module_id)), "module_id");

   add(chaiscript::fun([](const size_t id) -> asc::Module& { return asc::ModuleCore::getModule(id); }), "getModule");

   add(chaiscript::fun(static_cast<bool(asc::Link<asc::Module>::*)(const std::string&)>(&asc::Link<asc::Module>::name)), "name");

   add(chaiscript::fun(&asc::hidden::assignModule), "=");
   add(chaiscript::fun(&asc::hidden::assignLinkBase), "=");

   // Register Eigen matrix/vector types
   add(constructor<Eigen::Vector2d(const double x0, const double x1)>(), "Vector2d");
   add(constructor<Eigen::Vector2d(const Eigen::Vector2d&)>(), "Vector2d");
   add(fun(&assign<Eigen::Vector2d>), "=");
   add(fun(&timesL<double, Eigen::Vector2d>), "*");
   add(fun(&timesR<Eigen::Vector2d, double>), "*");
   add(fun(&dot<Eigen::Vector2d>), "dot");

   add(constructor<Eigen::Vector3d(const double x0, const double x1, const double x2)>(), "Vector3d");
   add(constructor<Eigen::Vector3d(const Eigen::Vector3d&)>(), "Vector3d");
   add(fun(&assign<Eigen::Vector3d>), "=");
   add(fun(&timesL<double, Eigen::Vector3d>), "*");
   add(fun(&timesR<Eigen::Vector3d, double>), "*");
   add(fun(&dot<Eigen::Vector3d>), "dot");
   add(fun(&cross<Eigen::Vector3d>), "cross");

   add(constructor<Eigen::Vector4d(const double x0, const double x1, const double x2, const double x3)>(), "Vector4d");
   add(constructor<Eigen::Vector4d(const Eigen::Vector4d&)>(), "Vector4d");
   add(fun(&assign<Eigen::Vector4d>), "=");
   add(fun(&timesL<double, Eigen::Vector4d>), "*");
   add(fun(&timesR<Eigen::Vector4d, double>), "*");
   add(fun(&dot<Eigen::Vector4d>), "dot");

   add(fun(&assign<Eigen::VectorXd>), "=");
   add(fun(&timesL<double, Eigen::VectorXd>), "*");
   add(fun(&timesR<Eigen::VectorXd, double>), "*");
   add(fun(&dot<Eigen::VectorXd>), "dot");

   add(fun(&assign<Eigen::MatrixXd>), "=");
   add(fun(&timesL<double, Eigen::MatrixXd>), "*");
   add(fun(&timesL<Eigen::VectorXd, Eigen::MatrixXd>), "*");
   add(fun(&timesR<Eigen::MatrixXd, double>), "*");
   add(fun(&timesR<Eigen::MatrixXd, Eigen::VectorXd>), "*");
}

bool ChaiEngine::registered(const std::string& module, const std::string& var)
{
   if (!chai_rg.count(module))
      return false;

   std::vector<std::string>& names = chai_rg[module];

   for (auto& name : names)
   {
      if (name == var)
         return true;
   }
   
   return false;
}
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

#include <Eigen/Dense>

using namespace asc;

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

ChaiEngine::ChaiEngine() : chaiscript::ChaiScript(chaiscript::Std_Lib::library())
{
   // Register Eigen matrix/vector types
   add(chaiscript::constructor<Eigen::Vector2d(const double x0, const double x1)>(), "Vector2d");
   add(chaiscript::constructor<Eigen::Vector2d(const Eigen::Vector2d&)>(), "Vector2d");
   add(chaiscript::fun(&assign<Eigen::Vector2d>), "=");
   add(chaiscript::fun(&timesL<double, Eigen::Vector2d>), "*");
   add(chaiscript::fun(&timesR<Eigen::Vector2d, double>), "*");
   add(chaiscript::fun(&dot<Eigen::Vector2d>), "dot");

   add(chaiscript::constructor<Eigen::Vector3d(const double x0, const double x1, const double x2)>(), "Vector3d");
   add(chaiscript::constructor<Eigen::Vector3d(const Eigen::Vector3d&)>(), "Vector3d");
   add(chaiscript::fun(&assign<Eigen::Vector3d>), "=");
   add(chaiscript::fun(&timesL<double, Eigen::Vector3d>), "*");
   add(chaiscript::fun(&timesR<Eigen::Vector3d, double>), "*");
   add(chaiscript::fun(&dot<Eigen::Vector3d>), "dot");
   add(chaiscript::fun(&cross<Eigen::Vector3d>), "cross");

   add(chaiscript::constructor<Eigen::Vector4d(const double x0, const double x1, const double x2, const double x3)>(), "Vector4d");
   add(chaiscript::constructor<Eigen::Vector4d(const Eigen::Vector4d&)>(), "Vector4d");
   add(chaiscript::fun(&assign<Eigen::Vector4d>), "=");
   add(chaiscript::fun(&timesL<double, Eigen::Vector4d>), "*");
   add(chaiscript::fun(&timesR<Eigen::Vector4d, double>), "*");
   add(chaiscript::fun(&dot<Eigen::Vector4d>), "dot");

   add(chaiscript::fun(&assign<Eigen::VectorXd>), "=");
   add(chaiscript::fun(&timesL<double, Eigen::VectorXd>), "*");
   add(chaiscript::fun(&timesR<Eigen::VectorXd, double>), "*");
   add(chaiscript::fun(&dot<Eigen::VectorXd>), "dot");

   add(chaiscript::fun(&assign<Eigen::MatrixXd>), "=");
   add(chaiscript::fun(&timesL<double, Eigen::MatrixXd>), "*");
   add(chaiscript::fun(&timesL<Eigen::VectorXd, Eigen::MatrixXd>), "*");
   add(chaiscript::fun(&timesR<Eigen::MatrixXd, double>), "*");
   add(chaiscript::fun(&timesR<Eigen::MatrixXd, Eigen::VectorXd>), "*");
}

bool ChaiEngine::registered(const std::string& module, const std::string& var)
{
   if (!chai_rg.count(var))
      return false;
   
   if (chai_rg[var] == module)
      return true;
   
   return false;
}
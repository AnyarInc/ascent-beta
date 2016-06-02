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

void assignVector2d(Eigen::Vector2d& lhs, const Eigen::Vector2d& rhs) { lhs = rhs; }
Eigen::Vector2d times0Vector2d(const double lhs, const Eigen::Vector2d& rhs) { return lhs*rhs; }
Eigen::Vector2d times1Vector2d(const Eigen::Vector2d& lhs, const double rhs) { return lhs*rhs; }
double dotVector2d(const Eigen::Vector2d& lhs, const Eigen::Vector2d& rhs) { return lhs.dot(rhs); }

void assignVector3d(Eigen::Vector3d& lhs, const Eigen::Vector3d& rhs) { lhs = rhs; }
Eigen::Vector3d times0Vector3d(const double lhs, const Eigen::Vector3d& rhs) { return lhs*rhs; }
Eigen::Vector3d times1Vector3d(const Eigen::Vector3d& lhs, const double rhs) { return lhs*rhs; }
double dotVector3d(const Eigen::Vector3d& lhs, const Eigen::Vector3d& rhs) { return lhs.dot(rhs); }
Eigen::Vector3d crossVector3d(const Eigen::Vector3d& lhs, const Eigen::Vector3d& rhs) { return lhs.cross(rhs); }

void assignVector4d(Eigen::Vector4d& lhs, const Eigen::Vector4d& rhs) { lhs = rhs; }
Eigen::Vector4d times0Vector4d(const double lhs, const Eigen::Vector4d& rhs) { return lhs*rhs; }
Eigen::Vector4d times1Vector4d(const Eigen::Vector4d& lhs, const double rhs) { return lhs*rhs; }
double dotVector4d(const Eigen::Vector4d& lhs, const Eigen::Vector4d& rhs) { return lhs.dot(rhs); }

Eigen::Vector4d times0VectorXd(const double lhs, const Eigen::VectorXd& rhs) { return lhs*rhs; }
Eigen::Vector4d times1VectorXd(const Eigen::VectorXd& lhs, const double rhs) { return lhs*rhs; }
double dotVectorXd(const Eigen::VectorXd& lhs, const Eigen::VectorXd& rhs) { return lhs.dot(rhs); }

Eigen::MatrixXd times0MatrixXd(const double lhs, const Eigen::MatrixXd& rhs) { return lhs*rhs; }
Eigen::MatrixXd times1MatrixXd(const Eigen::MatrixXd& lhs, const double rhs) { return lhs*rhs; }
Eigen::MatrixXd times2MatrixXd(const Eigen::VectorXd& lhs, const Eigen::MatrixXd& rhs) { return lhs*rhs; }
Eigen::MatrixXd times3MatrixXd(const Eigen::MatrixXd& lhs, const Eigen::VectorXd& rhs) { return lhs*rhs; }

ChaiEngine::ChaiEngine() : chaiscript::ChaiScript(chaiscript::Std_Lib::library())
{
   // Register Eigen matrix/vector types
   add(chaiscript::constructor<Eigen::Vector2d(const double x0, const double x1)>(), "Vector2d");
   add(chaiscript::constructor<Eigen::Vector2d(const Eigen::Vector2d&)>(), "Vector2d");
   add(chaiscript::fun(&assignVector2d), "=");
   add(chaiscript::fun(&times0Vector2d), "*");
   add(chaiscript::fun(&times1Vector2d), "*");
   add(chaiscript::fun(&dotVector2d), "dot");

   add(chaiscript::constructor<Eigen::Vector3d(const double x0, const double x1, const double x2)>(), "Vector3d");
   add(chaiscript::constructor<Eigen::Vector3d(const Eigen::Vector3d&)>(), "Vector3d");
   add(chaiscript::fun(&assignVector3d), "=");
   add(chaiscript::fun(&times0Vector3d), "*");
   add(chaiscript::fun(&times1Vector3d), "*");
   add(chaiscript::fun(&dotVector3d), "dot");
   add(chaiscript::fun(&crossVector3d), "cross");

   add(chaiscript::constructor<Eigen::Vector4d(const double x0, const double x1, const double x2, const double x3)>(), "Vector4d");
   add(chaiscript::constructor<Eigen::Vector4d(const Eigen::Vector4d&)>(), "Vector4d");
   add(chaiscript::fun(&assignVector4d), "=");
   add(chaiscript::fun(&times0Vector4d), "*");
   add(chaiscript::fun(&times1Vector4d), "*");
   add(chaiscript::fun(&dotVector4d), "dot");

   add(chaiscript::fun(&times0VectorXd), "*");
   add(chaiscript::fun(&times1VectorXd), "*");
   add(chaiscript::fun(&dotVectorXd), "dot");

   add(chaiscript::fun(&times0MatrixXd), "*");
   add(chaiscript::fun(&times1MatrixXd), "*");
   add(chaiscript::fun(&times2MatrixXd), "*");
   add(chaiscript::fun(&times3MatrixXd), "*");
}

bool ChaiEngine::registered(const std::string& module, const std::string& var)
{
   if (!chai_rg.count(var))
      return false;
   
   if (chai_rg[var] == module)
      return true;
   
   return false;
}
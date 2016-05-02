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

#pragma warning(disable:4503)
#include <chaiscript/chaiscript.hpp>

#include <memory>
#include <typeindex>
#include <unordered_set>

namespace asc
{
   class ChaiEngine : public chaiscript::ChaiScript
   {
   public:
      ChaiEngine();
      
      bool registered(const std::string& module, const std::string& var);
      
      std::unordered_set<std::string> modules; // modules registered with ChaiScript
      std::unordered_set<std::type_index> links; // links registered with ChaiScript
      std::map<std::string, std::string> chai_rg; // ChaiScript registered pairs of Module and variable names.      
   };
}
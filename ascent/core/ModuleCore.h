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
#include <memory>

namespace asc
{
   class Module;
   class Simulator;

   class ModuleCore
   {
   public:
      ModuleCore() {}

      static void error(const size_t sim, const std::string& description);
      
      static std::map<std::string, Module*> external; // registered module names with associated modules IDs (size_t module_id), allowing external access to modules via these names
      static Module& getExternal(const std::string& name);
      
      static std::map<size_t, Module*> accessor; // used to access modules by module_id across all simulators
      static Module& getModule(const size_t id);
      
      static std::map<size_t, std::unique_ptr<Simulator>> simulators;
      static Simulator& getSimulator(const size_t sim);
   };
}
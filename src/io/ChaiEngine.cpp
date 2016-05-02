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

using namespace asc;

ChaiEngine::ChaiEngine() : chaiscript::ChaiScript(chaiscript::Std_Lib::library())
{

}

bool ChaiEngine::registered(const std::string& module, const std::string& var)
{
   if (!chai_rg.count(var))
      return false;
   
   if (chai_rg[var] == module)
      return true;
   
   return false;
}
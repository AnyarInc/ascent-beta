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

#include "ascent/core/Stopper.h"

#include "ascent/Module.h"

using namespace asc;

void Stopper::check()
{
   auto it = stoppers.begin();
   while (it != stoppers.end())
   {
      if (auto ptr = it->lock())
      {
         ptr->callCheck();
         if (false == ptr->stop)
            return;

         ++it;
      }
      else
         stoppers.erase(it++);
   }

   // Will only be reached if all stop conditions are true.
   for (auto& ptr : stoppers)
   {
      if (auto s = ptr.lock())
         s->simulator.stop_simulation = true;
   }
}
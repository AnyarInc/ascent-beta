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

#include "ascent/history/History.h"

#include "ascent/core/ModuleCore.h"
#include "ascent/core/Simulator.h"

using namespace asc;

History::History(const size_t sim, const size_t steps) : simulator(ModuleCore::getSimulator(sim)),
   steps(steps), infinite(false),
   t(simulator.t), dt(simulator.dt)
{

}

History::History(const size_t sim) : History(sim, 0)
{
   infinite = true;
}

void History::error(const std::string& description)
{
   simulator.setError(description);
}

void History::insert(const double value)
{
   if (th.size() > 0)
   {
      if (simulator.t <= th.back())
         simulator.setError("Attempted push_back on History when the current time is less than or equal to the last time recorded.");
   }

   th.push_back(simulator.t);
   x.push_back(value);
}

void History::push_back(const double value)
{
   if (infinite)
      insert(value);
   else if (steps > 0)
   {
      while (x.size() >= steps) // allows number of steps to be lengthened or reduced during runtime
      {
         th.erase(th.begin());
         x.erase(x.begin());
      }

      insert(value);
   }
}
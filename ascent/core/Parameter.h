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

#include "Simulator.h"

#include <deque>
#include <map>

#include <typeindex>
#include <type_traits>

namespace asc
{
   template <typename T>
   class Parameter
   {
   private:
      bool initialized = false;
      Simulator* simulator = nullptr; // do not delete

   public:
      Parameter() {}
      Parameter(Simulator* simulator) : simulator(simulator) {}

      size_t t_begin = 0; // The index of the t_hist vector for which tracking history begins, this will be updated for history with finite steps.
      bool infinite = false; // If true, an infinite amount of steps will be recorded.
      size_t steps = 0; // Number of steps to keep track of, if steps == 0 then no history will be maintained.
      bool clear_on_access = false; // Whether or not the old history data should be cleared when accessed.

      std::deque<T> x; // parameter history (std::deque is used for fast erasing of the first element)
      T* ptr = nullptr; // pointer to parameter (memory handled by Module)
      
      void update()
      {
         if (!initialized)
            t_begin = simulator->t_hist.size() - 1;

         if (infinite)
            x.push_back(*ptr);
         else if (steps > 0)
         {
            while (x.size() >= steps)
            {
               ++t_begin; // Increment t_hist because the starting time value is being shifted toward the more current time.
               x.erase(x.begin());
            }

            x.push_back(*ptr);
         }
      }

      std::deque<double> time() // Get time vector associated with x parameter history.
      {
         const std::vector<double>& t = simulator->t_hist;
         std::deque<double> th; // time history
         size_t end = t_begin + x.size();
         for (size_t i = t_begin; i < end; ++i)
            th.push_back(t[i]);
         return th;
      }

      std::deque<T> history()
      {
         return x;
      }

      std::string type() const { return typeid(T).name(); }

      size_t length() const { return x.size(); }
   };
}
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

// This call is intended to filter out old history data so that graphing functions can more quickly plot a significant time history of a variable.

// THIS ALGORITHM HAS NOT BEEN OPTIMIZED FOR ERASING, BECAUSE IT USES std::deque, IMPROVE THIS IN THE FUTURE TO IMPROVE PERFORMANCE

#include "History.h"

namespace asc
{
   template <size_t sample_size>
   class HistorySparse : public History
   {
   private:
      size_t index = 1; // second element
      double spacing_ratio = 0.001;

   public:
      HistorySparse(const size_t sim) : History(sim)
      {
         History::steps = sample_size;
      }

      void push_back(const double value)
      {
         size_t loop = 0;

         while (x.size() >= sample_size)
         {
            // Ideally we would work backwards halfway and then forwards halfway, because often the latest step added is the one to be erased

            double time_diff = abs(th[index] - th[index - 1]);

            if (time_diff < dt() + spacing_ratio*dt()) // if the sampled values are distanced less than the end sampling distance (within some extra room), then we want to remove the sample
            {
               x.erase(x.begin() + index);
               th.erase(th.begin() + index);
            }

            if (loop > 0)
               spacing_ratio *= 2.0;

            ++index;

            if (index >= x.size())
            {
               index = 1; // reset the index
               ++loop;
            }
         }

         History::push_back(value);
      }
   };
}
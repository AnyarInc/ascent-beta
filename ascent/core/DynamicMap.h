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
#include <vector>

namespace asc
{
   template <typename T1, typename T2>
   class DynamicMap
   {
   private:
      std::map<T1, T2> dynamic_map;
      std::vector<T1> to_erase;

   public:
      DynamicMap() {}

      std::map<T1, T2>& getMap()
      {
         return dynamic_map;
      }

      T2& operator [](const T1& key) { return dynamic_map[key]; }

      size_t count(const T1& key) { return dynamic_map.count(key); }

      size_t size() { return dynamic_map.size(); }

      auto begin() { return dynamic_map.begin(); }
      auto end() { return dynamic_map.end(); }

      void directErase(const T1& key)
      {
         dynamic_map.erase(key);
      }

      bool direct_erase = true; // Whether or not calls to erase should be direct erases, not postponed. Default is true.

      void erase(const T1& key)
      {
         if (direct_erase)
            directErase(key);
         else
            to_erase.push_back(key);
      }

      void erase()
      {
         if (to_erase.size() > 0)
         {
            for (auto key : to_erase)
               dynamic_map.erase(key);

            to_erase.clear();
         }
      }
   };
}
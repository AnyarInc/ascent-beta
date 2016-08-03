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

#include "Parameter.h"
#include "ToString.h"

#include <iostream>
#include <functional>
#include <map>
#include <sstream>
#include <memory>

namespace asc
{
   class Vars
   {
      friend class Module;
   private:
      Simulator& simulator;

      // stores pointers to maps of Parameters
      std::map<std::type_index, void*> maps;
      std::map<std::type_index, std::function<void()>> delete_map;

      // A vector of all initialized object typeid(T).name() definitions and variable names. e.g. ("double", "height")
      std::vector<std::pair<std::string, std::string>> names;

      std::map<std::string, std::function<void()>> update_map;
      std::map<std::string, std::function<std::string(const size_t i)>> print_map;
      std::map<std::string, std::function<std::string()>> type_map;
      std::map<std::string, std::function<size_t()>> length_map;
      std::map<std::string, std::function<size_t()>> t_begin_map;
      std::map<std::string, std::function<void(size_t steps)>> steps_map;
      std::map<std::string, std::function<void(bool infinite)>> steps_infinite_map;

      template <typename T>
      std::map<std::string, Parameter<T>>& getMap()
      {
         return *static_cast<std::map<std::string, Parameter<T>>*>(maps[typeid(T)]);
      }

      template <typename T>
      T* getPtr(const std::string &id)
      {
         if (maps.count(typeid(T)))
         {
            auto& map = getMap<T>();

            auto p = map.find(id);
            if (p != map.end())
               return p->second.ptr;
            else
               simulator.setError("Variable <" + id + "> could not be located.");
         }
         else
            simulator.setError((std::string)"No variables of type <" + typeid(T).name() + "> were set for access.");

         return nullptr;
      }

   public:
      Vars(Simulator& simulator) : simulator(simulator) {}
      
      ~Vars()
      {
         for (auto p : maps)
            delete_map[p.first]();
      }

      template <typename T>
      Parameter<T>& initNoTrack(const std::string &id, T &x)
      {
         if (maps.count(typeid(T)) == 0) // if this type hasn't been initialized before, then create a new container for this type
         {
            auto map_ptr = new std::map<std::string, Parameter<T>>();
            maps[typeid(T)] = map_ptr;
            delete_map[typeid(T)] = [=] () { delete map_ptr; };
         }

         auto& map = getMap<T>();

         auto p = map.find(id);
         if (p != map.end())
            simulator.setError("id of <" + id + "> was already initialized. Overwriting.");

         names.push_back(std::pair<std::string, std::string>(typeid(T).name(), id));

         Parameter<T> param(&simulator);
         param.ptr = &x;
         map.emplace(id, param); // copy of Parameter param made (copy is legitimate because its pointers are copied)

         Parameter<T>& ref = map[id]; // get reference to copy

         type_map[id] = [&]() -> std::string { return ref.type(); };

         return ref;
      }

      template <typename T>
      void init(const std::string& id, T& x, size_t steps)
      {
         Parameter<T>& ref = initNoTrack(id, x);

         ref.steps = steps;

         update_map[id] = [&]() { ref.update(); };
         length_map[id] = [&]() -> size_t { return ref.length(); };
         t_begin_map[id] = [&]() -> size_t { return ref.t_begin; };
         steps_map[id] = [&](size_t steps) { ref.steps = steps; };
         steps_infinite_map[id] = [&](bool inifinite) { ref.infinite = inifinite; };

         print_map[id] = [&](const size_t i) -> std::string { return ToString::print(ref.x[i]); };
      }

      bool trackable(const std::string& id)
      {
         if (update_map.count(id))
            return true;
         
         return false;
      }

      void update(const std::string& id)
      {
         if (update_map.count(id))
            update_map[id]();
         else
            simulator.setError("Access failure in Vars::update(const std::string& id)");
      }

      void update() // updates all tracked parameters
      {
         for (auto& p : update_map)
            p.second();
      }

      std::string print(const std::string& id)
      {
         size_t n = length(id) - 1; // get last element
         return print(id, n);
      }

      std::string print(const std::string& id, const size_t i)
      {
         if (print_map.count(id))
            return print_map[id](i);
         simulator.setError("Access failure in Vars::print(const std::string& id, const size_t i)");
         return "";
      }

      std::string type(const std::string& id)
      {
         if (type_map.count(id))
            return type_map[id]();
         simulator.setError("Access failure in Vars::type(const std::string& id)");
         return "";
      }

      size_t length(const std::string& id) // returns length of id's Parameter history
      {
         if (length_map.count(id))
            return length_map[id]();
         simulator.setError("Access failure in Vars::length(const std::string& id)");
         return 0;
      }

      size_t tBegin(const std::string& id)
      {
         if (t_begin_map.count(id))
            return t_begin_map[id]();
         simulator.setError("Access failure in Vars::tBegin(const std::string& id)");
         return 0;
      }

      template <typename T>
      bool set(const std::string& id, const T& x)
      {
         T* ptr = getPtr<T>(id);
         if (ptr)
         {
            *ptr = x;
            return true;
         }

         simulator.setError("id of <" + id + "> could not be accessed via Vars::set<T>(const std::string& id, const T& x).");
         return false;
      }

      template <typename T>
      T get(const std::string& id)
      {
         T* ptr = getPtr<T>(id);
         if (ptr)
            return *ptr;

         simulator.setError("id of <" + id + "> could not be accessed via Vars::get<T>(const std::string& id).");
         return T();
      }

      template <typename T>
      std::deque<T> history(const std::string &id)
      {
         if (maps.count(typeid(T)))
         {
            auto& map = getMap<T>();

            auto p = map.find(id);
            if (p != map.end())
               return p->second.history();
            else
               simulator.setError("Variable <" + id + "> could not be located.");
         }
         else
            simulator.setError(static_cast<std::string>("No variables of type <") + typeid(T).name() + "> were set for access.");

         return std::deque<T>();
      }

      void steps(const std::string& id, size_t steps)
      {
         if (steps_map.count(id))
            steps_map[id](steps);
         else
            simulator.setError("Access failure in Vars::setSteps(" + id + ", " + std::to_string(steps) + ")");
      }

      void steps(const std::string& id, bool infinite = true)
      {
         if (steps_infinite_map.count(id))
            steps_infinite_map[id](infinite);
         else
            simulator.setError("Access failure in Vars::setStepsInfinite(" + id + ", " + std::to_string(infinite) + ")");
      }

      auto& getNames() { return names; }
   };
}
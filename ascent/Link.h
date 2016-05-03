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

#include "ascent/core/LinkBase.h"
#include "ascent/Module.h"

namespace asc
{
   template <typename T>
   class Link : public LinkBase
   {
      friend class Module;
      friend class Stopper;

      template <typename T1>
      friend class Link;

   private:
      std::shared_ptr<Unqualified<T>> module;

   public:
      Link() {}

      template <typename... Types>
      Link(const size_t sim, Types&&... args) : module(new Unqualified<T>(sim, std::forward<Types>(args)...)) {}

      Link(const Link<T>& link)
      {
         assign(link);
      }

      template <typename T1>
      Link(const Link<T1>& link)
      {
         assign(link);
      }

      ~Link()
      {
         if (module)
         {
            if (module.use_count() == 1) // remove this module if it is the last one, because the module contains a shared_ptr to itself
            {
               Simulator& simulator = module->simulator;
               if (simulator.phase != Phase::setup)
                  simulator.to_delete.push_back(module); // transfer ownership of the shared_ptr
               else
                  module = nullptr; // destroy this module immediately because the simulation isn't running
            }
         }
      }

      T* operator -> () { return access(); } // The pointer is copied with T qualifiers, so that the module can be const qualified.

      Link<T>& operator = (const Link<T>& link) { assign(link); return *this; }

      template <typename T1>
      Link<T>& operator = (const Link<T1>& link) { assign(link); return *this; }

      std::string classInfo() const { return "Link<" + static_cast<std::string>(typeid(T).name()) + ">"; }

      explicit operator bool() const { return (module.get() != 0); }

      bool name(const std::string& name)
      {
         return module->template name<T>(name);
      }

   private:
      T* access()
      {
         if (!module)
         {
            std::cerr << "Attempted access [->] from a non-initialized (i.e. non-instantiated) module: " + classInfo() << '\n';
            return nullptr;
         }

         Simulator& simulator = module->simulator;
         const Phase& phase = simulator.phase;

         if (phase != Phase::setup)
         {
            // For all phases except "setup", init() must have been called prior to access.
            // Calling the method callInit() ensures that initialization has been properly handled. Once a module has been initialized this method will simply do nothing.
            // This call is essential for simulations that add modules during the simulation loop.
            module->callInit();

            switch (phase)
            {
            case Phase::update:
               module->callUpdate();
               break;
            case Phase::postcalc:
               module->callPostCalc();
               break;
            case Phase::check:
               module->callCheck();
               break;
            case Phase::reset:
               module->callReset();
               break;
            default:
               break;
            }
         }

         if (simulator.error)
            simulator.setError(classInfo() + "::access()");

         return module.get();
      }

      template <typename T1>
      void assign(const Link<T1>& link)
      {
         module = std::static_pointer_cast<Unqualified<T>>(link.module);
      }

      void assign(Module& base)
      {
         // shared_from_this() must be cast in order to increment reference count on shared_ptr (myself can't be used because it has a null deleter)
         // need dynamic_pointer_cast to allow multiple inheritance for modules
         module = std::dynamic_pointer_cast<Unqualified<T>>(base.shared_from_this());
      }

      void assignLinkBase(LinkBase& link_base)
      {
         assign(static_cast<Link<T>&>(link_base));
      }
   };
}
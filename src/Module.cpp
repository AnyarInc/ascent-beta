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

#include "ascent/Module.h"
#include "ascent/core/ModuleCore.h"

#include "ascent/Link.h"

// Protecting json max/min macros from ChaiScript windows max/min macros
#undef max
#undef min

#include "jsoncons/json.hpp"
#include "jsoncons/json_deserializer.hpp"
#include "jsoncons_ext/csv/csv_reader.hpp"

using namespace asc;
using namespace std;

std::map<std::string, Module*> ModuleCore::external;
std::map<size_t, Module*> ModuleCore::accessor;
std::map<size_t, std::unique_ptr<Simulator>> ModuleCore::simulators;

std::string Module::file_type = ".csv";
size_t Module::next_module_id = 0;

struct null_deleter { void operator()(void const *) const {} };

// ModuleCore
void ModuleCore::error(const size_t sim, const std::string& description)
{
   getSimulator(sim).setError(description);
}

Module& ModuleCore::getExternal(const std::string& name)
{
   return *external[name];
}

Module& ModuleCore::getModule(const size_t id)
{
   return *accessor[id];
}

Simulator& ModuleCore::getSimulator(const size_t sim)
{
   if (!simulators.count(sim))
      simulators.emplace(sim, std::make_unique<Simulator>(sim));

   return *simulators[sim];
}

// Module
Module::Module(size_t sim) : simulator(getSimulator(sim)),
   t(simulator.t), dt(simulator.dt), dt_base(simulator.dtp),
   sim(sim),
   module_id(next_module_id),
   chai(*simulator.chai),
   myself(this, null_deleter()),
   vars(simulator),
   external(ModuleCore::external)
{
   ++next_module_id;
   ModuleCore::accessor[module_id] = this;

   simulator.modules[module_id] = this;
   
#define ascNS Module
   ascVar(frozen);

   // We have to take care not to invalidate the iterators for the maps within the Simulator if a module is added during run time.
   // We can't use call methods here like callInit() or callUpdate(), because the virtual implementations of the phase methods haven't been instantiated since we are in the constructor of the parent class.
   // Instead, we pass information to the simulator to call these methods after the current loop is finished.
   if (simulator.phase == Phase::setup)
      addPhases();
   else
      simulator.to_add.push_back(this);
}

void Module::addPhases()
{
   simulator.inits[module_id] = this;
   simulator.updates[module_id] = this;
   simulator.postcalcs[module_id] = this;
   simulator.checks[module_id] = this;
   simulator.reports[module_id] = this;
   simulator.resets[module_id] = this;
}

Module::~Module()
{
   for (State* state : states)
      delete state;

   ModuleCore::accessor.erase(module_id);

   if (ModuleCore::external.count(module_name))
      ModuleCore::external.erase(module_name);

   // Pointers shouldn't be deleted because they are to this class:

   // directErase is used here because when a module is destroyed all pointers to the object must be removed now and can't be delayed.
   // Removing pointers is delayed while modules are still alive so that erasing can be handled while looping.

   // The module is garunteed to be erasable:
   simulator.modules.directErase(module_id);

   if (simulator.inits.count(module_id))
      simulator.inits.directErase(module_id);

   if (simulator.updates.count(module_id))
      simulator.updates.directErase(module_id);

   if (simulator.postcalcs.count(module_id))
      simulator.postcalcs.directErase(module_id);

   if (simulator.checks.count(module_id))
      simulator.checks.directErase(module_id);

   if (simulator.reports.count(module_id))
      simulator.reports.directErase(module_id);

   if (simulator.resets.count(module_id))
      simulator.resets.directErase(module_id);

   if (simulator.propagate.count(module_id))
      simulator.propagate.directErase(module_id);

   if (simulator.trackers.count(module_id))
      simulator.trackers.directErase(module_id);

   if (simulator.modules.size() == 0) // erase the simulator if there are no more modules
      ModuleCore::simulators.erase(sim);

   // We need to erase the global_chai_engine if the program is going to close, to avoid errors in ChaiScript when multi-threading is enabled.
   // The Simulator class will generate a new ChaiEngine if a new Simulator is created.
   if (ModuleCore::simulators.size() == 0)
      Simulator::global_chai_engine = nullptr;
}

std::string Module::name() const
{
   if ("" == module_name)
      module_name = "<" + to_string(module_id) + "|" + to_string(sim) + ">";

   return module_name;
}

void Module::addIntegrator(double &x, double &xd, const double tolerance)
{
   if (!simulator.propagate.count(module_id)) // if no integrators have been added (i.e. this module hasn't been added to be propagated)
      simulator.propagate[module_id] = this;

   states.push_back(simulator.integrator->factory(x, xd));
   states.back()->tolerance = tolerance;
}

void Module::propagateStates()
{
   for (State* state : states)
      state->propagate();
}

void Module::callInit()
{
   if (!init_run)
   {
      if (init_called)
         error("Module: Circular dependency for init().");
      else
      {
         init_called = true;
         if (!frozen)
            init();
      }
      
      init_run = true;
      init_called = false;
      simulator.inits.erase(module_id); // Erasing module from inits because it should only be called once.
   }
}

void Module::callUpdate()
{
   if (!update_run)
   {
      std::vector<size_t> to_delete;
      for (auto& p : run_first)
      {
         if (auto ptr = p.second.lock())
         {
            if (!ptr->update_run)
            {
               if (ptr->update_called) // If the run_first map contains an updating module, then we shouldn't update this module yet.
                  return;

               ptr->callUpdate();
               if (!ptr->update_run) // If the call to update didn't update the module, then we shouldn't update this module yet.
                  return;
            }
         }
         else
            to_delete.push_back(p.first);
      }

      for (size_t id : to_delete)
         run_first.erase(id);

      if (update_called)
         error("Circular dependency for update(). Within " + name());
      else
      {
         update_called = true;
         if (!frozen)
            update();
      }
      
      update_run = true;
      update_called = false;
   }
}

void Module::callPostCalc()
{
   if (!postcalc_run)
   {
      std::vector<size_t> to_delete;
      for (auto& p : run_first)
      {
         if (auto ptr = p.second.lock())
         {
            if (!ptr->postcalc_called)
            {
               if (ptr->postcalc_called)
                  return;

               ptr->callPostCalc();
               if (!ptr->postcalc_run)
                  return;
            }
         }
         else
            to_delete.push_back(p.first);
      }

      for (size_t id : to_delete)
         run_first.erase(id);

      if (postcalc_called)
         error("Circular dependency for postcalc(). Within " + name());
      else
      {
         postcalc_called = true;
         if (!frozen)
            postcalc();
      }
      
      postcalc_run = true;
      postcalc_called = false;
   }
}

void Module::callCheck()
{
   if (!check_run)
   {
      if (check_called)
         error("Circular dependency for check().");
      else
      {
         check_called = true;
         if (!frozen)
            check();
      }
      
      check_run = true;
      check_called = false;
   }
}

void Module::callReport()
{
   if (!report_run)
   {
      if (report_called)
         error("Circular dependency for report().");
      else
      {
         report_called = true;
         if (!frozen)
            report();
      }
      
      report_run = true;
      report_called = false;
   }
}

void Module::callReset()
{
   if (!reset_run)
   {
      if (reset_called)
         error("Circular dependency for reset().");
      else
      {
         reset_called = true;
         if (!frozen)
            reset();
      }
      
      reset_run = true;
      reset_called = false;
   }
}

void Module::track(const std::string& var_name)
{
   if ("t" == var_name)
   {
      print_time = true;
      simulator.track_time = true;
   }
   else
   {
      tracking.push_back(std::make_pair(module_id, var_name));
      steps(var_name);
   }

   local_tracking.insert(var_name);
}

void Module::track(const std::string& module_name, const std::string& var_name)
{
   track(*ModuleCore::external[module_name], var_name);
}

void Module::track(Module& module, const std::string& var_name)
{
   if ("t" == var_name)
   {
      print_time = true;
      simulator.track_time = true;
   }
   else
   {
      tracking.push_back(std::make_pair(module.module_id, var_name));
      ModuleCore::getModule(module.module_id).steps(var_name);
   }
}

void Module::outputTrack()
{
   ofstream file;
   string filename = module_directory + module_name + file_type;
   file.open(filename);

   if (file)
   {
      streamTrack(file);
   }
   else
      error("File " + filename + " could not be created.");
   
   file.close();
}

std::string Module::csvTrack()
{
   stringstream ss{};
   
   streamTrack(ss);

   return ss.str();
}

std::string Module::jsonTrack()
{
   stringstream ss{};

   streamTrack(ss);

   jsoncons::json_deserializer handler;

   jsoncons::csv::csv_reader reader(ss, handler);
   reader.read();

   return handler.get_result().to_string();
}

void Module::streamCSV(std::shared_ptr<std::stringstream>& ss)
{
   std::string var_name = tracking.front().second;

   if (!ss) // Not initialized, so append names
   {
      ss = std::make_shared<std::stringstream>();
      std::stringstream& stream = *ss;

      if (print_time)
         stream << "t" << ",";

      size_t n = tracking.size();
      for (size_t i = 0; i < n; ++i)
      {
         auto& p = tracking[i];
         if (i == n - 1) // last parameter
            stream << getModule(p.first).name() << " " << p.second;
         else
            stream << getModule(p.first).name() << " " << p.second << ",";
      }

      stream << '\n';
   }

   std::stringstream& stream = *ss;

   if (print_time)
      stream << t << ",";

   size_t n = tracking.size();
   for (size_t j = 0; j < n; ++j)
   {
      auto& p = tracking[j];
      stream << getModule(p.first).vars.print(p.second);
      if (j < n - 1) // not the last parameter
         stream << ",";
   }

   stream << '\n';
}

Module& Module::getModule(const size_t id)
{
   return ModuleCore::getModule(id);
}

Simulator& Module::getSimulator(const size_t sim)
{
   return ModuleCore::getSimulator(sim);
}

void asc::integrationTolerance(size_t sim, const double tolerance)
{
   ModuleCore::getSimulator(sim).integrationTolerance(tolerance);
}

void asc::generateInputFile(const std::string& name)
{
   std::string filename = name + ".asc";

   std::ofstream file;
   file.open(name);

   if (file)
   {
      for (auto& p : ModuleCore::external)
      {
         auto& module = *p.second;
         auto& var_names = module.varNames();

         for (auto& var : var_names)
         {
            if (var.first == typeid(double).name())
               file << module.varToString<double>(var);
            else if (var.first == typeid(bool).name())
            {
               bool value = module.vars.get<bool>(var.second);
               if (value)
                  file << module.name() + "." + var.second + " = true;\n";
               else
                  file << module.name() + "." + var.second + " = false;\n";
            }
            else if (var.first == typeid(size_t).name())
               file << module.varToString<size_t>(var);
            else if (var.first == typeid(int).name())
               file << module.varToString<int>(var);
            else if (var.first == typeid(unsigned).name())
               file << module.varToString<unsigned>(var);
            else if (var.first == typeid(Eigen::Vector3d).name())
            {
               Eigen::Vector3d vec = module.vars.get<Eigen::Vector3d>(var.second);
               file << module.name() + "." + var.second + " = Vector3d(" + std::to_string(vec.x()) + ", " + std::to_string(vec.y()) + ", " + std::to_string(vec.z()) + ");\n";
            }
         }

         file << '\n';
      }
   }
}
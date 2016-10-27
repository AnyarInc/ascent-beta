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

#include "ascent/core/Simulator.h"

#include "ascent/Module.h"
#include "ascent/integrators/RK4.h"

#include <assert.h>

using namespace asc;

bool GlobalChaiScript::on = false;
bool GlobalChaiScript::initialized = false;

std::map<std::string, std::shared_ptr<Module>> Simulator::tracking;

std::shared_ptr<ChaiEngine> Simulator::global_chai_engine{};

using namespace std;

struct null_deleter { void operator()(void const *) const {} };

Simulator::Simulator(size_t sim) : sim(sim), stepper(EPS, dtp, dt, t, t1, kpass, integrator_initialized)
{
   if (GlobalChaiScript::on)
   {
      if (!global_chai_engine)
         global_chai_engine = std::make_shared<ChaiEngine>();

      chai = global_chai_engine;
   }
   else
      chai = std::shared_ptr<ChaiEngine>(new ChaiEngine(), null_deleter()); // We are using a null_deleter here because ChaiScript was deleting itself

   integrator = std::make_unique<RK4>(stepper);

   if (!GlobalChaiScript::on)
   {
      chai->add(chaiscript::const_var(std::ref(t)), "t");
      chai->add(chaiscript::var(std::ref(dt)), "dt");
      chai->add(chaiscript::var(std::ref(dtp)), "dt_base");
      chai->add(chaiscript::var(std::ref(t_end)), "t_end");
   }

   bool register_module = true;
   if (GlobalChaiScript::on && GlobalChaiScript::initialized)
      register_module = false;

   if (register_module)
   {
      using namespace chaiscript;

      // Register functions for Module
      chai->add(fun(static_cast<bool (Module::*)()>(&Module::run)), "run");
      chai->add(fun(static_cast<bool (Module::*)(const double, const double)>(&Module::run)), "run");
      chai->add(fun(static_cast<void (Module::*)(const std::string&)>(&Module::track)), "track");
      chai->add(fun(static_cast<void (Module::*)(const std::string&, const std::string&)>(&Module::track)), "track");
      chai->add(fun(static_cast<void (Module::*)(asc::Module&, const std::string&)>(&Module::track)), "track");
      chai->add(fun(static_cast<void (Module::*)()>(&Module::outputTrack)), "outputTrack");

      chai->add(fun(&Module::chaiscript_event), "event");
   }

   ascType(Module, "Module");

   GlobalChaiScript::initialized = true;
}

bool Simulator::run(const double dt_base, const double tmax)
{
   t_end = tmax;

   if (t_end <= t)
      setError("The end time : " + to_string(t_end) + " is less than or equal to the current time : " + to_string(t));

   if (modules.size() == 0)
      setError("There are no modules to run.");

   if (!error)
   {
      setup(dt_base);

      init();
   }

   while (!error)
   {
      event(t_end);

      if (tickfirst)
      {
         if (tick0 && track_time) // If the very first tick of the simulation.
            t_hist.push_back(t);

         changeTimeStep();
         report();

         if (tick0) // tracker() must run after report(), but t_hist must be recorded before rpt(), thus tick0 checks are separated.
         {
            tracker();
            tick0 = false;
         }
      }

      update();

      tickfirst = false;

      if (sample())
      {
         if (integrator->adaptiveFSAL() && integrator_initialized)
            adaptiveCalc();
      }

      propagateStates();
      updateClock();

      if (sample())
      {
         if (track_time)
            t_hist.push_back(t);

         postcalc();

         chaiscript_event();

         check();

         runStoppers();

         if (stop_simulation || (t + EPS >= t_end))
            ticklast = true;

         report();

         tracker();

         if (integrator->adaptive())
            adaptiveCalc();

         changeTimeStep();
         changeEndTime();

         deleteModules();

         if (ticklast)
         {
            createFiles();
            break;
         }
      }

      reset();
   }
   
   directErase(true); // Specify that all DynamicMaps should use direct erasing since the simulation finished.

   phase = Phase::setup;

   if (error)
      return setError("Simulation was stopped due to an ERROR.");
   return true;
}

void Simulator::directErase(bool b)
{
   modules.direct_erase = b;

   inits.direct_erase = b;
   updates.direct_erase = b;
   postcalcs.direct_erase = b;
   checks.direct_erase = b;
   reports.direct_erase = b;
   resets.direct_erase = b;

   propagate.direct_erase = b;
}

void Simulator::setup(const double dt_base)
{
   phase = Phase::setup;

   this->dt = dtp = dt_base; // sets base time step (dtp) and adjustable time step (dt)
   t1 = t + dt; // sets intended end time of next timestep
   kpass = 0;
   ticklast = false;
   tickfirst = true;
   directErase(false);
   stop_simulation = false;
}

void Simulator::init()
{
   phase = Phase::init;
   
   for (auto& p : inits)
   {
      p.second->callInit();

      if (error)
         break;
   }

   for (Module* module : to_add) // handle modules added during runtime
   {
      module->addPhases();
      module->callInit();
   }
   to_add.clear();

   inits.erase(); // removes any modules that have been set for deletion
}

void Simulator::update()
{
   phase = Phase::update;

   for (auto& p : updates)
   {
      p.second->callUpdate();

      if (error)
         break;
   }

   for (Module* module : to_add) // handle modules added during runtime
   {
      module->addPhases();
      module->callUpdate();
   }
   to_add.clear();

   updates.erase();

   for (auto& p : updates)
      p.second->update_run = false;
}

void Simulator::postcalc()
{
   phase = Phase::postcalc;

   for (auto& p : postcalcs)
   {
      p.second->callPostCalc();

      if (error)
         break;
   }

   for (Module* module : to_add) // handle modules added during runtime
   {
      module->addPhases();
      module->callPostCalc();
   }
   to_add.clear();

   postcalcs.erase();

   for (auto& p : postcalcs)
      p.second->postcalc_run = false;
}

void Simulator::check()
{
   phase = Phase::check;

   for (auto& p : checks)
   {
      p.second->callCheck();

      if (error)
         break;
   }

   for (Module* module : to_add) // handle modules added during runtime
   {
      module->addPhases();
      module->callCheck();
   }
   to_add.clear();

   checks.erase();

   for (auto& p : checks)
      p.second->check_run = false;
}

void Simulator::chaiscript_event()
{
   // There is no ordering on ChaiScript events
   for (auto& p : modules)
   {
      p.second->chaiscript_event();

      if (error)
         break;
   }

   for (Module* module : to_add) // handle modules added during runtime
   {
      module->addPhases();
   }
}

void Simulator::report()
{
   phase = Phase::report;

   for (auto& p : reports)
   {
      p.second->callReport();

      if (error)
         break;
   }

   for (Module* module : to_add) // handle modules added during runtime
   {
      module->addPhases();
      module->callReport();
   }
   to_add.clear();

   reports.erase();

   for (auto& p : reports)
      p.second->report_run = false;
}

void Simulator::reset()
{
   phase = Phase::reset;

   for (auto& p : resets)
   {
      p.second->callReset();

      if (error)
         break;
   }

   for (Module* module : to_add) // handle modules added during runtime
   {
      module->addPhases();
      module->callReset();
   }
   to_add.clear();

   resets.erase();

   for (auto& p : resets)
      p.second->reset_run = false;
}

void Simulator::tracker()
{
   phase = Phase::tracker;

   for (auto& p : trackers)
      p.second->tracker();
}

void Simulator::propagateStates()
{
   for (auto &p : propagate)
   {
      auto module = p.second;
      if (!module->frozen && !module->freeze_integration) // if neither is frozen then propagate
         module->propagateStates();
   }
}

void Simulator::updateClock()
{
   const double t_prev = t;

   integrator->updateClock();

   if (t >= (t_prev + EPS))
      time_advanced = true;
   else
      time_advanced = false;
}

void Simulator::adaptiveCalc()
{
   double dt_optimal = 1.0e9; // Start with huge step size to be reduced.
   bool optimal_found = false;
   for (auto &p : propagate)
   {
      auto module = p.second;
      if (!module->frozen && !module->freeze_integration)
      {
         for (State* state : module->states)
         {
            double computed = state->optimalTimeStep();
            if ((computed > 0.0) && (computed < dt_optimal))
            {
               dt_optimal = computed;
               optimal_found = true;
            }
         }
      }
   }

   if (optimal_found)
   {
      if (dt_optimal < EPS)
         dt_change = EPS;
      else
         dt_change = dt_optimal;
         
      change_dt = true;
   }
}

void Simulator::changeTime(const double t_new)
{
   if (t_new >= 0.0)
   {
      t = t_new;
   }
   else
   {
      setError("attempted to set the new time (t_new) less than zero");
   }

   if (phase != Phase::setup)
      setError("attempted to set a new time while not in the setup phase");
}

void Simulator::changeTimeStep()
{
   if (change_dt)
   {
      dt = dtp = dt_change;
      t1 = t + dt;
      change_dt = false;
   }
}

void Simulator::changeEndTime()
{
   if (change_t_end)
   {
      t_end = t_end_change;
      change_t_end = false;
   }
}

void Simulator::recursiveDelete(const size_t n_prev = 0)
{
   size_t n = to_delete.size();
   if (n > 0)
   {
      for (size_t i = n_prev; i < n; ++i)
         to_delete[i] = nullptr;

      if (to_delete.size() > n) // if the to_delete vector has grown since removing modules
         recursiveDelete(n); // call this function recurvisely until all shared_ptrs are nullptr
   }
}

void Simulator::deleteModules()
{
   recursiveDelete();
   to_delete.clear();
}

bool Simulator::sample(double sdt) // only changes the timestep if the sample produces a time step less than the current time step
{
   if (!sample())
      return false; // if intermediate step
                    
   // calculate the end time if using the sample deltat (sdt)
   double n = floor((t + EPS) / sdt + 1); // number of sample time steps that have occurred + 1, rounded down to nearest whole number
   double ts = n * sdt; // number of time steps till next sample time, multiplied by the sample time step (sdt)
   if (ts < t1 - EPS)
      t1 = ts;

   dt = t1 - t;
   // check to see if it is time to sample
   // Note: the sample will always return true when t == 0.0
   if (t - ts + sdt < EPS)
      return true;
   else
      return false;
}

bool Simulator::event(double t_event)
{
   if (!sample())
      return false; // if intermediate step

   if (t_event < t1 - EPS && t_event >= t + EPS)
      t1 = t_event;

   dt = t1 - t;
   if (fabs(t_event - t) < EPS)
      return true;
   else
      return false;
}

void Simulator::integrationTolerance(double tolerance) // Set global adaptive step size tolerance
{
   for (auto& p : modules)
      p.second->integrationTolerance(tolerance);
}

void Simulator::createFiles()
{
   for (auto& p : tracking)
      p.second->outputTrack();
}

bool Simulator::setError(const std::string& description)
{
   error = true;
   error_descriptions.push_back(description);
   if (print_errors)
      cerr << "ERROR: " + description << '\n';
   throw std::runtime_error(description.c_str());
   return false;
}

void Simulator::runStoppers()
{
   std::vector<size_t> to_erase;

   for (size_t i = 0; i < stoppers.size(); ++i)
   {
      auto& s = stoppers[i];
      s->check();
      if (s->stoppers.size() == 0)
         to_erase.push_back(i);
   }

   for (auto i : to_erase)
      stoppers.erase(stoppers.begin() + i);
}
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

#include "ascent/core/DynamicMap.h"
#include "ascent/io/ChaiEngine.h"

#include "ascent/core/State.h"
#include "ascent/core/Stepper.h"
#include "ascent/core/Stopper.h"

#include <functional>
#include <iostream>
#include <string>

namespace asc
{
   template <typename T>
   class Link;

   enum class Phase
   {
      setup,
      init,
      update,
      postcalc,
      check,
      report,
      reset,
      tracker
   };

   class Simulator
   {
      typedef DynamicMap<size_t, Module*> module_map;

   public:
      Simulator(size_t sim);

      ChaiEngine chai;

      bool setError(const std::string& description); // always returns false

      bool track_time = false;
      std::vector<double> t_hist; // time history (used to interpolate and provide time pairing with Parameter history)

      bool run(const double dt, const double t_end);
      bool run() { return run(dt, t_end); }

      bool sample() { return (kpass == 0); }
      bool sample(double sdt);
      bool event(double t_event);

      bool error = false;
      std::vector<std::string> error_descriptions;
      bool print_errors = true;

      bool stop_simulation = false;

      const size_t sim;

      void directErase(bool b);

      module_map modules;

      module_map inits;
      module_map updates;
      module_map postcalcs;
      module_map checks;
      module_map reports;
      module_map resets;

      module_map propagate;

      module_map trackers;

      std::vector<asc::Module*> to_add; // modules are temporarily held here when added during runtime to avoid invalidating the module_map iterator for the current phase

      void setup(const double dt);

      void init();
      void update();
      void postcalc();
      void check();
      void report();
      void reset();
      void tracker();

      void createFiles(); // create end of simulation run files (i.e. tracked parameter files)

      Phase phase = Phase::setup;

      void propagateStates(); // calls Module propagateStates() methods
      void updateClock();

      bool time_advanced = false; // Whether or not time advanced with the last simulation pass.

      void adaptiveCalc();

      void changeTimeStep();

      std::vector<std::shared_ptr<Module>> to_delete; // modules are temporarily held here from Link<T> so that they can be deleted at the appropriate time
      void recursiveDelete(const size_t n_prev);
      void deleteModules();

      std::unique_ptr<State> integrator;
      Stepper stepper;

      bool tick0 = true; // Very first tick of the simulation, used to avoid overlapping between tickfirst and ticklast tracking calls for additional run() calls.

      double EPS = 1e-8;
      bool tickfirst = true;
      bool ticklast = false;
      double dtp{}; // base time step of run loop
      double dt{}; // time step (can be changed by sampling and events)
      double dt_change{}; // the time step value to be changed to if the change_dt flag is set to true
      bool change_dt = false; // whether or not a change is desired for the time step
      double t{}; // time
      double t1{}; // intended end time of next timestep
      double t_end{}; // end time of this simulation loop
      size_t kpass{}; // internal integrator pass

      bool integrator_initialized = false; // whether or not the integration scheme has been initialized (i.e. for a predictor-corrector or DOPRI45), not used for basic schemes like RK4

      void integrationTolerance(double tolerance); // Set adaptive step size tolerance for all modules in this simulator.

      static std::map<std::string, std::shared_ptr<Module>> tracking; // all trackers for all simulators

      void addStopper(std::shared_ptr<Module>& module)
      {
         std::shared_ptr<Stopper> stopper(new Stopper());
         stopper->addStopper(module);
         stoppers.push_back(stopper);
      }

      template <typename T>
      void addStopper(Link<T>& module)
      {
         std::shared_ptr<Stopper> stopper(new Stopper());
         stopper->addStoppers(module);
         stoppers.push_back(stopper);
      }

      template <typename T, typename... Trest>
      void addStopper(Link<T>& first, Trest&... rest)
      {
         std::shared_ptr<Stopper> stopper(new Stopper());
         stopper->addStoppers(first, rest...);
         stoppers.push_back(stopper);
      }

      void runStoppers();
      std::vector<std::shared_ptr<Stopper>> stoppers;
   };
}
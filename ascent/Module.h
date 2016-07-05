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

#define EIGEN_MPL2_ONLY // Ensure that Eigen license is MPL2 compatible.

#include "ascent/core/LinkBase.h"
#include "ascent/core/State.h"
#include "ascent/core/Type.h"
#include "ascent/core/Vars.h"

#define ascModule(module)\
if (!chai.modules.count(#module)) {\
   chai.add(chaiscript::fun(static_cast<bool (module::*)()>(&module::run)), "run");\
   chai.add(chaiscript::fun(static_cast<bool (module::*)(const double, const double)>(&module::run)), "run");\
   chai.add(chaiscript::base_class<asc::Module, std::decay<decltype(*this)>::type>());\
   chai.modules.insert(#module);\
}

#define ascVar(x) define(#x, x);\
if (!chai.registered(typeid(*this).name(), #x)) {\
   chai.add(chaiscript::fun(static_cast<std::decay<decltype(x)>::type (ascNS::*)>(&ascNS::x)), #x);\
   chai.chai_rg[typeid(*this).name()].push_back(#x);\
}

namespace asc {
   namespace hidden {
      inline void assignModule(asc::LinkBase& link_base, asc::Module& module) { link_base.assign(module); }
      inline void assignLinkBase(asc::LinkBase& lhs, asc::LinkBase& rhs) { lhs.assignLinkBase(rhs); }
   }
}

#define ascLink(x) \
if (!chai.registered(typeid(*this).name(), #x)) {\
   chai.add(chaiscript::fun(static_cast<std::decay<decltype(x)>::type (ascNS::*)>(&ascNS::x)), #x);\
   chai.chai_rg[typeid(*this).name()].push_back(#x);\
}\
if (!chai.links.count(typeid(decltype(x)))) {\
   chai.add(chaiscript::base_class<asc::LinkBase, std::decay<decltype(x)>::type>());\
   chai.links.insert(typeid(decltype(x)));\
}\
defineLink(#x, x);

namespace dynode
{
   class Editor;
   class Plotter;
   class Menu;
}

namespace asc
{
   template <typename T>
   using Unqualified = typename std::remove_cv<T>::type;

   class LinkBase;
   class ModuleCore;

   /** Provides the core functionality of the Ascent architecture.
   * A module in Ascent should always inherit from the Module class.
   */
   class Module : public std::enable_shared_from_this<Module>
   {
      template <typename T>
      friend class Link;

      friend class JsonAPI;
      friend class Simulator;
      friend class Stopper;

      template <typename E>
      friend class HistoryVector;

      friend class dynode::Editor;
      friend class dynode::Plotter;
      friend class dynode::Menu;

      template <typename T>
      friend void integrator(size_t sim);

      friend void integrationTolerance(size_t sim, const double tolerance);

   private:
      Simulator& simulator; // simulator is defined first so that it can be used for other construction components

   public:
      Module(size_t sim);
      virtual ~Module();

      /** Set relative error integration tolerance for this module's states.
      * @param tolerance  The integration tolerance for this module's states. A negative value turns off step resizing for this module's states.
      */
      void integrationTolerance(double tolerance)
      {
         for (State* state : states)
            state->tolerance = tolerance;
      }

      /** Specifies whether the module should be frozen (init(), update(), postcalc(), check(), report(), reset(), and integration (state propagation) will not be called on the module), useful for testing purposes or handling stages. */
      bool frozen = false;

      /** Specifies whether the integration (state propagation) should be frozen for this module. */
      bool freeze_integration = false;

      /** Whether this module wants to stop the simulation, used for building stoppers. */
      bool stop = false;

      /** Puts this module's simulator in an error state, which will shut down the simulator as soon as possible.
      * @param description  Describe the error. Collected errors are retrieved via the getError() method.
      * @return Always returns false.
      */
      bool error(const std::string& description) { return simulator.setError(description); }

      /** Check if the simulator is in an error state and get a vector of recorded error descriptions.
      * @return Returns a pair whose first value is true if the simulator is in an error state. The second value is a vector of error descriptions.
      */
      std::pair<bool, std::vector<std::string>> error() { return std::make_pair(simulator.error, simulator.error_descriptions); }

      /** Set whether or not error messages should be printed to the console. */
      void printErrors(bool b) { simulator.print_errors = b; }

      /** Runs this module's associated simulator.
      * @param dt  The time step of for the simulator.
      * @param t_end  The end time to run the simulator until.
      */
      bool run(const double dt, const double t_end) { return simulator.run(dt, t_end); }

      /** Runs this module's associated simulator at currently set dt and t_end values. */
      bool run() { return simulator.run(); }

      /** The simulator's current time. */
      const double& t;

      /** The simulator's current time step size. */
      const double& dt;

      const double& dt_base;

      /** Change the time step of this module's simulator.
      * @param new_dt  New time step size.
      */
      void dtChange(double new_dt)
      {
         simulator.change_dt = true;
         simulator.dt_change = new_dt;

         if (phase == Phase::setup)
            simulator.changeTimeStep(); // change now
      }

      /** Retrieve a vector of the time history, with the time at each full step recorded. */
      const std::vector<double>& timeHistory() const { return simulator.t_hist; };

      /** Obtain time history of a tracked variable.
      * @param id  The string identification of the variable.
      * @return Returns a pair of time history matched with the variable's state history.
      */
      template <typename T>
      std::deque<T> history(const std::string& id) { return vars.history<T>(id); }

      /** True if at the first update of the current simulation run.
      * True only for the first pass on update() for the current run() call.
      */
      const bool& first_update = simulator.tickfirst;

      /** True if at the first report of the current simulation run. */
      const bool& first_report = simulator.tickfirst;

      /** True if at the final report for the current simulation run. */
      const bool& last_report = simulator.ticklast;

      /** If time advanced in the last simulation pass */
      const bool& time_advanced = simulator.time_advanced;

      /** Specifies that the module within the link container must run before this module.
      * The runBefore method applies to the update() and postcalc() simulation phases.
      * @param link  A Link contained module.
      */
      template <typename T>
      void runBefore(Link<T>& link)
      {
         if (link.module)
            link.module->run_first[module_id] = myself;
         else
            error("Module: Module of type <" + static_cast<std::string>(typeid(T).name()) + "> was not initialized and fails in runBefore().");
      }

      /** A variadic implementation of runBefore(Link<T>& link).
      * @param link  The first module that must run before this module.
      * @param rest  Any number of modules that must be run before this module.
      * Example:
      * @code
      *     runBefore(spring0, damper1, spring4);
      * @endcode
      * Within the update() and postcalc() phases, this Module will run before the spring0, damper1, and spring4 update() and postcalc() methods.
      */
      template <typename T, typename... Trest>
      void runBefore(Link<T>& link, Trest&... rest)
      {
         runBefore(link);
         runBefore(rest...);
      }

      /** Specifies that the module within the link container must run before this module.
      * The runBefore method applies to the update() and postcalc() simulation phases.
      */
      void runBefore(Module& module)
      {
         module.run_first[module_id] = myself;
      }

      /** A variadic implementation of runBefore(Module& module).
      * @param module  The first module that must run before this module.
      * @param rest  Any number of modules that must be run before this module.
      */
      template <typename... Trest>
      void runBefore(Module& module, Trest&... rest)
      {
         runBefore(module);
         runBefore(rest...);
      }

      /** Access this module's name, which is printed with output data. */
      std::string name() const;

      /** Define this module for external access.
      * @param name  Used in output files if this module is used for tracking.
      */
      template <typename T>
      bool name(const std::string& name)
      {
         module_name = name;

         if (external.count(name) > 0)
            return error("Module::name(const std::string& name): " + name + " was already defined");

         chai.add(chaiscript::var(std::ref(*static_cast<T*>(this))), name);

         external[name] = this;
         return true;
      }

      /** Sets a new default directory for output files. */
      void directory(const std::string& directory) { module_directory = directory; }

      /** This Module's simulator identification. */
      const size_t sim;

      /** This Module's unique identification across all simulators. */
      const size_t module_id;

      /** Generate a Link<T> container from this class; similar to std::shared_from_this(). */
      template <typename T>
      Link<T> linkFromThis()
      {
         Link<T> link;
         link.assign(*this);
         return link;
      }

      /** Check if a variable in a module is setup for tracking.
      * @param module_id  Module ID for the module that contains the variable of interest.
      * @param var_name  Associated variable name.
      * @return Returns true if the variable was setup for tracking.
      */
      bool trackable(const std::string& var_name) { return vars.trackable(var_name); }

      /** Specify a variable to be tracked.
      * @param var_name  Associated variable name.
      */
      void track(const std::string& var_name);

      /** Specify a variable to be tracked.
      * @param module  An uncontained Module.
      * @param var_name  Associated variable name.
      */
      void track(Module& module, const std::string& var_name);

      /** Specify a variable in a Link contained Module to be tracked.
      * @param link  Link contained Module.
      * @param var_name  Associated variable name.
      */
      template <typename T>
      void track(Link<T>& link, const std::string& var_name)
      {
         if ("t" == var_name)
         {
            print_time = true;
            simulator.track_time = true;
         }
         else
         {
            tracking.push_back(std::make_pair(link.module->module_id, var_name));
            link.module->steps(var_name);
         }
      }

      /** Create output file for this Module's tracked parameters. */
      void outputTrack();

      /** Change output file type to .txt instead of .csv */
      void txtFiles() { file_type = ".txt"; }

      /** Generate a manipulator module whose memory is owned by this module as long as the manipulator isn't also stored elsewhere (if Link<T> isn't saved).
      * Manipulators should usually only mess with parameters from this module.
      * Manipulators are ordered via the runBefore method, so they always run before the module they are manipulating.
      */
      template <typename T, typename... Types>
      asc::Link<T> addManipulator(Types&&... args)
      {
         std::shared_ptr<Module> ptr(new T(std::forward<Types>(args)...));
         ptr->runBefore(*this); // run the manipulator before this module
         manipulators.emplace_back(ptr);
         return manipulators.back()->linkFromThis<T>();
      }

      /** ChaiScript interface, unique to the simulator that this module belongs to. */
      ChaiEngine& chai;

      const Phase& phase{ simulator.phase };
      const double& t_end{ simulator.t_end };

      void endTime(double new_t_end)
      {
         if (phase == Phase::setup)
            simulator.t_end = new_t_end;
         else
         {
            simulator.change_t_end = true;
            simulator.t_end = new_t_end;
         }
      }

      void manipulatorReport()
      {
         for (auto & manipulator : manipulators)
            manipulator->report();
      }

      std::vector<std::pair<std::string, std::string>> varNames()
      {
         return vars.getNames();
      }

      virtual std::string type()
      {
         return Type<Module>::name();
      }

   protected:
      /** Stops this Module's simulator at the end the current integration step.
      * @param b  Allows the command to stop the simulation to be turned on and off, so module's can compete and ordering can decide ending the simulation.
      */
      void stopSimulation(bool b = true) { simulator.stop_simulation = b; }

      /** Add a state and its derivative to be integrated.
      * @param x  State.
      * @param xd  State derivative.
      * @param tolerance  The integration tolerance for this state. Only applicable when using an adaptively stepping integration method.
      * The default negative tolerance means that this state will not be considered for adaptive stepping, even if an adaptive solver is used.
      */
      void addIntegrator(double &x, double &xd, const double tolerance = -1.0);

      /** Add a std::vector, std::deque, Eigen::Vector3d, etc. to be integrated.
      * @param x  State vector.
      * @param xd  State derivatives vector.
      * @param tolerance  The integration tolerance for these states. Only applicable when using an adaptively stepping integration method.
      * The default negative tolerance means that this state will not be considered for adaptive stepping, even if an adaptive solver is used.
      */
      template <typename T>
      void addIntegrator(T &x, T &xd, const double tolerance = -1.0)
      {
         for (decltype(x.size()) i = 0; i < x.size(); ++i)
            addIntegrator(x[i], xd[i], tolerance);
      }

      /** For initialization computations. */
      virtual void init() {}

      /** Called for every kpass internal step (for example: called four times for a 4th order Runge Kutta integrator). */
      virtual void update() { simulator.updates.erase(module_id); }

      /** Runs once per full integration step and after propagate states for computations based on updated states, runs before check() and report(). */
      virtual void postcalc() { simulator.postcalcs.erase(module_id); }

      /** Called at the end of every full time step before report() in order to check whether the simulation should be stopped (set stoppers here). */
      virtual void check() { simulator.checks.erase(module_id); } // 

      /** End of full time step report (for a 4th order Runge Kutta, report() is called once after four update() calls). */
      virtual void report() { simulator.reports.erase(module_id); }

      /** Used to reset Module parameters after update() and report() have been called (runs for internal kpass steps as well). */
      virtual void reset() { simulator.resets.erase(module_id); }

      void addPhases();

      /** Enables a variable in this Module to be tracked and/or externally accessed and set.
      * @param id  The string identification used to access the variable.
      * @param x  Variable to be tracked via a pointer. Hence the variable's memory should be owned by this class.
      * @param infinite  Set infinite to true in order to record history of the variable indefinitely.
      */
      template <typename T>
      T& define(const std::string& id, T &x, bool infinite = false)
      {
         if (!simulator.trackers.count(module_id))
            simulator.trackers[module_id] = this;

         vars.init(id, x, 0);
         vars.steps(id, infinite);
         return x;
      }

      /** Set the number of time steps to keep track of for a tracked variable.
      * @param id  The string identification of the variable.
      * @param steps  The number of steps to keep track of.
      */
      void steps(const std::string& id, size_t steps) { vars.steps(id, steps); }

      /** Set a variable to be tracked indefinitely; in other words, to have an infinite number of steps recorded.
      * @param id  The string identification of the variable.
      * @param infinite  Whether or not to have an infinite number of steps tracked.
      */
      void steps(const std::string& id, bool infinite = true) { vars.steps(id, infinite); }

      /** Discrete sampling per time step.
      * @return Returns true if at the first pass of the integration method.
      */
      bool sample() const { return simulator.sample(); }

      /** Discrete sampling with a specified sampling rate.
      * The simulator will be stepped to exact multiples of the sampling rate.
      * @param sdt  The sampling rate. This number doesn't need to be fixed and is allowed to be changed during the simulation.
      * @return Returns true if at the first pass of the integration method at the specified sampling time.
      */
      bool sample(double sdt) { return simulator.sample(sdt); }

      /** Discrete run time event.
      * @param t_event  The time of the desired event.
      * @return Returns true if at the first pass of the integration method and at the specified event time (t_event).
      */
      bool event(double t_event) { return simulator.event(t_event); }

      /** Add an uncontained module as a stopper.
      * Uncontained modules must be added one at a time.
      */
      void addStopper(Module& module)
      {
         simulator.addStopper(module.myself);
      }

      /** Add a Link contained module as a stopper. */
      template <typename T>
      void addStopper(Link<T>& module)
      {
         simulator.addStopper(module);
      }

      /** Add a collection of Link contained module as a cummulative stopper. */
      template <typename T, typename... Trest>
      void addStopper(Link<T>& first, Trest&... rest)
      {
         simulator.addStopper(first, rest...);
      }

      template <typename T>
      Link<T>& defineLink(const std::string& id, Link<T>& x)
      {
         LinkBase& link_base = x;
         links.emplace(std::pair<std::string, LinkBase*>(id, &link_base));
         return x;
      }

      template <typename T>
      const Link<T>& defineLink(const std::string& id, const Link<T>& x)
      {
         LinkBase& link_base = const_cast<Link<T>&>(x);
         links.emplace(std::pair<std::string, LinkBase*>(id, &link_base));
         return x;
      }

   private:
      std::shared_ptr<Module> myself; // myself: this Module with a null deleter, only used for module connections so that weak_ptr can be used

      void callInit();
      void callUpdate();
      void callPostCalc();
      void callCheck();
      void callReport();
      void callReset();

      void tracker() { vars.update(); }

      mutable std::string module_name = "";

      std::string module_directory = ""; // The directory to where output files will be written.
      static std::string file_type;

      static size_t next_module_id; // module id across all simulators

      bool init_called = false;
      bool update_called = false;
      bool postcalc_called = false;
      bool check_called = false;
      bool report_called = false;
      bool reset_called = false;

      bool init_run = false;
      bool update_run = false;
      bool postcalc_run = false;
      bool check_run = false;
      bool report_run = false;
      bool reset_run = false;

      std::map<size_t, std::weak_ptr<Module>> run_first; // other modules that must be run before this module is updated

      Vars vars; // contains variable access for the module by string

      std::vector<State*> states; // Must be owned by this module. (i.e. addIntegrator should only be called on this module's variables)
      void propagateStates();

      // manipulators contains modules whose lifetime is to be maintained by this module, and whose modules shouldn't be accessed by other modules.
      std::vector<std::shared_ptr<Module>> manipulators; // Uses std::shared_ptr rather than std::unique_ptr because of std::weak_ptr use for ordering (runBefore()).

      // Tracking
      std::unordered_set<std::string> local_tracking; // Local variables to this module that are being tracked.
      std::vector<std::pair<size_t, std::string>> tracking; // Vector of pairs of module IDs and their associated variables to be tracked.
      bool print_time = false; // Whether or not to print the simulation time as well.

      std::map<std::string, Module*>& external; // Reference to ModuleCore external map, needed here for templated name function.
      static Simulator& getSimulator(const size_t sim); // Needed to avoid publically exposing ModuleCore, used in templated integrator(size_t sim).

      std::map<std::string, LinkBase*> links; // Links belonging to this module. Do Not Delete. std::string is the name associated with the link.

      /** Create a string in csv format for tracked data. */
      std::string csvTrack();

      template <typename T>
      void streamTrack(T& stream)
      {
         size_t id = tracking.front().first;
         std::string var_name = tracking.front().second;

         size_t length;
         if (id == module_id)
            length = vars.length(var_name);
         else
            length = ModuleCore::getModule(id).vars.length(var_name);

         if (print_time)
            stream << "t" << ",";

         size_t n = tracking.size();
         for (size_t i = 0; i < n; ++i)
         {
            auto& p = tracking[i];
            if (i == n - 1) // last parameter
               stream << ModuleCore::getModule(p.first).name() << " " << p.second;
            else
               stream << ModuleCore::getModule(p.first).name() << " " << p.second << ",";
         }

         stream << '\n';

         for (size_t i = 0; i < length; ++i)
         {
            if (print_time)
               stream << simulator.t_hist[i] << ",";

            n = tracking.size();
            for (size_t j = 0; j < n; ++j)
            {
               auto& p = tracking[j];
               stream << ModuleCore::getModule(p.first).vars.print(p.second, i);
               if (j < n - 1) // not the last parameter
                  stream << ",";
            }

            stream << '\n';
         }
      }

      /** For Node Editor GUI */
      Eigen::Vector2d gui_pos{ 0.0, 0.0 }, gui_size{ 10.0, 10.0 };
   };

   /** Set the integrator for the simulator whose number is input.
   * @param sim  The simulator number.
   */
   template <typename T>
   inline void integrator(size_t sim)
   {
      Simulator& s = Module::getSimulator(sim);
      if (s.propagate.size() > 0)
         s.setError("States have already been set for integration. The integrator cannot be changed.");
      else
         s.integrator = std::make_unique<T>(s.stepper);
   }

   /** Set the relative error integration tolerance for the entire simulator associated with this module.
   * @param sim  The simulator number.
   * @param tolerance  The integration tolerance. If negative, it will turn off step resizing for all states in this module's simulator.
   */
   inline void integrationTolerance(size_t sim, const double tolerance);
}
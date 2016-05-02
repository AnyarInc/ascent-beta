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

namespace asc
{
   class State
   {
   public:
      State(double &x, double &xd) : x(x), xd(xd) {}
      virtual ~State() {}

      virtual State* factory(double &x, double &xd) = 0;

      virtual void propagate() = 0;
      virtual void updateClock() = 0;
      virtual double optimalTimeStep() = 0;
      virtual bool adaptive() { return false; } // Whether this is an adaptive integrator (NOT FSAL), like Dormand Prince 87 (DOPRI87).
      virtual bool adaptiveFSAL() { return false; } // Whether this is a First Same As Last (FSAL) adaptive integration scheme (i.e. Dormand Prince 45 (DOPRI45)).

      double &x, &xd; // xd is the derivative of x
      double tolerance; // allows adaptive step size tolerance to be set uniquely for every state
   };
}
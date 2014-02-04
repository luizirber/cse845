//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "resource_count.hh"

#include <math.h>
#include <iostream.h>

const double cResourceCount::UPDATE_STEP(1.0 / 10000.0);
const int cResourceCount::PRECALC_DISTANCE(100);

cResourceCount::cResourceCount(int num_resources)
  : resource_count(num_resources)
  , decay_rate(num_resources)
  , inflow_rate(num_resources)
  , decay_precalc(num_resources, PRECALC_DISTANCE+1)
  , inflow_precalc(num_resources, PRECALC_DISTANCE+1)
  , update_time(0.0)
{
  resource_count.SetAll(0.0);
  decay_rate.SetAll(0.0);
  inflow_rate.SetAll(0.0);
  decay_precalc.SetAll(0.0);
  inflow_precalc.SetAll(0.0);
}

cResourceCount::~cResourceCount()
{
}

void cResourceCount::Setup(int id, double initial, double inflow, double decay)
{
  assert(id >= 0 && id < resource_count.GetSize());
  assert(initial >= 0.0);
  assert(decay >= 0.0);
  assert(inflow >= 0.0);

  cerr << "Setting up resource " << id
       << " with initial quatity=" << initial
       << ", decay=" << decay
       << ", and inflow = " << inflow
       << endl;

  resource_count[id] = initial;
  decay_rate[id] = decay;
  inflow_rate[id] = inflow;

  double step_decay = pow(decay, UPDATE_STEP);
  double step_inflow = inflow * UPDATE_STEP;

  decay_precalc(id, 0) = 1.0;
  inflow_precalc(id, 0) = 0.0;
  for (int i = 1; i <= PRECALC_DISTANCE; i++) {
    decay_precalc(id, i)  = decay_precalc(id, i-1) * step_decay;
    inflow_precalc(id, i) = inflow_precalc(id, i-1) * step_decay + step_inflow;
  }
}


const tArray<double> & cResourceCount::GetResources() const
{
  DoUpdates();
  return resource_count;
}

void cResourceCount::Modify(const tArray<double> & res_change)
{
  assert(resource_count.GetSize() == res_change.GetSize());

  for (int i = 0; i < resource_count.GetSize(); i++) {
    resource_count[i] += res_change[i];
    assert(resource_count[i] >= 0.0);
  }
}


void cResourceCount::Modify(int id, double change)
{
  assert(id < resource_count.GetSize());

  resource_count[id] += change;
  assert(resource_count[id] >= 0.0);
}


///// Private Methods /////////

void cResourceCount::DoUpdates() const
{ 
  assert(update_time >= 0);

  // Determine how many update steps have progressed
  int num_steps = (int) (update_time / UPDATE_STEP);

  // Preserve remainder of update_time
  update_time -= num_steps * UPDATE_STEP;


  while (num_steps > PRECALC_DISTANCE) {
    for (int i = 0; i < resource_count.GetSize(); i++) {
      resource_count[i] *= decay_precalc(i, PRECALC_DISTANCE);
      resource_count[i] += inflow_precalc(i, PRECALC_DISTANCE);
    }
    num_steps -= PRECALC_DISTANCE;
  }

  for (int i = 0; i < resource_count.GetSize(); i++) {
    resource_count[i] *= decay_precalc(i, num_steps);
    resource_count[i] += inflow_precalc(i, num_steps);
  }
}

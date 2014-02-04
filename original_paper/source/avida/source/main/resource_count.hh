//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_COUNT_HH
#define RESOURCE_COUNT_HH

#include "../tools/tArray.hh"
#include "../tools/tMatrix.hh"

class cResourceCount {
private:
  mutable tArray<double> resource_count;  // Current quantity of each resource
  tArray<double> decay_rate;      // Multiplies resource count at each step
  tArray<double> inflow_rate;     // An increment for resource at each step
  tMatrix<double> decay_precalc;  // Precalculation of decay values
  tMatrix<double> inflow_precalc; // Precalculation of inflow values

  // Setup the update process to use lazy evaluation...
  mutable double update_time;     // Portion of an update compleated...
  void DoUpdates() const;         // Update resource count based on update time

  // A few constants to describe update process...
  static const double UPDATE_STEP;   // Fraction of an update per step
  static const int PRECALC_DISTANCE; // Number of steps to precalculate
public:
  cResourceCount(int num_resources);
  ~cResourceCount();

  void Setup(int id, double initial, double inflow, double decay);
  void Update(double in_time) { update_time += in_time; }

  const tArray<double> & GetResources() const;
  void Modify(const tArray<double> & res_change);
  void Modify(int id, double change);
};

#endif

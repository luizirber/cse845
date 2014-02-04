//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "mutations.hh"

#include "../tools/tools.hh"

cMutationRates::cMutationRates()
  : point_mut_prob(0.0),
    copy_mut_prob(0.0),
    ins_mut_prob(0.0),
    del_mut_prob(0.0),
    divide_mut_prob(0.0),
    divide_ins_prob(0.0),
    divide_del_prob(0.0),
    parent_mut_prob(0.0),
    crossover_prob(0.0),
    aligned_cross_prob(0.0),
    exe_err_prob(0.0)
{
}

cMutationRates::~cMutationRates()
{
}

void cMutationRates::Clear()
{
  point_mut_prob = 0.0;
  copy_mut_prob = 0.0;
  ins_mut_prob = 0.0;
  del_mut_prob = 0.0;
  divide_mut_prob = 0.0;
  divide_ins_prob = 0.0;
  divide_del_prob = 0.0;
  parent_mut_prob = 0.0;
  crossover_prob = 0.0;
  aligned_cross_prob = 0.0;
  exe_err_prob = 0.0;
}

bool cMutationRates::TestCopyMut() const
{
  return g_random.P(copy_mut_prob);
}

bool cMutationRates::TestPointMut() const
{
  return g_random.P(point_mut_prob);
}

bool cMutationRates::TestDivideMut() const
{
  return g_random.P(divide_mut_prob);
}

bool cMutationRates::TestDivideIns() const
{
  return g_random.P(divide_ins_prob);
}

bool cMutationRates::TestDivideDel() const
{
  return g_random.P(divide_del_prob);
}

bool cMutationRates::TestParentMut() const
{
  return g_random.P(parent_mut_prob);
}

bool cMutationRates::TestCrossover() const
{
  return g_random.P(crossover_prob);
}

bool cMutationRates::TestAlignedCrossover() const
{
  return g_random.P(aligned_cross_prob);
}

bool cMutationRates::TestExeErr() const
{
  return g_random.P(exe_err_prob);
}



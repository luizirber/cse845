//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "inst_lib.hh"

#include "../tools/string_util.hh"

//////////////////////
//  cInstLib
//////////////////////

// Initialize static variables
const cInstruction cInstLib::inst_default(   0 );
const cInstruction cInstLib::inst_error  ( 255 );

cInstLib::cInstLib()
{
}

cInstLib::cInstLib(const cInstLib & in_inst_lib)
  : inst_array(in_inst_lib.inst_array)
  , nop_mods(in_inst_lib.nop_mods)
  , mutation_chart(in_inst_lib.mutation_chart)
{
}

cInstLib::~cInstLib()
{
}

cInstLib & cInstLib::operator=(const cInstLib & _in)
{
  inst_array = _in.inst_array;
  nop_mods = _in.nop_mods;
  mutation_chart = _in.mutation_chart;
  return *this;
}

bool cInstLib::OK() const
{
  // assert(inst_array.OK());
  // assert(nop_mods.OK());
  assert(inst_array.GetSize() < 256);
  assert(nop_mods.GetSize() < inst_array.GetSize());

  // Make sure that all of the redundancies are represented the appropriate
  // number of times.
  tArray<int> test_redundancy(inst_array.GetSize());
  test_redundancy.SetAll(0);
  for (int i = 0; i < mutation_chart.GetSize(); i++) {
    int test_id = mutation_chart[i];
    test_redundancy[test_id]++;
  }
  for (int i = 0; i < inst_array.GetSize(); i++) {
    assert(inst_array[i].redundancy == test_redundancy[i]);
  }

  return true;
}

cInstruction cInstLib::GetRandomInst() const
{
  int inst_op = mutation_chart[g_random.GetUInt(mutation_chart.GetSize())];
  return cInstruction(inst_op);
}

int cInstLib::Add(const cString & _name, tHardwareMethod _fun,
		  const int redundancy, const int ft_cost, const int cost,
		  const double prob_fail)
{
  const int inst_id = inst_array.GetSize();

  assert(inst_id < 255);

  // Increase the size of the array...
  inst_array.Resize(inst_id + 1);

  // Setup the new function...
  inst_array[inst_id].name = _name;
  inst_array[inst_id].function = _fun;
  inst_array[inst_id].redundancy = redundancy;
  inst_array[inst_id].cost = cost;
  inst_array[inst_id].ft_cost = ft_cost;
  inst_array[inst_id].prob_fail = prob_fail;

  const int total_redundancy = mutation_chart.GetSize();
  mutation_chart.Resize(total_redundancy + redundancy);
  for (int i = 0; i < redundancy; i++) {
    mutation_chart[total_redundancy + i] = inst_id;
  }

  return inst_id;
}

int cInstLib::AddNop(const cString & _name, tHardwareMethod _fun,
		     const int reg, const int redundancy, const int ft_cost,
		     const int cost, const double prob_fail)
{ 
  // Assert nops are at the _beginning_ of an inst_set.
  assert(inst_array.GetSize() == nop_mods.GetSize());

  const int inst_id = Add(_name, _fun, redundancy, ft_cost, cost, prob_fail);

  nop_mods.Resize(inst_id + 1);
  nop_mods[inst_id] = reg;

  return inst_id;
}

cString cInstLib::FindBestMatch(const cString & in_name) const
{
  int best_dist = 1024;
  cString best_name("");
  
  for (int i = 0; i < inst_array.GetSize(); i++) {
    const cString & cur_name = inst_array[i].name;
    const int cur_dist = cStringUtil::EditDistance(cur_name, in_name);
    if (cur_dist < best_dist) {
      best_dist = cur_dist;
      best_name = cur_name;
    }
    if (cur_dist == 0) break;
  }

  return best_name;
}


//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "population_cell.hh"

#include "../tools/tools.hh"

#include "organism.hh"

cPopulationCell::cPopulationCell()
  : organism(NULL)
  , cur_input(0)
  , organism_count(0)
{
}

void cPopulationCell::Setup(int in_id, const cMutationRates & in_rates)
{
  cell_id = in_id;
  mutation_rates = in_rates;
}

void cPopulationCell::Rotate(cPopulationCell & new_facing)
{
  // @CAO Note, this breaks avida if new_facing is not in connection_list

#ifdef DEBUG
  int scan_count = 0;
#endif
  while (connection_list.GetFirst() != &new_facing) {
    connection_list.CircNext();
    assert(++scan_count < connection_list.GetSize());
  }
}


void cPopulationCell::RandomizeInputs()
{
  // Set the top 8 bits of the input buffer...
  input_array[0] = 15 << 24;  // 00001111
  input_array[1] = 51 << 24;  // 00110011
  input_array[2] = 85 << 24;  // 01010101

  // And randomize the rest...
  for (int i = 0; i < IO_SIZE; i++) {
    input_array[i] += g_random.GetUInt(1 << 24);
  }
}

int cPopulationCell::GetInput()
{
  if (cur_input >= IO_SIZE) cur_input = 0;
  return input_array[cur_input++];
}


int cPopulationCell::GetInput(int id)
{
  assert(id >= 0 && id < IO_SIZE);
  return input_array[id];
}


void cPopulationCell::InsertOrganism(cOrganism & new_org)
{
  assert(&new_org != NULL);
  assert(new_org.GetGenotype() != NULL);
  assert(organism == NULL);

  // Adjust this cell's attributes to account for the new organism.
  organism = &new_org;
  organism_count++;

  // Adjust the organism's attributes to match this cell.
  organism->MutationRates() = mutation_rates;
  organism->PopInterface().SetCellID(cell_id);
}

cOrganism * cPopulationCell::RemoveOrganism()
{
  if (organism == NULL) return NULL;   // Nothing to do!

  // For the moment, the cell doesn't keep track of much...
  cOrganism * out_organism = organism;
  organism = NULL;
  return out_organism;
}


bool cPopulationCell::OK()
{
  // Nothing for the moment...
  return true;
}

bool cPopulationCell::SaveState(ofstream & fp)
{
  // Nothing for the moment...
  return false;
}


bool cPopulationCell::LoadState(ifstream & fp)
{
  // Nothing for the moment...
  return false;
}

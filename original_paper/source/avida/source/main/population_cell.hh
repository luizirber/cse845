//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_CELL_HH
#define POPULATION_CELL_HH

#include <fstream.h>

#include "../tools/tList.hh"

#include "mutations.hh"
#include "../cpu/cpu_defs.hh"

class cOrganism;
class cPopulation;

class cPopulationCell {
  friend class cPopulation;
private:
  cOrganism * organism;                    // The occupent of this cell.
  tList<cPopulationCell> connection_list;  // A list of neighboring cells.
  cMutationRates mutation_rates;           // Mutation rates at this cell.
  int input_array[IO_SIZE];                // Environmental Inputs...
  int cur_input;                           // Next input to give organism.

  int cell_id;           // Unique id for position of cell in population.
  int organism_count;    // Total number of orgs to ever inhabit this cell.

private:  // Organism changes should only occur through population...
  void InsertOrganism(cOrganism & new_org);
  cOrganism * RemoveOrganism();

public:
  cPopulationCell();
  ~cPopulationCell() { ; }

  void Setup(int in_id, const cMutationRates & in_rates);
  void Rotate(cPopulationCell & new_facing);
  void RandomizeInputs();

  cOrganism * GetOrganism() const { return organism; }
  tList<cPopulationCell> & ConnectionList() { return connection_list; }
  const cMutationRates & MutationRates() const { return mutation_rates; }
  cMutationRates & MutationRates() { return mutation_rates; }
  int GetInput();
  int GetInput(int id);

  int GetID() const { return cell_id; }
  int GetOrganismCount() const { return organism_count; }

  bool IsOccupied() const { return organism != NULL; }

  bool OK();

  bool SaveState(ofstream & fp);
  bool LoadState(ifstream & fp);
};

#endif

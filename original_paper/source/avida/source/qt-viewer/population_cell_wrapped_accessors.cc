//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#include "messaging_population_wrapper.hh"
#include "population_cell_wrapper.hh"
#include "population_cell.hh"
#include "population.hh"
#include "organism.hh"

#include "population_cell_wrapped_accessors.hh"


cPopulationCell *GetPopulationCell(
  MessagingPopulationWrapper *pop_wrapper,
  int cell_id
){
  if(pop_wrapper == NULL) return NULL;
  cPopulation *population = pop_wrapper->getPopulation();
  if(population == NULL) return NULL;
  return &population->GetCell(cell_id);
}

cPopulationCell *GetPopulationCell(PopulationCellWrapper *cell_wrapper){
  if(cell_wrapper == NULL) return NULL;
  else return cell_wrapper->getPopulationCell();
}

cOrganism *GetOrganism(cPopulationCell *cell){
  if(cell == NULL) return NULL;
  else return cell->GetOrganism();
}

cGenotype *GetGenotype(cOrganism *organism){
  if(organism == NULL) return NULL;
  else return organism->GetGenotype();
}

cMutationRates *GetMutationRates(cPopulationCell *cell){
  if(cell == NULL) return NULL;
  else return &cell->MutationRates();
}

cPhenotype *GetPhenotype(cOrganism *organism){
  if(organism == NULL) return NULL;
  else return &organism->GetPhenotype();
}

cHardwareCPU *GetHardwareCPU(cOrganism *organism){
  if(organism == NULL) return NULL;
  else return (cHardwareCPU *) &organism->GetHardware();
}

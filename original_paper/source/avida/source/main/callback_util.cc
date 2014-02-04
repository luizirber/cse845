//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2002 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "callback_util.hh"

#include "avida.hh"
#include "birth_chamber.hh"
#include "genotype.hh"
#include "organism.hh"
#include "population.hh"
#include "population_cell.hh"
#include "pop_interface.hh"

#include "../cpu/hardware_base.hh"
#include "../cpu/hardware_factory.hh"
#include "../cpu/test_cpu.hh"


cHardwareBase * cCallbackUtil::CB_NewHardware(cPopulation * pop,
					      cOrganism * owner)
{
  cInstLib * inst_lib;

  // Determine which inst_lib we should be using...
  // If we're in a population, setup the inst_lib properly.
  if (pop != NULL) inst_lib = &(pop->GetEnvironment().GetInstLib());

  // Otherwise, use the test inst lib...
  else inst_lib = cTestCPU::GetInstLib();

  // For the moment, we only have one hardware type...
  assert(inst_lib != NULL && inst_lib->OK());
  cHardwareBase * new_hardware = cHardwareFactory::Create(owner, inst_lib);

  return new_hardware;
}


void cCallbackUtil::CB_RecycleHardware(cHardwareBase * out_hardware)
{
  cHardwareFactory::Recycle(out_hardware);
}


// Callback for a divide inside of a population
bool cCallbackUtil::CB_Divide(cPopulation * pop, int cell_id,
			      cOrganism * parent, cOrganism * child)
{
  assert(pop != NULL);
  assert(parent != NULL);
  assert(child != NULL);
  assert(pop->GetCell(cell_id).GetOrganism() == parent);

  pop->ActivateOffspring(child, *parent);
  return true;
}


// Callback for a divide outside of a population
bool cCallbackUtil::CB_TestDivide(cPopulation * pop, int cell_id,
				  cOrganism * parent, cOrganism * child)
{
  assert(pop == NULL);  // Must not be in a population...

  // Reset the parent from the divide
  parent->GetPhenotype().TestDivideReset(parent->GetGenome().GetSize());

  // Dispose of the child.
  // @CAO in the future, we probably want to bass this to the test_cpu!
  delete child;

  return true;
}


cOrganism * cCallbackUtil::CB_GetNeighbor(cPopulation * pop, int cell_id)
{
  assert(pop != NULL);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  return cell.ConnectionList().GetFirst()->GetOrganism();
}


cBirthChamber & cCallbackUtil::CB_BirthChamber(cPopulation * pop, int cell_id)
{
  assert(pop != NULL);
  assert(pop->GetCell(cell_id).IsOccupied());

  return pop->GetBirthChamber(cell_id);
}

cBirthChamber & cCallbackUtil::CB_TestBirthChamber(cPopulation *pop, int cell_id)
{
  assert(pop == NULL);  // Must not be in a population...
  return cBirthChamber::GetDefaultChamber();
}


int cCallbackUtil::CB_GetNumNeighbors(cPopulation * pop, int cell_id)
{
  assert(pop != NULL);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  return cell.ConnectionList().GetSize();
}


void cCallbackUtil::CB_Rotate(cPopulation * pop, int cell_id,
				    int direction)
{
  assert(pop != NULL);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  if (direction >= 0) cell.ConnectionList().CircNext();
  else cell.ConnectionList().CircPrev();
}


void cCallbackUtil::CB_Breakpoint()
{
  cAvidaDriver_Base::main_driver->SignalBreakpoint();
}


double cCallbackUtil::CB_TestFitness(cPopulation * pop, int cell_id)
{
  assert(pop != NULL);
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());

  return cell.GetOrganism()->GetGenotype()->GetTestFitness();
}


int cCallbackUtil::CB_GetInput(cPopulation * pop, int cell_id)
{
  if (pop == NULL) return cTestCPU::GetInput();
  cPopulationCell & cell = pop->GetCell(cell_id);
  assert(cell.IsOccupied());
  return cell.GetInput();
}


const tArray<double> &
cCallbackUtil::CB_GetResources(cPopulation * pop, int cell_id)
{
  if (pop == NULL) return cTestCPU::GetResources();

  // @CAO for the moment, ignore the possibility of local resources...
  return pop->GetResources();
}


void cCallbackUtil::CB_UpdateResources(cPopulation * pop, int cell_id,
				       const tArray<double> & res_change)
{
  if (pop == NULL) return cTestCPU::UpdateResources(res_change);

  // @CAO for the moment, ignore the possibility of local resources...
  return pop->UpdateResources(res_change);
}


void cCallbackUtil::CB_KillCell(cPopulation * pop, int death_id)
{
  assert(pop != NULL);
  cPopulationCell & death_cell = pop->GetCell(death_id);
  pop->KillOrganism(death_cell);
}

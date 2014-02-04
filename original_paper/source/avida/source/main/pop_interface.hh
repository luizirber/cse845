//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

// This class is used by organisms to interface with their population.

#ifndef POPULATION_INTERFACE_HH
#define POPULATION_INTERFACE_HH

#include "../tools/tArray.hh"

class cBirthChamber;
class cOrganism;
class cPopulation;
class cHardwareBase;

typedef cHardwareBase * (*tFunNewHardware)(cPopulation *pop, cOrganism *owner);
typedef void (*tFunRecycle)(cHardwareBase * out_hardware);
typedef bool (*tFunDivide)
  (cPopulation * pop, int cell_id, cOrganism * parent, cOrganism * child);
typedef cOrganism * (*tFunGetNeighbor)(cPopulation * pop, int cell_id);
typedef cBirthChamber & (*tFunBirthChamber)(cPopulation * pop, int cell_id);
typedef int (*tFunNumNeighbors)(cPopulation * pop, int cell_id);
typedef void (*tFunRotate)(cPopulation * pop, int cell_id, int direction);
typedef void (*tFunBreakpoint)();
typedef double (*tFunTestFitness)(cPopulation * pop, int cell_id);
typedef int (*tFunGetInput)(cPopulation * pop, int cell_id);
typedef const tArray<double> & (*tFunGetResources)
  (cPopulation * pop, int cell_id);
typedef void (*tFunUpdateResources)
  (cPopulation * pop, int cell_id, const tArray<double> & res_change);
typedef void (*tFunKillCell)(cPopulation * pop, int death_id);

class cPopulationInterface {
private:
  cPopulation * population;
  int cell_id;

  tFunNewHardware      fun_new_hardware;
  tFunRecycle          fun_recycle;
  tFunDivide           fun_divide;
  tFunGetNeighbor      fun_get_neighbor;
  tFunBirthChamber     fun_birth_chamber;
  tFunNumNeighbors     fun_num_neighbors;
  tFunRotate           fun_rotate;
  tFunBreakpoint       fun_breakpoint;
  tFunTestFitness      fun_test_fitness;
  tFunGetInput         fun_get_input;
  tFunGetResources     fun_get_resources;
  tFunUpdateResources  fun_update_resources;
  tFunKillCell         fun_kill_cell;
public:
  cPopulationInterface();
  ~cPopulationInterface();

  cPopulation * GetPopulation() { return population; }
  void SetPopulation(cPopulation * pop) { population = pop; }

  int GetCellID() { return cell_id; }
  void SetCellID(int in_id) { cell_id = in_id; }

  void SetFun_NewHardware(tFunNewHardware fun) { fun_new_hardware = fun; }
  void SetFun_Recycle(tFunRecycle fun) { fun_recycle = fun; }
  void SetFun_Divide(tFunDivide fun) { fun_divide = fun; }
  void SetFun_GetNeighbor(tFunGetNeighbor fun) { fun_get_neighbor = fun; }
  void SetFun_BirthChamber(tFunBirthChamber fun) { fun_birth_chamber = fun; }
  void SetFun_NumNeighbors(tFunNumNeighbors fun) { fun_num_neighbors = fun; }
  void SetFun_Rotate(tFunRotate fun) { fun_rotate = fun; }
  void SetFun_Breakpoint(tFunBreakpoint fun) { fun_breakpoint = fun; }
  void SetFun_TestFitness(tFunTestFitness fun) { fun_test_fitness = fun; }
  void SetFun_GetInput(tFunGetInput fun) { fun_get_input = fun; }
  void SetFun_GetResources(tFunGetResources fun) { fun_get_resources = fun; }
  void SetFun_UpdateResources(tFunUpdateResources fun)
    { fun_update_resources = fun; }
  void SetFun_KillCell(tFunKillCell fun) { fun_kill_cell = fun; }
  void CopyCallbacks(cPopulationInterface & in_interface);

  // Activate callbacks...
  cHardwareBase * NewHardware(cOrganism * owner);
  void RecycleHardware(cHardwareBase * out_hardware);
  bool Divide(cOrganism * parent, cOrganism * child);
  cOrganism * GetNeighbor();
  cBirthChamber & GetBirthChamber();
  int GetNumNeighbors();
  void Rotate(int direction=1);
  void Breakpoint();
  double TestFitness();
  int GetInput();
  const tArray<double> & GetResources();
  void UpdateResources(const tArray<double> & res_change);
  void Die();
};

#endif

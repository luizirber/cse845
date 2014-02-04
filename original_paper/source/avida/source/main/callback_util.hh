//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CALLBACK_UTIL_HH
#define CALLBACK_UTIL_HH

#include "../tools/tArray.hh"

class cBirthChamber;
class cHardwareBase;
class cOrganism;
class cPopulation;
class cPopulationInterface;

class cCallbackUtil {
public:
  // Callbacks from organism...
  static cHardwareBase * CB_NewHardware(cPopulation * pop, cOrganism * owner);
  static void CB_RecycleHardware(cHardwareBase * out_hardware);
  static bool CB_Divide(cPopulation * pop, int cell_id,
			cOrganism * parent, cOrganism * child);
  static bool CB_TestDivide(cPopulation * pop, int cell_id,
			    cOrganism * parent, cOrganism * child);
  static cOrganism * CB_GetNeighbor(cPopulation * pop, int cell_id);
  static cBirthChamber & CB_BirthChamber(cPopulation * pop, int cell_id);
  static cBirthChamber & CB_TestBirthChamber(cPopulation * pop, int cell_id);
  static int CB_GetNumNeighbors(cPopulation * pop, int cell_id);
  static void CB_Rotate(cPopulation * pop, int cell_id, int direction);
  static void CB_Breakpoint();
  static double CB_TestFitness(cPopulation * pop, int cell_id);
  static int CB_GetInput(cPopulation * pop, int cell_id);
  static const tArray<double>& CB_GetResources(cPopulation * pop, int cell_id);
  static void CB_UpdateResources(cPopulation  * pop, int cell_id,
				 const tArray<double> & res_change);
  static void CB_KillCell(cPopulation * pop, int death_id);
};

#endif

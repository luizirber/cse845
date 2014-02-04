//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2002 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef BIRTH_CHAMBER_HH
#define BIRTH_CHAMBER_HH

#include "../tools/tList.hh"

#include "../cpu/label.hh"

class cGenome;
class cOrganism;

/**
 * All genome-based organisms must go through the birth chamber, which will
 * handle any special modifications during the birth process, such as divide
 * mutations reversions, sterilization, and crossover for sex.  If sex is
 * turned on, this class will hold organisms that are waiting to be crossed
 * over before they are born.
 **/

class cBirthChamber {
private:
  class cBirthEntry {
  public:
    cCodeLabel birth_label;
    cGenome * child_genome;
  };

  tList<cBirthEntry> wait_list;

  static cBirthChamber default_chamber;

public:
  cBirthChamber();
  ~cBirthChamber();

  // Handle manipulations & tests of genome.  Return false if divide process
  // should halt.
  bool SubmitOffspring(cGenome & genome, cOrganism & parent,
		       const cCodeLabel & target_label);

  static cBirthChamber & GetDefaultChamber() { return default_chamber; }
};

#endif

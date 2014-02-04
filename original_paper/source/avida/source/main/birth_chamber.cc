//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2002 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "birth_chamber.hh"

#include "genome.hh"
#include "organism.hh"

// static variables
cBirthChamber cBirthChamber::default_chamber;

cBirthChamber::cBirthChamber()
{
}

cBirthChamber::~cBirthChamber()
{
}

bool cBirthChamber::SubmitOffspring(cGenome & genome, cOrganism & parent,
				    const cCodeLabel & target_label)
{
  return true;
}

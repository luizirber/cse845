//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "hardware_base.hh"

#include "../main/inst_lib.hh"

int cHardwareBase::instance_count(0);

cHardwareBase::cHardwareBase(cOrganism * in_organism, cInstLib * in_inst_lib)
  : organism(in_organism)
  , inst_lib(in_inst_lib)
  , viewer_lock(-1)
{
  assert(inst_lib->OK());
  assert(organism != NULL);

  instance_count++;
}

cHardwareBase::~cHardwareBase()
{
  instance_count--;
}

void cHardwareBase::Recycle(cOrganism * new_organism, cInstLib * in_inst_lib)
{
  assert(inst_lib->OK());
  assert(new_organism != NULL);

  organism    = new_organism;
  inst_lib    = in_inst_lib;
  viewer_lock = -1;
}

void cHardwareBase::Inst_Nop()          // Do Nothing.
{
}


int cHardwareBase::GetNumInst()
{
  assert(inst_lib != NULL);
  return inst_lib->GetSize();
}


cInstruction cHardwareBase::GetRandomInst()
{
  assert(inst_lib != NULL);
  return inst_lib->GetRandomInst();
}


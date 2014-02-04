//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "hardware_factory.hh"

#include "../main/organism.hh"

#include "hardware_base.hh"
#include "hardware_cpu.hh"

// Static private variables...
tList<cHardwareBase> cHardwareFactory::hardware_cpu_list;
int cHardwareFactory::new_count(0);
int cHardwareFactory::recycle_count(0);

cHardwareBase * cHardwareFactory::Create(cOrganism * in_org,
					 cInstLib * inst_lib, int type)
{
  assert(in_org != NULL);
  assert(inst_lib != NULL && inst_lib->OK());

  cHardwareBase * new_hardware = NULL;

  // If there is nothing to recycle, just create a new one.
  if (hardware_cpu_list.GetSize() == 0) {
    new_count++;
    new_hardware = new cHardwareCPU(in_org, inst_lib);
  }

  // If we do have hardware to recycle, do so!
  else {
    new_hardware = hardware_cpu_list.PopRear();
    new_hardware->Recycle(in_org, inst_lib);
    recycle_count++;
  }

  return new_hardware;
}

// Recycled hardware may be *briefly* in use, so don't delete immediately.
void cHardwareFactory::Recycle(cHardwareBase * out_hardware)
{
  hardware_cpu_list.Push(out_hardware);
}

// Delete any hardware that needs it, save the rest for re-use.
void cHardwareFactory::Update()
{
  int delete_count = 0;
  while (hardware_cpu_list.GetSize() > 100) {
    delete hardware_cpu_list.Pop();
    delete_count++;
  }

//    cerr << "   NewHardware: " << new_count
//         << "   Recycled: " << recycle_count
//         << "   Deleted: " << delete_count
//         << "   Stored: " << hardware_cpu_list.GetSize()
//         << "   Existing: " << cHardwareBase::GetInstanceCount()
//         << endl;

  new_count = recycle_count = 0;
}

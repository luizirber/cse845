//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "resource.hh"

cResource::cResource(const cString & _name, int _id)
  : name(_name)
  , id(_id)
  , initial(0.0)
  , inflow(0.0)
  , outflow(0.0)
{
}

cResourceLib::~cResourceLib()
{
  for (int i = 0; i < resource_array.GetSize(); i++) {
    delete resource_array[i];
  }
}

cResource * cResourceLib::AddResource(const cString & res_name)
{
  const int new_id = resource_array.GetSize();
  cResource * new_resource = new cResource(res_name, new_id);
  resource_array.Resize(new_id + 1);
  resource_array[new_id] = new_resource;
  return new_resource;
}

cResource * cResourceLib::GetResource(const cString & res_name) const
{
  for (int i = 0; i < resource_array.GetSize(); i++) {
    if (resource_array[i]->GetName() == res_name) return resource_array[i];
  }
  return NULL;
}

cResource * cResourceLib::GetResource(int id) const
{
  return resource_array[id];
}

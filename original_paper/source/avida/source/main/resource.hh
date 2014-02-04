//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_HH
#define RESOURCE_HH

#include "../tools/string.hh"
#include "../tools/tArray.hh"

class cResource {
private:
  cString name;
  int id;
  double initial;
  double inflow;
  double outflow;
public:
  cResource(const cString & _name, int _id);
  ~cResource() { ; }

  const cString & GetName() const { return name; }
  int GetID() const { return id; }
  double GetInitial() const { return initial; }
  double GetInflow() const { return inflow; }
  double GetOutflow() const { return outflow; }

  void SetInitial(double _initial) { initial = _initial; }
  void SetInflow (double _inflow ) { inflow  = _inflow; }
  void SetOutflow(double _outflow) { outflow = _outflow; }
};

class cResourceLib {
private:
  tArray<cResource *> resource_array;
public:
  cResourceLib() { ; }
  ~cResourceLib();

  int GetSize() const { return resource_array.GetSize(); }

  cResource * AddResource(const cString & res_name);
  cResource * GetResource(const cString & res_name) const;
  cResource * GetResource(int id) const;
};

#endif

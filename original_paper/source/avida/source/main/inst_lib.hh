//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INST_LIB_HH
#define INST_LIB_HH

#include "../tools/string.hh"
#include "../tools/tArray.hh"
#include "../tools/tools.hh"

#include "inst.hh"

// A typdef to simplify having an instruction point to methods in the
// cHardwareBase object and its dirivitives...
class cHardwareBase;
typedef void (cHardwareBase::*tHardwareMethod)();


/**
 * This class is used to create a mapping from the command strings in
 * an organism's genome into real methods in one of the hardware objects.  This
 * object has been designed to allow easy manipulation of the instruction
 * sets, as well as multiple instruction sets within a single soup (just
 * attach different cInstLib objects to different hardware.
 **/

class cInstLib {
private:
  // This class gives full info about a single instruction in the library.
  class cInstEntry {
  public:
    cString name;             // Name of this instruction.
    tHardwareMethod function; // Pointer to hardware method.
    int redundancy;           // Weight in instruction set (not impl.)
    int cost;                 // additional time spent to exectute inst.
    int ft_cost;              // time spent first time exec (in add to cost)
    double prob_fail;         // probability of failing to execute inst
  };

  tArray<cInstEntry> inst_array;  // The instructions indexed by ID
  tArray<int> nop_mods;           // Modification table for nops
  tArray<int> mutation_chart;     // ID's represented by redundancy values.

  // Static components...
  static const cInstruction inst_error;
  // static const cInstruction inst_none;
  static const cInstruction inst_default;


public:
  cInstLib();
  cInstLib(const cInstLib & in_inst_lib);
  ~cInstLib();

  cInstLib & operator=(const cInstLib & _in);

  bool OK() const;

  // Accessors
  const cString & GetName(int id) const
   { return inst_array[id].name; }
  const cString & GetName(const cInstruction & inst) const
   { return GetName(inst.GetOp()); }
  int GetCost(const cInstruction & inst) const
   { return inst_array[inst.GetOp()].cost; }
  int GetFTCost(const cInstruction & inst) const
   { return inst_array[inst.GetOp()].ft_cost; }
  double GetProbFail(const cInstruction & inst) const
   { return inst_array[inst.GetOp()].prob_fail; }
  int GetRedundancy(const cInstruction & inst) const
   { return inst_array[inst.GetOp()].redundancy; }

  tHardwareMethod GetFunction(const cInstruction & inst) const
   { return inst_array[inst.GetOp()].function; }

  int GetNopMod(const cInstruction & inst) const
    { return nop_mods[inst.GetOp()]; }

  cInstruction GetRandomInst() const;
  tHardwareMethod GetRandFunction() const
   { return inst_array[ GetRandomInst().GetOp() ].function; }

  int GetSize() const { return inst_array.GetSize(); }
  int GetNumNops() const { return nop_mods.GetSize(); }

  // Instruction Analysis.
  int IsNop(const cInstruction & inst) const
   { return (inst.GetOp() < nop_mods.GetSize()); }

  // Insertion of new instructions...
  int Add(const cString & _name, tHardwareMethod _fun, const int redundancy=1,
	  const int ft_cost=0, const int cost=0, const double prob_fail=0.0);
  int AddNop(const cString & _name, tHardwareMethod _fun, const int reg,
	     const int redundancy=1, const int ft_cost=0, const int cost=0,
	     const double prob_fail=0.0);

  inline cInstruction GetInst(const cString & in_name) const;
  cString FindBestMatch(const cString & in_name) const;

  // Static methods..
  static const cInstruction & GetInstDefault() { return inst_default; }
  static const cInstruction & GetInstError()   { return inst_error; }
  // static const cInstruction & GetInstNone()    { return inst_none; }
};


inline cInstruction cInstLib::GetInst(const cString & in_name) const
{
  for (int i = 0; i < inst_array.GetSize(); i++) {
    if (inst_array[i].name == in_name) return cInstruction(i);
  }

  // assert(false); // Requesting ID for unknown inst.
  return cInstLib::GetInstError();
}


#endif

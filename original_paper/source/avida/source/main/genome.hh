//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GENOME_HH
#define GENOME_HH

#include <assert.h>
#include "../tools/string.hh"
#include "../tools/tArray.hh"
#include "inst.hh"

/**
 * This class stores the genome of an Avida organism.  Note that once created,
 * a genome should not be modified; only the corresponding memory should be,
 * before creating the genome.  Keeping genome light-weight...
 **/

class cGenome {
protected:
  tArray<cInstruction> genome;
  int active_size;

public:
  explicit cGenome(int _size);
  cGenome(const cGenome & in_genome);
  cGenome(const cString & in_string);
  virtual ~cGenome();

  virtual void operator=(const cGenome & other_genome);
  virtual bool operator==(const cGenome & other_genome) const;

  cInstruction & operator[](int index)
    { assert(index >= 0 && index < active_size);  return genome[index]; }
  const cInstruction & operator[](int index) const
    { assert(index >= 0 && index < active_size);  return genome[index]; }

  virtual void Copy(int to, int from);

  bool OK() const;
   
  int GetSize() const { return active_size; }
  cString AsString() const;

};

#endif



//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////
// This file is for misc. objects which are of general use...

#ifndef TOOLS_HH
#define TOOLS_HH

#include "random.hh"

class cString;

extern cRandom g_random;

/*
 *   Filesystem tools...
 */

class cTools {
public:
  static bool MkDir(const cString & dirname, bool verbose=false);
};


#endif

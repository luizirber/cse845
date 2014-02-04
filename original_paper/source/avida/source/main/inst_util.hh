//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INST_UTIL_HH
#define INST_UTIL_HH

#include <iostream.h>

#include "../tools/tDictionary.hh"

class cInitFile;
class cInstLib;
class cGenome;

class cInstUtil {
public:

  // ========= Genome-File Interaction =========
  // Saving and loading of files.  These functions assume that the genome is
  // the only thing in the file unless 'Internal' is in the function name
  // (Internal genomes must begin with a number that indicates genome length)

  static cGenome LoadGenome(const cString &filename, const cInstLib &inst_lib);
  static cGenome LoadInternalGenome(istream & fp, const cInstLib &inst_lib);
  static void SaveGenome(ostream &fp, const cInstLib &lib, const cGenome &gen);
  static void SaveInternalGenome(ostream &fp, const cInstLib &lib,
				 const cGenome &gen);

  // ========= Genome Construction =========
  static cGenome RandomGenome(int length, const cInstLib & inst_lib);
};

#endif

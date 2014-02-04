//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "tools.hh"

cRandom g_random;

#include <iostream.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#include "string.hh"

bool cTools::MkDir(const cString & dirname, bool verbose)
{
  if (verbose) cout << "Checking for directory '" << dirname << "'..." << endl;

  FILE * fp = fopen ( dirname(), "r" );
  if (fp == 0) {
    if (errno == ENOENT) {
      if (verbose) cout << "  ...not found.  Has to be created..." << endl;
      if (mkdir( dirname(), ACCESSPERMS )) {
	if (verbose) cerr << "  ...ERROR!  Failed to create directory '"
			  << dirname << "'.  This may cause problems..."
			  << endl;
	return false;
      }

      if (verbose) cout << "  ...done!" << endl;
      return true;
    }

    if (verbose) cout << "  ...ERROR! -- Failed to open directory!  This may cause problems." << endl;
    return false;
  }

  return true;
}

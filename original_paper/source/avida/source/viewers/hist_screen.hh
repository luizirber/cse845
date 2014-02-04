//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_HIST_SCREEN_HH
#define VIEW_HIST_SCREEN_HH

#include "text_screen.hh"

class cGenotype;
class cSpecies;

#define HIST_GENOTYPE  0
#define HIST_SPECIES   1
#define NUM_HIST       2

// Unimplemented histogram modes...
#define HIST_TASKS     3
#define HIST_SIZE      4

class cHistScreen : public cScreen {
protected:
  cPopulation & population;
  int mode;

  void PrintGenotype(cGenotype * in_gen, int in_pos, int max_num);
  void PrintSpecies(cSpecies * in_species, int in_pos, int max_num);
public:
  cHistScreen(int y_size, int x_size, int y_start, int x_start,
	      cViewInfo & in_info, cPopulation & in_pop)
    : cScreen(y_size, x_size, y_start, x_start, in_info), population(in_pop)
  { mode = HIST_GENOTYPE; }
  virtual ~cHistScreen() { ; }

  // Virtual in base screen...
  void Draw();
  void Update();
  void DoInput(int in_char);
};

#endif

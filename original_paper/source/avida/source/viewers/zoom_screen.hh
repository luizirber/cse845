//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_ZOOM_SCREEN_HH
#define VIEW_ZOOM_SCREEN_HH

#include "text_screen.hh"
#include "map_screen.hh"

#define ZOOM_MODE_STATS    0
#define ZOOM_MODE_CPU      1
#define ZOOM_MODE_GENOTYPE 2
#define NUM_ZOOM_MODES     3

#define ZOOM_SECTION_MEMORY    0
#define ZOOM_SECTION_MAP       1
#define ZOOM_SECTION_REGISTERS 2
#define ZOOM_SECTION_STACK     3
#define ZOOM_SECTION_INPUTS    4
#define NUM_ZOOM_SECTIONS      5


#define MEMORY_X      0
#define MEMORY_Y      0
#define MEMORY_PRE_SIZE 4
#define MEMORY_POST_SIZE 4

#define CPU_FLAGS_X  0
#define CPU_FLAGS_Y 13

#define STACK_X     18
#define STACK_Y     14
#define REG_X        0
#define REG_Y       14
#define INPUT_X     32
#define INPUT_Y     14
#define TASK_X      50
#define TASK_Y       7
#define MINI_MAP_X  38
#define MINI_MAP_Y   0
#define OPTIONS_X   57
#define OPTIONS_Y    1
#define FAULT_X      2
#define FAULT_Y     13

#define INST_EDIT_BREAKPOINT  0
#define INST_EDIT_JUMP_IP     1
#define INST_EDIT_CHANGE      2
#define INST_EDIT_INSERT      3
#define INST_EDIT_REMOVE      4
#define NUM_INST_EDITS        5

#define THREAD_OPTIONS_VIEW   0
#define THREAD_OPTIONS_LOCK   1
#define NUM_THREAD_OPTIONS    2

class cZoomScreen : public cScreen {
private:
  cPopulation & population;
  int mini_center_id;
  int map_x_size;
  int map_y_size;

  int mode;
  int map_mode;
  bool inst_view_mode;
  int memory_offset;
  bool parasite_zoom; // If true, then view original parasite code, not host
  int active_section;
  int task_offset;

private:
  void DrawCPU();
  void DrawStats();
  void DrawGenotype();
  void UpdateCPU();
  void UpdateStats();
  void UpdateGenotype();

  void EditMemory();
  void ViewMemory();

  void ThreadOptions();

  void ViewInstruction();
  void ViewRegisters();
  void ViewStack();
  void ViewInputs();
  void ViewThreads();

  cCoords GetSectionCoords(int in_section);
  cString GetSectionName(int in_section);
  void SetActiveSection(int in_section);

  void DrawMiniMap();

public:
  cZoomScreen(int y_size, int x_size, int y_start, int x_start,
	      cViewInfo & in_info, cPopulation & in_pop);
  virtual ~cZoomScreen() { ; }

  // Virtual in base screen...
  void Draw();
  virtual void Update();
  virtual void DoInput(int in_char);
  bool DoInputCPU(int in_char);
  bool DoInputStats(int in_char);
  bool DoInputGenotype(int in_char);

  // Other misc functions...
  void AdvanceUpdate() { memory_offset = 0; }
};

#endif

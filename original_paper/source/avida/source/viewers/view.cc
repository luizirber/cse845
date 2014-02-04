//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "../main/genotype.hh"
#include "../main/organism.hh"
#include "../main/phenotype.hh"
#include "../main/population.hh"
#include "../main/population_cell.hh"
#include "../main/stats.hh"

#include "../cpu/test_util.hh"
#include "../cpu/hardware_base.hh"

#include "view.hh"
#include "menu.hh"

#include "ncurses.hh"
#include "ansi.hh"

#include "bar_screen.hh"
#include "map_screen.hh"
#include "stats_screen.hh"
#include "hist_screen.hh"
#include "options_screen.hh"
#include "zoom_screen.hh"

cTextWindow * cView::base_window = NULL;
cScreen * cView::cur_screen = NULL;
cBarScreen * cView::bar_screen = NULL;

//////////////////
//  cView
//////////////////

cView::cView(cPopulation & in_population) :
  info( in_population )
{
  Setup("Avida");

  map_screen     = new cMapScreen     (0,0,3,0,info, in_population);
  stats_screen   = new cStatsScreen   (0,0,3,0,info, in_population);
  hist_screen    = new cHistScreen    (0,0,3,0,info, in_population);
  options_screen = new cOptionsScreen (0,0,3,0,info);
  zoom_screen    = new cZoomScreen    (0,0,3,0,info, in_population);

  info.SetActiveCell( &( in_population.GetCell(0) ) );
}

cView::~cView()
{
  if (map_screen) delete map_screen;
  if (stats_screen) delete stats_screen;
  if (hist_screen) delete hist_screen;
  if (options_screen) delete options_screen;
  if (zoom_screen) delete zoom_screen;

  EndProg(0);
}

void cView::Setup(const cString & in_name)
{
  cur_screen = NULL;

  // Setup text-interface

  StartProg();

  bar_screen = new cBarScreen(3, 0, 0, 0, info, in_name, info.GetPopulation());
  base_window = new cTextWindow(0,0,3,0);
  bar_screen->Draw();
}

void cView::SetViewMode(int in_mode)
{
  if (in_mode == MODE_BLANK) {
    cur_screen = NULL;
  } else if (in_mode == MODE_MAP) {
    cur_screen = map_screen;
  } else if (in_mode == MODE_STATS) {
    cur_screen = stats_screen;
  } else if (in_mode == MODE_HIST) {
    cur_screen = hist_screen;
  } else if (in_mode == MODE_OPTIONS) {
    cur_screen = options_screen;
  } else if (in_mode == MODE_ZOOM) {
    cur_screen = zoom_screen;
  }
}

void cView::Refresh()
{
  ChangeCurScreen(cur_screen); bar_screen->Redraw();
}

void cView::Redraw()
{
  bar_screen->Redraw();
  if (cur_screen) cur_screen->Redraw();
  else base_window->Redraw();
}

void cView::NewUpdate()
{
  if (info.GetPauseLevel() == PAUSE_ADVANCE_STEP){
    return;
  }
  NotifyUpdate();
}

void cView::NotifyUpdate()
{
  // If we're locked onto a specific thread, only stop for it.
  if (info.GetPauseLevel() == PAUSE_ADVANCE_STEP &&
      info.GetThreadLock() != -1  &&
      info.GetThreadLock() != info.GetActiveCell()->GetOrganism()->GetHardware().ViewerLock()){
    return;
  }

  bar_screen->Update();
  info.UpdateSymbols();

  if (cur_screen) cur_screen->Update();
  DoInputs();
}

void cView::NotifyError(const cString & in_string)
{
  cString out_string(in_string);
  out_string.Insert("Error: ");
  Notify(out_string);
  EndProg(1);
}

void cView::NotifyWarning(const cString & in_string)
{
  cString out_string(in_string);
  out_string.Insert("Warning: ");
  Notify(out_string);
}

void cView::NotifyComment(const cString & in_string)
{
  cString out_string(in_string);
  Notify(out_string);
}

void cView::DoBreakpoint()
{
  if (info.GetPauseLevel() == PAUSE_OFF ||
      info.GetPauseLevel() == PAUSE_ADVANCE_UPDATE) {
    Pause();
    NotifyUpdate();
  }
}

void cView::DoInputs()
{
  // If we are in step-wise mode, "finish" this step!

  if (info.GetPauseLevel() == PAUSE_ADVANCE_STEP) {
    info.DisEngageStepMode();
    info.SetPauseLevel(PAUSE_ON);
  }

  int cur_char = ERR;
  // int cur_char = 0; // *** If GetBlockingInput works, use this ***

  // If there is any input in the buffer, process all of it.

  while ((cur_char = GetInput()) != ERR || info.GetPauseLevel() == PAUSE_ON) {

  // *** If GetBlockingInput works, use this ***
  // while( cur_char != ERR ){
    // if paused, block on input
    // if( info.GetPauseLevel() == PAUSE_ON ){
      // cur_char = GetBlockingInput();
    // }else{
      // cur_char = GetInput();
    // }

    switch (cur_char) {
      // Note: Capital 'Q' quits w/o confirming.
    case 'q':
      if (!Confirm("Are you sure you want to quit?")) break;
    case 'Q':
      // clear the windows before we go.  Do bar window last to end at top.
      base_window->Redraw();
      bar_screen->Clear();
      bar_screen->Refresh();
      EndProg(0);  // This implementation calls exit(), blowing us clean away
      break;
    case 'b':
    case 'B':
      ChangeCurScreen(NULL);
      break;
    case 'C':
    case 'c':
      NavigateMapWindow();
      // Now we need to restore the proper window mode (already cleared)
      ChangeCurScreen(cur_screen);
      break;
    case 'h':
    case 'H':
      ChangeCurScreen(hist_screen);
      break;
    case 'm':
    case 'M':
      ChangeCurScreen(map_screen);
      break;
    case 'n':
    case 'N':
      if (info.GetPauseLevel() == PAUSE_ON) {
	info.SetPauseLevel(PAUSE_ADVANCE_UPDATE);
	// parasite_zoom = false; // if executing, show code that is running
	info.GetActiveCell()->GetOrganism()->GetPhenotype().SetFault("");
      }
      if (cur_screen) cur_screen->AdvanceUpdate();
      break;
    case 'o':
    case 'O':
      ChangeCurScreen(options_screen);
      break;
    case 'p':
    case 'P':
      TogglePause();
      break;
    case 's':
    case 'S':
      ChangeCurScreen(stats_screen);
      break;
    case 'W':
    case 'w':
      CloneSoup();
      break;
    case 'X':
    case 'x':
      ExtractCreature();
      break;
    case 'z':
    case 'Z':
      ChangeCurScreen(zoom_screen);
      break;
    case 12: // Ideally this is CTRL-L...
      Refresh();
      break;

    case '*':   // Test Key!!!
      if (true) {
	Confirm("Starting Tests.");
	cMenuWindow menu(50);
	char message[40];
	for (int j = 0; j < 50; j++) {
	  sprintf(message, "Line %d", j);
	  menu.AddOption(j, message);
	}
	menu.SetActive(3);
	menu.Activate();
	Redraw();
      }
      break;
    case ERR:
      break;
    default:
      if (cur_screen) cur_screen->DoInput(cur_char);
      break;
    }
  }

  if (info.GetPauseLevel() == PAUSE_ADVANCE_UPDATE) {
    info.SetPauseLevel(PAUSE_ON);
  }
}

void cView::TogglePause()
{
  // If the run is already paused, un-pause it!
  if (info.GetPauseLevel() != PAUSE_OFF) {
    info.DisEngageStepMode();
    info.SetPauseLevel(PAUSE_OFF);
    info.GetActiveCell()->GetOrganism()->GetPhenotype().SetFault("");
  }

  // Otherwise, turn on the pause.
  else {
    info.SetPauseLevel(PAUSE_ON);
  }

  // Redraw the screen to account for the toggled pause.
  if (cur_screen) cur_screen->Draw();
}

void cView::CloneSoup()
{
  cString filename;
  filename.Set("clone.%d", info.GetPopulation().GetUpdate());
  ofstream fp(filename());
  info.GetPopulation().SaveClone(fp);
  cString message;
  message.Set("Saved clone to file: %s", filename());
  Notify(message);
}

void cView::ExtractCreature()
{
  cGenotype * cur_gen = info.GetActiveGenotype();
  cString gen_name = cur_gen->GetName();

  if (gen_name == "(no name)")
    gen_name.Set("%03d-unnamed", cur_gen->GetLength());

  if (cur_screen) cur_screen->Print(20, 0, "Extracting %s...", gen_name());

  cTestUtil::PrintGenome(cur_gen->GetGenome(), gen_name);

  if (cur_screen) {
    cur_screen->Print(20, 24, "Done.");
    cur_screen->Refresh();
  }
}


void cView::ChangeCurScreen(cScreen * new_screen)
{
  if (cur_screen) cur_screen->Exit();

  cur_screen = new_screen;
  base_window->Redraw();
  if (cur_screen) {
    cur_screen->Clear();
    cur_screen->Draw();
  }
}

void cView::PrintMerit(int in_y, int in_x, cMerit in_merit)
{
  // if we can print the merit normally, do so.
  //if (in_merit.GetDouble() < 1000000) {
    //if (cur_screen) cur_screen->Print(in_y, in_x, "%7d", in_merit.GetUInt());
  //}

  // otherwise use scientific notation. (or somesuch)
  //else {
    if (cur_screen) cur_screen->Print(in_y, in_x, "%7.1e", in_merit.GetDouble());
  //}
}

void cView::PrintFitness(int in_y, int in_x, double in_fitness)
{
  if (!cur_screen) return;

  // If we can print the fitness, do so!
  if (in_fitness <= 0.0) {
    cur_screen->Print(in_y, in_x, " 0.0000");
  }
  else if (in_fitness < 10)
    cur_screen->Print(in_y, in_x, "%7.4f", in_fitness);
  //  else if (in_fitness < 100)
  //    cur_screen->Print(in_y, in_x, "%7.3f", in_fitness);
  else if (in_fitness < 1000)
    cur_screen->Print(in_y, in_x, "%7.2f", in_fitness);
  //  else if (in_fitness < 10000)
  //    cur_screen->Print(in_y, in_x, "%7.1f", in_fitness);
  else if (in_fitness < 100000)
    cur_screen->Print(in_y, in_x, "%7.0f", in_fitness);

  // Otherwise use scientific notations.
  else
    cur_screen->Print(in_y, in_x, "%7.1e", in_fitness);
}

void cView::NavigateMapWindow()
{
  map_screen->Navigate();
}


int cView::Confirm(const cString & message)
{
  int mess_length = message.GetSize();

  // Create a confirm window, and draw it on the screen.

  cTextWindow * conf_win
    = new cTextWindow(3, mess_length + 10, 10, (70 - mess_length) / 2);
  conf_win->Box();
  conf_win->SetBoldColor(COLOR_WHITE);
  conf_win->Print(1, 2, "%s (y/n)", message());
  conf_win->SetBoldColor(COLOR_CYAN);
  conf_win->Print(1, mess_length + 4, 'y');
  conf_win->Print(1, mess_length + 6, 'n');
  conf_win->SetColor(COLOR_WHITE);
  conf_win->Refresh();

  // Wait for the results.
  bool finished = false;
  bool result = false;
  int cur_char;

  while (finished == false) {
    cur_char = GetInput();
    switch (cur_char) {
    case 'q':
    case 'Q':
    case 'n':
    case 'N':
    case ' ':
    case '\n':
    case '\r':
      finished = true;
      result = false;
      break;
    case 'y':
    case 'Y':
      finished = true;
      result = true;
      break;
    }
  }

  // Delete the window, redraw the screen, and return the results.
  delete conf_win;
  Redraw();
  return result;
}

void cView::Notify(const cString & message)
{
  cString mess_copy(message);

  // Setup all of the individual lines.
  int num_lines = message.CountNumLines();
  cString * line_array = new cString[num_lines];
  int max_width = 0;
  for (int i = 0; i < num_lines; i++) {
    line_array[i] = mess_copy.PopLine();
    if (line_array[i].GetSize() > max_width)
      max_width = line_array[i].GetSize();
  }

  // Create a window and draw it on the screen.

  cTextWindow * notify_win
    = new cTextWindow(2 + num_lines, max_width + 4, (24 - num_lines - 3) / 2,
		      (70 - max_width) / 2);
  notify_win->Box();
  notify_win->SetBoldColor(COLOR_WHITE);
  for (int j = 0; j < num_lines; j++) {
    notify_win->Print(1 + j, 2, "%s", line_array[j]());
  }
  notify_win->Refresh();

  // Wait for the results.
  bool finished = false;
  int cur_char;

  while (!finished) {
    cur_char = GetInput();
    switch (cur_char) {
    case 'q':
    case 'Q':
    case ' ':
    case '\n':
    case '\r':
      finished = true;
      break;
    }
  }

  // Delete the window and redraw the screen.
  delete notify_win;
  delete [] line_array;
  Redraw();
}


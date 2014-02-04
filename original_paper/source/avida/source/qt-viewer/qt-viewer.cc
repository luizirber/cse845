//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "../main/avida.hh"

#include "../main/config.hh"
#include "../main/environment.hh"
#include "../main/population.hh"

#include "./avida_driver.hh"

#include "gui_config.hh"

#include <qapplication.h>
#include <qwindowsstyle.h>


/*
FIXME:
fercrissake just write a damned header file.
-- kaben.
*/
void
SetupAvidaQtMultithreading(
  int argc,
  char *argv[],
  cEnvironment &environment,
  cPopulationInterface &test_interface
);


int main(int argc, char * argv[])
{
  /*
  app comes first so that Qt can parse command-line options.
  any options recognized by Qt will be removed from argv; argc will
  modified accordingly.

  see the QApplication Class Reference in Qt docs for details.
  */
  QApplication app( argc, argv );

  cEnvironment environment;
  cPopulationInterface test_interface;
  //SetupAvida( argc, argv, environment, test_interface );
  SetupAvidaQtMultithreading(argc, argv, environment, test_interface);
  
  if (cConfig::GetAnalyzeMode() == true) {
    cAvidaDriver_Base::main_driver = new cAvidaDriver_Analyze();
    ExitAvida(0);
  }
  
  guiConfig();


  cAvidaDriver_QT *qt_viewer = new cAvidaDriver_QT(environment, &app );
  cAvidaDriver_Base::main_driver = qt_viewer;
  
  // Start Avida thread
  qt_viewer->start(); // from QThread; triggers qt_mt_viewer->run();
  
  // Assemble GUI thread
  qt_viewer->Run();
  //cAvidaDriver_Base::main_driver->Run();
  /*
  Unfortunate naming collision:
  run(), inherited from QThread, is the avida thread body;
  Run(), inherited from cAvidaDriver_Population, is the driver entrance.
  */

  // Start GUI thread
  app.exec();

  // Exit Nicely
  ExitAvida(0);
}



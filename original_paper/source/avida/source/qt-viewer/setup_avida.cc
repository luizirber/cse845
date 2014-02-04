//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2002 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

// needs paths to src/tools, src/main, src/cpu, src/event

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>

#include "avida.hh"
#include "callback_util.hh"
#include "config.hh"
#include "environment.hh"
#include "population.hh"
#include "test_cpu.hh"
#include "hardware_util.hh"

#include <qapplication.h>
#include "avida_driver.hh"

#include "setup_wizard.hh"
#include "setup_wizard2.hh"
#include "testwizard.hh"
  
void ExitAvidaQtMultithreading(int exit_code)
{
  signal(SIGINT, SIG_IGN);          // Ignore all future interupts.
  QApplication::exit(exit_code);
  //delete cAvidaDriver_Base::main_driver;
  exit(exit_code);
}


void SetupAvidaQtMultithreading(
  int argc,
  char *argv[],
  cEnvironment &environment,
  cPopulationInterface &test_interface
){
  // Catch Interrupt making sure to close appropriately
  signal(SIGINT, ExitAvidaQtMultithreading);

  // output copyright message
  printf( "Avida version %s\nCopyright (C) 1993-2001 California Institute of Technology.\n\n", AVIDA_VERSION );
  printf( "Avida comes with ABSOLUTELY NO WARRANTY.\n" );
  printf( "This is free software, and you are welcome to redistribute it\nunder certain conditions. See file COPYING for details.\n\n" );

  TestWizard test_wizard(argc, argv);
  test_wizard.exec();

  //AvidaSetupWizard setup_wizard(&argc, &argv);
  //setup_wizard.exec();

  //AvidaSetupWizard2 setup_wizard(argc, argv);
  //setup_wizard.exec();
  
  // Initialize the configuration data...
  //cConfig::Setup(argc, argv);
  //cConfig::SetupMS();
  
  // Initialize the default environment...
   if (environment.Load(cConfig::GetEnvironmentFilename()) == false) {
    cerr << "Unable to load environment... aborting!" << endl;
    ExitAvida(-1);
  }
  cHardwareUtil::LoadInstLibCPU(cConfig::GetInstFilename(),
        environment.GetInstLib());
  cConfig::SetNumInstructions(environment.GetInstLib().GetSize());
  cConfig::SetNumTasks(environment.GetTaskLib().GetSize());
  cConfig::SetNumReactions(environment.GetReactionLib().GetSize());
  cConfig::SetNumResources(environment.GetResourceLib().GetSize());
    
  // Build a test population interface.
  test_interface.SetFun_NewHardware(&cCallbackUtil::CB_NewHardware);
  test_interface.SetFun_Recycle(&cCallbackUtil::CB_RecycleHardware);
  test_interface.SetFun_Divide(&cCallbackUtil::CB_TestDivide);
  test_interface.SetFun_BirthChamber(&cCallbackUtil::CB_TestBirthChamber);
  test_interface.SetFun_GetInput(&cCallbackUtil::CB_GetInput);
  test_interface.SetFun_GetResources(&cCallbackUtil::CB_GetResources);
  test_interface.SetFun_UpdateResources(&cCallbackUtil::CB_UpdateResources);

  // Setup the test CPUs.
  cTestCPU::Setup( &(environment.GetInstLib()), &environment, test_interface );
} 


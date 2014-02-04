//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "avida_driver.hh"


const QString
  cAvidaDriver_QT::update_avida_message("UpdateAvida"),
  cAvidaDriver_QT::step_organism_message("StepOrganism"),
  cAvidaDriver_QT::reset_avida_message("ResetAvida"),
  cAvidaDriver_QT::exit_avida_message("ExitAvida"),
  
  cAvidaDriver_QT::avida_updated_message("AvidaUpdated"),
  cAvidaDriver_QT::organism_stepped_message("OrganismStepped"),
  cAvidaDriver_QT::avida_breakpoint_message("AvidaBreakpoint"),
  cAvidaDriver_QT::avida_continuing_message("AvidaContinuing"),
  cAvidaDriver_QT::avida_reset_message("AvidaReset"),
  cAvidaDriver_QT::avida_exited_message("AvidaExited"),
  
  cAvidaDriver_QT::hello_query_message("Hello?"),
  cAvidaDriver_QT::hello_response_message("Hello..."),

  cAvidaDriver_QT::malformed_message_to_avida("AvidaMessageMalformed"),
  cAvidaDriver_QT::null_message,

  cAvidaDriver_QT::avida_threadstat_breakpoint("Breakpoint"),
  cAvidaDriver_QT::avida_threadstat_stepping("Stepping"),
  cAvidaDriver_QT::avida_threadstat_waiting("Waiting");

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "avida_driver.hh"
#include "messaging_population_wrapper.hh"

#include <qstringlist.h>
#include <qtimer.h>


#ifndef DEBUG
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#else
//# define LOCAL_DEBUG 1
//# define USE_LOCAL_STRINGS 1
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#endif


void cAvidaDriver_QT::guiMessageCatcher(void){
  QString message = m_pop_wrap->GUIReceive(0);
  if (message != null_message){
    #if LOCAL_DEBUG
    cout
    << "<cAvidaDriver_QT::guiMessageCatcher>\n"
    << " --- got message \"" << message << "\".\n";
    #endif

    QStringList components = QStringList::split(",", message);
    if(components[0] == avida_updated_message){
      if(!avidaUpdated(components)) avidaSentBunkMessage(components);
    } else
    if(components[0] == organism_stepped_message){
      if(!organismStepped(components)) avidaSentBunkMessage(components);
    } else
    if(components[0] == avida_breakpoint_message){
      if(!avidaBreakpoint(components)) avidaSentBunkMessage(components);
    } else
    if(components[0] == avida_continuing_message){
      if(!avidaContinuing(components)) avidaSentBunkMessage(components);
    } else
    if(components[0] == avida_reset_message){
      if(!avidaReset(components)) avidaSentBunkMessage(components);
    } else
    if(components[0] == avida_exited_message){
      if(!avidaExited(components)) avidaSentBunkMessage(components);
    } else
    if(components[0] == hello_response_message){
      if(!avidaSaysHi(components)) avidaSentBunkMessage(components);
    } else
    if(components[0] == malformed_message_to_avida){
      avidaGotBunkMessage(components);
    } else {
      avidaSentBunkMessage(components);
    }
  }
}


bool
cAvidaDriver_QT::avidaUpdated(const QStringList &components){
  #if LOCAL_DEBUG
  cout << "<cAvidaDriver_QT::avidaUpdated> called.\n";
  #endif

  if (components.size() != 1){
    return false;
  }
  emit( avidaUpdatedSig() );

  scheduleUpdateSlot();

  #if LOCAL_DEBUG
  cout
  << "<cAvidaDriver_QT::avidaUpdated> called scheduleUpdateSlot().\n";
  #endif

  return true;
}


bool
cAvidaDriver_QT::organismStepped(const QStringList &components){
  #if LOCAL_DEBUG
  cout << "<cAvidaDriver_QT::organismStepped> called.\n";
  #endif

  if (components.size() != 2){
    return false;
  }

  bool ok;
  int cell_id = components[1].toInt(&ok);
  if(!ok
    /*
    use of m_gui_step_organism as a sanity check may be a bad idea;
    things may run fine under a mismatch.

    FIXME:  if this turns out to hold, ditch m_gui_step_organism.
    */
    /*
    ||
    cell_id != m_gui_step_organism
    */
  ){
    return false;
  }

  m_expected_avida_state = STOPPED;
  emit(avidaSteppedSig(cell_id));
  return true;
}


bool
cAvidaDriver_QT::avidaBreakpoint(const QStringList &components){
  //#if LOCAL_DEBUG
  cout << "<cAvidaDriver_QT::avidaBreakpoint> called.\n";
  //#endif

  if (components.size() != 2){
    return false;
  }

  bool ok;
  int cell_id = components[1].toInt(&ok);
  if(!ok
    /*
    use of m_gui_step_organism as a sanity check may be a bad idea;
    things may run fine under a mismatch.

    FIXME:  if this turns out to hold, ditch m_gui_step_organism.
    */
    /*
    ||
    cell_id != m_gui_step_organism
    */
  ){
    return false;
  }

  m_expected_avida_state = STOPPED;
  emit(avidaBreakpointSig(cell_id));
  return true;
}


bool
cAvidaDriver_QT::avidaContinuing(const QStringList &components){
  #if LOCAL_DEBUG
  cout << "<cAvidaDriver_QT::avidaContinuing> called.\n";
  #endif

  if (components.size() != 1){
    return false;
  }
  cout
  << "<cAvidaDriver_QT::avidaContinuing>\n"
  << " --- NOT YET IMPLEMENTED.\n";
  return true;
}


bool
cAvidaDriver_QT::avidaReset(const QStringList &components){
  #if LOCAL_DEBUG
  cout << "<cAvidaDriver_QT::avidaReset> called.\n";
  #endif

  if (components.size() != 1){
    return false;
  }
  cout
  << "<cAvidaDriver_QT::avidaReset>\n"
  << " --- NOT YET IMPLEMENTED.\n";
  return true;
}


bool
cAvidaDriver_QT::avidaExited(const QStringList &components){
  #if LOCAL_DEBUG
  cout << "<cAvidaDriver_QT::avidaExited> called.\n";
  #endif

  if (components.size() != 1){
    return false;
  }
  cout
  << "<cAvidaDriver_QT::avidaExited>\n"
  << " --- NOT FULLY IMPLEMENTED.\n";
  //m_should_avida_update = false;
  m_expected_avida_state = STOPPED;
  m_delay_timer->stop();
  return true;
}


bool
cAvidaDriver_QT::avidaSaysHi(const QStringList &components){
  #if LOCAL_DEBUG
  cout << "<cAvidaDriver_QT::avidaSaysHi> called.\n";
  #endif

  if (components.size() != 2){
    return false;
  }
  cout
  << "<cAvidaDriver_QT::avidaSaysHi>\n"
  << " --- message: \"" << components.join(",") << "\".\n"
  << " --- NOT FULLY IMPLEMENTED.\n";
  return true;
}


void
cAvidaDriver_QT::avidaGotBunkMessage(const QStringList &components){
  cout
  << "<cAvidaDriver_QT::avidaGotBunkMessage> called.\n"
  << " --- bunk message: \""
  << components.join(",") << "\".\n";
  qFatal("<cAvidaDriver_QT::avidaGotBunkMessage>");
}


void
cAvidaDriver_QT::avidaSentBunkMessage(const QStringList &components){
  cout
  << "<cAvidaDriver_QT::avidaSentBunkMessage> called.\n"
  << " --- bunk message: \""
  << components.join(",") << "\".\n";
  qFatal("<cAvidaDriver_QT::avidaSentBunkMessage>");
}


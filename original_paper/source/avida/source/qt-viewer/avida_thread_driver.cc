//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif

#include "population.hh"
#include "config.hh"
#include "messaging_population_wrapper.hh"
#include <qstringlist.h>
#include "avida_driver.hh"


void
cAvidaDriver_QT::avidaMainMessageCatcher(void){

  m_avida_thread_status = avida_threadstat_waiting;

  GenDebug(
    "<cAvidaDriver_QT::avidaMainMessageCatcher>\n"
    " --- entered; ready to begin.\n"
  );

  QString message;
  QStringList components;

  for (;;) {

    GenDebug(
      "<cAvidaDriver_QT::avidaMainMessageCatcher>\n"
      " --- waiting for message...\n"
    );

    /*
    FIXME:  make timeout be a configurable option.
    */
    int timeout = 1000;
    message = m_pop_wrap->avidaReceive(timeout);

    GenDebug(
      "<cAvidaDriver_QT::avidaMainMessageCatcher>\n"
      " --- got message \""
    )(message)("\".\n");

    components = QStringList::split(",", message);

    if(components[0] == update_avida_message){
      if(!parseUpdateAvidaArgs(components)){
        malformedMessageToAvida(message);
      } else {
        m_avida_step_organism = -1;
        // we're nailed into updating mode.
        m_pop_wrap->lock();
        if(ProcessUpdate()){
          m_pop_wrap->unlock();
          m_pop_wrap->urgentToGUI(avida_exited_message);

          GenDebug(
            "<cAvidaDriver_QT::avidaMainMessageCatcher>\n"
            " --- avida thread finishing.\n"
            " --- sent \""
          )(avida_exited_message)("\".\n");

          exit();
        }
        m_pop_wrap->unlock();
      }
    } else
    if(components[0] == step_organism_message){
      if(!parseStepAvidaArgs(components, m_avida_step_organism)){
        malformedMessageToAvida(message);
      } else {
        // we've just switched to stepping mode.
        GenDebug(
          "<cAvidaDriver_QT::avidaMainMessageCatcher>\n"
          " --- trying to enter step mode.  m_avida_step_organism: "
        )(m_avida_step_organism)(".\n");

        m_pop_wrap->lock();
        if(ProcessUpdate()){
          m_pop_wrap->unlock();
          m_pop_wrap->urgentToGUI(avida_exited_message);

          GenDebug(
            "<cAvidaDriver_QT::avidaMainMessageCatcher>\n"
            " --- avida thread finishing.\n"
            " --- sent \""
          )(avida_exited_message)("\".\n");

          exit();
        }
        m_pop_wrap->unlock();
      }
    } else
    if(components[0] == reset_avida_message){
      if(!parseResetAvidaArgs(components)) malformedMessageToAvida(message);
    } else
    if(components[0] == exit_avida_message){
      if(!parseExitAvidaArgs(components)){
        malformedMessageToAvida(message);
      } else {
        m_pop_wrap->urgentToGUI(avida_exited_message);

        GenDebug(
          "<cAvidaDriver_QT::avidaMainMessageCatcher>\n"
          " --- avida thread finishing.\n"
          " --- sent \""
        )(avida_exited_message)("\".\n");

        exit();
      }
    } else
    if(components[0] == hello_query_message){
      if(!parseHelloAvidaArgs(components)) malformedMessageToAvida(message);
    } else
    if(components[0] == null_message){
      ; // do nothing
    } else {
      malformedMessageToAvida(message);
    }
  }
}


void
cAvidaDriver_QT::avidaSteppingMessageCatcher(void){

  m_avida_thread_status = avida_threadstat_stepping;

  GenDebug(
    "<cAvidaDriver_QT::avidaSteppingMessageCatcher>\n"
    " --- entered; ready to begin.\n"
  );

  QString message;
  QStringList components;

  for (;;) {

    GenDebug(
      "<cAvidaDriver_QT::avidaSteppingMessageCatcher>\n"
      " --- waiting for message...\n"
    );

    message = m_pop_wrap->avidaReceive();

    GenDebug(
      "<cAvidaDriver_QT::avidaSteppingMessageCatcher>\n"
      " --- got message \""
    )(message)("\".\n");

    components = QStringList::split(",", message);

    if(components[0] == update_avida_message){
      if(!parseUpdateAvidaArgs(components)){
        malformedMessageToAvida(message);
      } else {
        m_avida_step_organism = -1;
        // we've just switched out of stepping mode, into updating mode.
        return;
      }
    } else
    if(components[0] == step_organism_message){
      if(!parseStepAvidaArgs(components, m_avida_step_organism)){
        malformedMessageToAvida(message);
      } else {
        // we've still in stepping mode, maybe with a different stepping
        // organism.
        return;
      }
    } else
    if(components[0] == reset_avida_message){
      if(!parseResetAvidaArgs(components)) malformedMessageToAvida(message);
    } else
    if(components[0] == exit_avida_message){
      if(!parseExitAvidaArgs(components)){
        malformedMessageToAvida(message);
      } else {
        m_pop_wrap->urgentToGUI(avida_exited_message);

        GenDebug(
          "<cAvidaDriver_QT::avidaSteppingMessageCatcher>\n"
          " --- avida thread finishing.\n"
          " --- sent \""
        )(avida_exited_message)("\".\n");

        exit();
      }
    } else
    if(components[0] == hello_query_message){
      if(!parseHelloAvidaArgs(components)) malformedMessageToAvida(message);
    } else
    if(components[0] == null_message){
      ; // do nothing
    } else {
      malformedMessageToAvida(message);
    }
  }
}


void
cAvidaDriver_QT::avidaBreakpointMessageCatcher(void){

  m_avida_thread_status = avida_threadstat_breakpoint;

  GenDebug(
    "<cAvidaDriver_QT::avidaBreakpointMessageCatcher>\n"
    " --- entered; ready to begin.\n"
  );

  QString message;
  QStringList components;

  for (;;) {

    GenDebug(
      "<cAvidaDriver_QT::avidaBreakpointMessageCatcher>\n"
      " --- waiting for message...\n"
    );

    message = m_pop_wrap->avidaReceive();

    GenDebug(
      "<cAvidaDriver_QT::avidaBreakpointMessageCatcher>\n"
      " --- got message \""
    )(message)("\".\n");

    components = QStringList::split(",", message);

    if(components[0] == update_avida_message){
      if(!parseUpdateAvidaArgs(components)){
        malformedMessageToAvida(message);
      } else {
        m_avida_step_organism = -1;
        // we've just switched out of stepping mode, into updating mode.
        return;
      }
    } else
    if(components[0] == step_organism_message){
      if(!parseStepAvidaArgs(components, m_avida_step_organism)){
        malformedMessageToAvida(message);
      } else {
        // We're in stepping mode now even if we weren't before
        // breakpoint.  If we were, maybe we've switched to a different
        // stepping organism.
        return;
      }
    } else
    if(components[0] == reset_avida_message){
      if(!parseResetAvidaArgs(components)) malformedMessageToAvida(message);
    } else
    if(components[0] == exit_avida_message){
      if(!parseExitAvidaArgs(components)){
        malformedMessageToAvida(message);
      } else {
        m_pop_wrap->urgentToGUI(avida_exited_message);

        GenDebug(
          "<cAvidaDriver_QT::avidaBreakpointMessageCatcher>\n"
          " --- avida thread finishing.\n"
          " --- sent \""
        )(avida_exited_message)("\".\n");

        exit();
      }
    } else
    if(components[0] == hello_query_message){
      if(!parseHelloAvidaArgs(components)) malformedMessageToAvida(message);
    } else
    if(components[0] == null_message){
      ; // do nothing
    } else {
      malformedMessageToAvida(message);
    }
  }
}


void
cAvidaDriver_QT::NotifyUpdate(void){
  m_pop_wrap->sendToGUI(avida_updated_message);

  GenDebug(
    "<cAvidaDriver_QT::NotifyUpdate>"
    " sent \""
  )(avida_updated_message)("\".\n");
}

void
cAvidaDriver_QT::NotifyStep(int cell_id){
  m_pop_wrap->sendToGUI(
    (organism_stepped_message + ",%1")
    .arg(cell_id)
  );

  GenDebug(
    "<cAvidaDriver_QT::NotifyStep>"
    " sent \""
  )((organism_stepped_message + ",%1").arg(cell_id))("\".\n");
  
  avidaSteppingMessageCatcher();
}

void
cAvidaDriver_QT::ProcessOrganisms(void){
  GenDebug("<cAvidaDriver_QT::ProcessOrganisms> called.\n");
  
  // Process the update.
  const int UD_size =
    cConfig::GetAveTimeslice()*population->GetNumOrganisms();
  m_pop_wrap->unlock();
  const double step_size = 1.0 / (double) UD_size;
  
  // Keep the viewer informed about the organism we are stepping
  // through, if we're stepping through an organism...
  // m_avida_step_organism == -1 if we're not, in which case
  // NotifyStep() will never be called.

  for (int i = 0; i < UD_size; i++) {
    m_pop_wrap->lock();
    m_current_organism_id = population->ScheduleOrganism();
    population->ProcessStep(step_size, m_current_organism_id);
    m_pop_wrap->unlock();
    if (m_current_organism_id == m_avida_step_organism){
      NotifyStep(m_current_organism_id);
    }
  }

  // end-of-update stats...
  m_pop_wrap->lock();
  population->CalcUpdateStats();

  // Setup the viewer for the new update.
  cStats & stats = population->GetStats();
  Message(
    "UD: %d\t"
    "Gen: %f\t"
    "Fit: %f",
    stats.GetUpdate(),
    stats.SumGeneration().Average(),
    stats.GetAveFitness()
  );
  m_pop_wrap->unlock();

  NotifyUpdate();

  m_pop_wrap->lock();
}

// SignalBreakpoint():  called from beneath
// population->ProcessStep() call tree when an organism's instruction has
// break flag set.
void
cAvidaDriver_QT::SignalBreakpoint(void){
  GenDebug(
    "<cAvidaDriver_QT::SignalBreakpoint>\n"
    " --- BREAKPOINT CAUGHT.\n"
  );

  m_pop_wrap->unlock();
  m_pop_wrap->sendToGUI(
    (avida_breakpoint_message + ",%1")
    .arg(m_current_organism_id)
  );
  GenDebug(
    "<cAvidaDriver_QT::SignalBreakpoint>\n"
    " --- sent message \""
  )((avida_breakpoint_message + ",%1").arg(m_current_organism_id))
  ("\".\n");

  avidaBreakpointMessageCatcher();
  GenDebug(
    "<cAvidaDriver_QT::SignalBreakpoint>\n"
    " --- CONTINUING.\n"
  );
  m_pop_wrap->lock();
}


bool
cAvidaDriver_QT::parseUpdateAvidaArgs(const QStringList &components){
  GenDebug("<cAvidaDriver_QT::parseUpdateAvidaArgs> called.\n");

  if (components.size() != 1){
    return false;
  } else {
    return true;
  }
}


bool
cAvidaDriver_QT::parseStepAvidaArgs(
  const QStringList &components,
  int &cell_id_return
){
  GenDebug("<cAvidaDriver_QT::parseStepAvidaArgs> called.\n");

  if (components.size() != 2){
    return false;
  }
  bool ok;
  int cell_id = components[1].toInt(&ok);
  if (!ok){
    return false;
  }

  /* FIXME:  sanity check cell_id:  is it in the avida world?
     if it isn't, puke.
  */

  cell_id_return = cell_id;

  return true;
}


bool
cAvidaDriver_QT::parseContinueAvidaArgs(const QStringList &components){
  GenDebug("<cAvidaDriver_QT::parseContinueAvidaArgs> called.\n");

  if (components.size() != 1){
    return false;
  }
  GenError(
    "<cAvidaDriver_QT::parseContinueAvidaArgs>\n"
    " --- NOT YET IMPLEMENTED.\n"
  );
  m_pop_wrap->sendToGUI(avida_continuing_message);

  GenDebug(
    "<cAvidaDriver_QT::parseContinueAvidaArgs>"
    " sent \""
  )(avida_continuing_message)("\".\n");

  return true;
}


bool
cAvidaDriver_QT::parseResetAvidaArgs(const QStringList &components){
  GenDebug("<cAvidaDriver_QT::parseResetAvidaArgs> called.\n");

  if (components.size() != 1){
    return false;
  }
  GenError(
    "<cAvidaDriver_QT::parseResetAvidaArgs>\n"
    " --- NOT YET IMPLEMENTED.\n"
  );
  m_pop_wrap->urgentToGUI(avida_reset_message);

  GenDebug(
    "<cAvidaDriver_QT::parseResetAvidaArgs>"
    " sent \""
  )(avida_reset_message)("\".\n");

  return true;
}


bool
cAvidaDriver_QT::parseExitAvidaArgs(const QStringList &components){
  GenDebug("<cAvidaDriver_QT::parseExitAvidaArgs> called.\n");

  if (components.size() != 1){
    return false;
  } else {
    return true;
  }
}


bool
cAvidaDriver_QT::parseHelloAvidaArgs(const QStringList &components){
  GenDebug("<cAvidaDriver_QT::parseHelloAvidaArgs> called.\n");

  if (components.size() != 1){
    return false;
  }
  m_pop_wrap->sendToGUI(
    hello_response_message + "," + m_avida_thread_status
  );

  GenDebug(
    "<cAvidaDriver_QT::parseHelloAvidaArgs>"
    " sent \""
  )(hello_response_message + "," + m_avida_thread_status)("\".\n");

  return true;
}


void
cAvidaDriver_QT::malformedMessageToAvida(const QStringList &components){
  GenError("<cAvidaDriver_QT::malformedMessageToAvida> called.\n");

  m_pop_wrap->urgentToGUI(
    malformed_message_to_avida + "," + components.join(",")
  );
  GenError(
    "<cAvidaDriver_QT::malformedMessageToAvida>"
    " sent \""
  )(malformed_message_to_avida + "," + components.join(","))("\".\n");

  exit();
}

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif

#include "./avida_driver.hh"
#include "./main_window.hh"
#include "messaging_population_wrapper.hh"

#include <qapplication.h>
#include <qtimer.h>
#include <qdatetime.h>

#include <qvbox.h>
#include <qpushbutton.h>


cAvidaDriver_QT::
cAvidaDriver_QT(cEnvironment & environment, QApplication *app)
  : cAvidaDriver_Population(environment), m_app( app ),
    m_main_window( 0 )
{
  m_pop_wrap = new MessagingPopulationWrapper(population);
  // event_list belongs to inherited class cAvidaDriver_Population.
  m_pop_wrap->setEventList(event_list);

  // signals to avida from gui.
  connect(
    m_pop_wrap, SIGNAL(startAvidaSig()),
    this, SLOT(startAvidaSlot())
  );
  connect(
    m_pop_wrap, SIGNAL(stopAvidaSig()),
    this, SLOT(stopAvidaSlot())
  );
  connect(
    m_pop_wrap, SIGNAL(stepAvidaSig(int)),
    this, SLOT(stepOrganismSlot(int))
  );
  connect(
    m_pop_wrap, SIGNAL(updateAvidaSig()),
    this, SLOT(updateAvidaSlot())
  );
  connect(
    m_pop_wrap, SIGNAL(resetAvidaSig()),
    this, SLOT(resetAvidaSlot())
  );
  connect(
    m_pop_wrap, SIGNAL(exitAvidaSig()),
    this, SLOT(exitAvidaSlot())
  );

  // signals from avida to gui.
  connect(
    this, SIGNAL(avidaUpdatedSig()),
    m_pop_wrap, SLOT(avidaUpdatedSlot())
  );
  connect(
    this, SIGNAL(avidaSteppedSig(int)),
    m_pop_wrap, SLOT(avidaSteppedSlot(int))
  );
  connect(
    this, SIGNAL(avidaBreakpointSig(int)),
    m_pop_wrap, SLOT(avidaBreakpointSlot(int))
  );
  connect(
    this, SIGNAL(avidaStoppedSig()),
    m_pop_wrap, SLOT(avidaStoppedSlot())
  );

  connect(
    m_pop_wrap, SIGNAL(queryGUISig()),
    this, SLOT(queryGUISlot())
  );
  connect(
    this, SIGNAL(guiStatusSig(bool)),
    m_pop_wrap, SLOT(guiStatusSlot(bool))
  );
}

cAvidaDriver_QT::~cAvidaDriver_QT()
{
  GenDebug(
    "<cAvidaDriver_QT::~cAvidaDriver_QT> called.\n"
  );

#define CHECK_DELAY 500
#define FIRST_WAIT 5000
#define SECOND_WAIT 5000

  if(running()){
    GenDebug(
      " --- avida thread is still running.\n"
      " --- sending \""
    )(
      exit_avida_message
    )(
      "\" to avida thread.\n"
    );

    m_last_update_time->restart();
    m_pop_wrap->urgentToAvida(exit_avida_message);

    GenDebug(
      "<cAvidaDriver_QT::~cAvidaDriver_QT>\n"
      " --- sent \""
    )(
      exit_avida_message
    )(
      "\" to avida thread.\n"
      " --- waiting for avida thread to send \""
    )(
      avida_exited_message
    );

    while(running()){
      QString message = m_pop_wrap->GUIReceive(CHECK_DELAY);

      GenDebug(
        "<cAvidaDriver_QT::~cAvidaDriver_QT> got message \""
      )(
        message
      );

      QStringList components = QStringList::split(",", message);
      if(components[0] == avida_exited_message && components.size() == 1){

        GenDebug(" --- waiting for avida thread to quit...\n");

        wait();

        GenDebug(
          "<cAvidaDriver_QT::~cAvidaDriver_QT> "
          "avida thread has quit.\n"
        );

        break;
      } else {

        GenError(
          " --- avida thread did not send \""
        )(
          avida_exited_message
        )(
          "\".\n"
        );

      }
      if(m_last_update_time->elapsed() > FIRST_WAIT){
        GenError(
          "<cAvidaDriver_QT::~cAvidaDriver_QT>\n"
          " --- avida thread not responding to message\n"
          " --- \""
        )(
          exit_avida_message
        )(
          "\" for %d, seconds.\n"
          " --- force-quitting in %d"
          " seconds.\n",
          FIRST_WAIT/1000,
          SECOND_WAIT/1000
        );

        m_last_update_time->restart();
        while(m_last_update_time->elapsed() < SECOND_WAIT){
          if(m_pop_wrap->tryLock() == true){
            delete m_pop_wrap;
            break;
          }
        }
        break;
      }
    }
  }
  GenDebug("<cAvidaDriver_QT::~cAvidaDriver_QT> exiting.\n");

  return;

}


void
cAvidaDriver_QT::run(void)
{
  avidaMainMessageCatcher();
}


void
cAvidaDriver_QT::Run(void)
{ 
  assert( population != NULL );
  assert( m_app != NULL );
    
  GenDebug("<cAvidaDriver_QT::Run> entered.  building GUI.\n");
  
  buildGUI();
  
  //m_should_avida_update = false;
  m_expected_avida_state = STOPPED;
  m_gui_step_organism = -1; // no step organism yet.
  
  GenDebug(
    "<cAvidaDriver_QT::Run>\n"
    " --- constructing m_messaging_timer.\n"
  );
  m_messaging_timer_delay = DEFAULT_MESSAGING_TIMER_DELAY;
  m_messaging_timer = new QTimer(this);
  connect(
    m_messaging_timer, SIGNAL(timeout()),
    this, SLOT(guiMessageCatcher())
  );
  m_messaging_timer->start(m_messaging_timer_delay, false);

  m_min_update_interval = DEFAULT_MIN_UPDATE_INTERVAL;
  m_delay_timer = new QTimer(this);
  connect(
    m_delay_timer, SIGNAL(timeout()),
    this, SLOT(updateAvidaSlot())
  );

  m_retry_timer_delay = DEFAULT_RETRY_TIMER_DELAY;
  m_retry_timer = new QTimer(this);

  GenDebug(" --- constructing m_last_update_time.\n");

  m_last_update_time = new QTime();
  m_last_update_time->start();

  GenDebug(
    " --- m_last_update_time: "
  )(
    m_last_update_time->toString()
  )(
    ".\n"
  );
}


void
cAvidaDriver_QT::buildGUI(void){
  m_main_window = new MainWindow(
    m_pop_wrap,
    0,
    m_app->argc(),
    m_app->argv()
  );
  m_app->setMainWidget(m_main_window);
  m_main_window->show();
}

void
cAvidaDriver_QT::scheduleUpdateSlot(void) {
  GenDebug("<cAvidaDriver_QT::scheduleUpdateSlot> called.\n");
  
  /*
  if (m_should_avida_update){
    m_gui_step_organism = -1;
    int delay = m_min_update_interval - m_last_update_time->elapsed();
    if(delay < 0) delay = 0;
    m_delay_timer->start(delay, true); // single-shot mode.

    GenDebug(
      "<cAvidaDriver_QT::scheduleUpdateSlot>\n"
      " --- scheduled update in %d msec.\n", delay
    );

  } else {
    GenDebug(
      " --- m_should_avida_update == false.\n"
      " --- (Avida is stopped.)\n"
    );
  }
  */
  switch(m_expected_avida_state){
  case STEPPING:
    GenDebug(
      "<cAvidaDriver_QT::scheduleUpdateSlot>\n"
      " --- trying to resume stepping mode.\n"
    );

    stepOrganismSlot(m_gui_step_organism);
    break;
  case RUNNING:
    m_gui_step_organism = -1;
    /*
    disabling delay_timer; replacing with direct call to
    updateAvidaSlot().

    FIXME:  scheduleUpdateSlot() and updateAvidaSlot() can be joined if
    I decide to do away with the delay timer.
    */
    /*
    int delay = m_min_update_interval - m_last_update_time->elapsed();
    if(delay < 0) delay = 0;
    m_delay_timer->start(delay, true); // single-shot mode.
    GenDebug(
      "<cAvidaDriver_QT::scheduleUpdateSlot>\n"
      " --- scheduled update in %d msec.\n",
      delay
    );
    */

    GenDebug(
      "<cAvidaDriver_QT::scheduleUpdateSlot>\n"
      " --- trying to enter running mode.\n"
    );

    updateAvidaSlot();


    break;
  case STOPPED:
    GenDebug(
      " --- m_expected_avida_state == STOPPED.\n"
      " --- (not updating.)\n"
      " --- sending avidaStoppedSig().\n"
    );

    emit avidaStoppedSig();
    
    break;
  default:
    GenError(
      " --- m_expected_avida_state == INSANE.\n"
      " --- (m_expected_avida_state shouldn't have this value.)\n"
      " --- dying gracelessly.\n"
    );
    qFatal("m_expected_avida_state has insane value.");

    break;  // never reached.
  }
  GenDebug("<cAvidaDriver_QT::scheduleUpdateSlot> done.\n");
} 
  

void
cAvidaDriver_QT::updateAvidaSlot(void){
  GenDebug(
    "<cAvidaDriver_QT::updateAvidaSlot> called.\n"
    " --- last update was %d"
    " msec ago.\n",
    m_last_update_time->restart()
  );
  m_last_update_time->restart();

  m_gui_step_organism = -1;
  m_pop_wrap->sendToAvida(update_avida_message);

  GenDebug(
    "<cAvidaDriver_QT::updateAvidaSlot>\n"
    " --- sent \""
  )(
    update_avida_message
  )(
    "\".\n"
  );
}


void
cAvidaDriver_QT::stepOrganismSlot(int cell_id){
  GenDebug("<cAvidaDriver_QT::stepOrganismSlot> called.\n");

  /* FIXME:  verify that cell_id is in the grid.  -- K.
  */
  m_expected_avida_state = STEPPING;
  m_gui_step_organism = cell_id;
  m_pop_wrap->sendToAvida(
    (step_organism_message + ",%1")
    .arg(cell_id)
  );

  GenDebug(
    "<cAvidaDriver_QT::stepOrganismSlot>\n"
    " --- sent \""
  )(
    (step_organism_message + ",%1").arg(cell_id)
  )(
    "\".\n"
  );
}

//void
//cAvidaDriver_QT::continueAvidaSlot(void){
//  GenDebug(
//    "<cAvidaDriver_QT::continueAvidaSlot> called.\n"
//    " --- NOT YET IMPLEMENTED \n"
//  );
//
//  //m_pop_wrap->sendToAvida(continue_avida_message);
//
//  GenDebug(
//    "<cAvidaDriver_QT::continueAvidaSlot>\n"
//    " --- sent \""
//  )(
//    continue_avida_message
//  )(
//    "\".\n"
//  );
//}

void
cAvidaDriver_QT::startAvidaSlot(void){
  GenDebug("<cAvidaDriver_QT::startAvidaSlot> called.\n");

  //m_should_avida_update = true;
  m_expected_avida_state = RUNNING;
  scheduleUpdateSlot();
}

void
cAvidaDriver_QT::stopAvidaSlot(void){
  GenDebug("<cAvidaDriver_QT::stopAvidaSlot> called.\n");

  //m_should_avida_update = false;
  m_expected_avida_state = STOPPED;
  m_delay_timer->stop();
}

void
cAvidaDriver_QT::resetAvidaSlot(void){
  GenDebug("<cAvidaDriver_QT::resetAvidaSlot> called.\n");

  m_expected_avida_state = STOPPED;
  m_pop_wrap->urgentToAvida(reset_avida_message);

  GenDebug(
    "<cAvidaDriver_QT::resetAvidaSlot>\n"
    " --- sent \""
  )(
    reset_avida_message
  )(
    "\".\n"
  );
}

void
cAvidaDriver_QT::exitAvidaSlot(void){
  m_app->quit();
}

void
cAvidaDriver_QT::helloAvidaSlot(void){
  GenDebug("<cAvidaDriver_QT::helloAvidaSlot> called.\n");

  m_pop_wrap->urgentToAvida(hello_query_message);

  GenDebug(
    "<cAvidaDriver_QT::helloAvidaSlot>\n"
    " --- sent \""
  )(
    hello_query_message
  )(
    "\".\n"
  );
}

void
cAvidaDriver_QT::malformedMessageToAvidaSlot(void){
  GenError("<cAvidaDriver_QT::malformedMessageToAvidaSlot> called.\n");
  m_pop_wrap->sendToAvida("fooey");
  GenError(
    "<cAvidaDriver_QT::malformedMessageToAvidaSlot>\n"
    " --- sent \"fooey\".\n"
  );
}



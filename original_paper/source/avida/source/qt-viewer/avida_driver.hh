//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_DRIVER_HH
#define AVIDA_DRIVER_HH


#include "../main/avida.hh"

#include <qobject.h>
#include <qthread.h>


//#define DEFAULT_MESSAGING_TIMER_DELAY 100
#define DEFAULT_MESSAGING_TIMER_DELAY 50
//#define DEFAULT_MIN_UPDATE_INTERVAL 1000
#define DEFAULT_MIN_UPDATE_INTERVAL 0
#define DEFAULT_RETRY_TIMER_DELAY 100


class QApplication;
class QTimer;
class QTime;
class MessagingPopulationWrapper;
class MainWindow;

class cAvidaDriver_QT :
  public QObject,
  public cAvidaDriver_Population,
  public QThread
{
  Q_OBJECT
private:
  QApplication *m_app;
  MessagingPopulationWrapper *m_pop_wrap;
  MainWindow *m_main_window;

  static const QString
    update_avida_message,
    step_organism_message,
    reset_avida_message,
    exit_avida_message,
  
    avida_updated_message,
    organism_stepped_message,
    avida_breakpoint_message,
    avida_continuing_message,
    avida_reset_message,
    avida_exited_message,

    hello_query_message,
    hello_response_message,

    malformed_message_to_avida,
    null_message,

    avida_threadstat_breakpoint,
    avida_threadstat_stepping,
    avida_threadstat_waiting;

  /*
  These should be set and read only by the avida thread.
  m_avida_thread_status should be one of
    avida_threadstat_breakpoint,
    avida_threadstat_stepping,
    avida_threadstat_waiting;
  */
  QString m_avida_thread_status;
  int m_avida_step_organism;
  int m_current_organism_id;

  /*
  These should be set and read only by the gui thread.
  */
  //bool m_should_avida_update;
  enum ExpectedAvidaState{
    STOPPED,
    STEPPING,
    RUNNING
  } m_expected_avida_state;
  int m_gui_step_organism;

  QTime *m_last_update_time;

  QTimer
    *m_messaging_timer,
    *m_delay_timer,
    *m_retry_timer;
  unsigned int
    m_messaging_timer_delay,
    m_min_update_interval,
    m_retry_timer_delay;

public:
   cAvidaDriver_QT(cEnvironment & environment, QApplication *app);
  ~cAvidaDriver_QT();

  virtual void Run();
  virtual void run();

  // Avida thread management
  virtual void ProcessOrganisms();

  /*
  virtual void ExitProgram(int exit_code);  // Called on an interupt,

  virtual void NotifyComment(const cString & in_string);
  virtual void NotifyWarning(const cString & in_string);
  virtual void NotifyError(const cString & in_string);
  */

signals:
  void avidaRunningSig(void);
  void avidaStoppedSig(void);

  void avidaUpdatedSig(void);
  void avidaBreakpointSig(int cell_id);
  void avidaSteppedSig(int cell_id);
  void avidaExitedSig(void);
  void avidaResetSig(void);
  
  void guiStatusSig(bool stopped);
private slots:

  void scheduleUpdateSlot(void);
  void updateAvidaSlot(void);
  void stepOrganismSlot(int cell_id);
  //void continueAvidaSlot(void);
  void startAvidaSlot(void);
  void stopAvidaSlot(void);
  void resetAvidaSlot(void);
  void exitAvidaSlot(void);
  void helloAvidaSlot(void);
  void malformedMessageToAvidaSlot(void);

  void queryGUISlot(void){
    emit guiStatusSig((m_expected_avida_state == STOPPED));
  }
protected slots:

  // GUI thread management
  void guiMessageCatcher(void);

protected:

  void buildGUI(void);

  // GUI thread management
  bool avidaUpdated(const QStringList &);
  bool organismStepped(const QStringList &);
  bool avidaBreakpoint(const QStringList &);
  bool avidaContinuing(const QStringList &);
  bool avidaReset(const QStringList &);
  bool avidaExited(const QStringList &);
  bool avidaSaysHi(const QStringList &);
  void avidaGotBunkMessage(const QStringList &);
  void avidaSentBunkMessage(const QStringList &);

  // experimental messaging classes
  /*
  class Msg : public QCustomEvent {
  public: Msg(MessagingPopulationWrapper *p):QCustomEvent(6666),mpw(p){;}
    MessagingPopulationWrapper *pw(){return mpw;}
  private: MessagingPopulationWrapper *mpw;
  };

  class Rcv : public QObject {
  public: Rcv(){;}
    void customEvent(QCustomEvent *e){
      Msg *m=(Msg *)e;
      QString message = m->GUIReceive(0);
      if (message != null_message){
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
  };
  */

  // Avida thread management
  void avidaMainMessageCatcher(void);
  void avidaSteppingMessageCatcher(void);
  void avidaBreakpointMessageCatcher(void);

  bool parseUpdateAvidaArgs(const QStringList &);
  bool parseStepAvidaArgs(const QStringList &, int &cell_id_return);
  bool parseContinueAvidaArgs(const QStringList &);
  bool parseResetAvidaArgs(const QStringList &);
  bool parseExitAvidaArgs(const QStringList &);
  bool parseHelloAvidaArgs(const QStringList &);

  void malformedMessageToAvida(const QStringList &);

  virtual void NotifyUpdate(void);
  virtual void NotifyStep(int cell_id);
  virtual void SignalBreakpoint(void);
};



#endif

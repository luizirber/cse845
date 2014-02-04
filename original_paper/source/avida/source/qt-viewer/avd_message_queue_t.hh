//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
// 
// Read the COPYING and README files, or contact 'avida@alife.org',
//
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//
//////////////////////////////////////////////////////////////////////////////

#include <qapplication.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qthread.h>

#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif

#include "avd_message_queue.hh"


typedef avd_MessageQueue<int> int_MessageQueue;


class Provider : public QPushButton, public QThread {

  Q_OBJECT

private:
  int_MessageQueue *m_message_queue;
  int_MessageQueue *m_quit_queue;
  int m_counter;
public:
  Provider(
    int_MessageQueue &message_queue,
    int_MessageQueue &quit_queue,
    QWidget *parent = 0,
    const char *name = 0
  )
  : QPushButton(parent, name)
  {
    m_message_queue = &message_queue;
    m_quit_queue = &quit_queue;

    setText("Send Message");
    connect(
      this, SIGNAL(clicked()), SLOT(sendMessage())
    );

    m_counter = 1;

    start();
  }
public slots:
  void sendMessage(void){
    m_message_queue->send(m_counter++);
  }
protected:
  void run(void){
    for(;;){
      if(m_quit_queue->receive(0) == 0){
        m_message_queue->send(0);
        return;
      }
      m_message_queue->send(m_counter++);
      sleep(1);
    }
  }
};


class Consumer : public QObject, public QThread {

  Q_OBJECT

private:
  int_MessageQueue *m_message_queue;
public:
  Consumer(
    int_MessageQueue &message_queue,
    QWidget *parent = 0,
    const char *name = 0
  )
  : QObject(parent, name)
  {
    m_message_queue = &message_queue;

    start();
  }
protected:
  void run(void){
    int message;

    for(;;){
      message = m_message_queue->receive();
      GenDebug(
        "<Consumer::run>\n"
        " --- message %s\n",
        message
      );
      if (0 == message){
        GenDebug(" --- quitting.\n");
        return;
      }
    }
  }
};


class QuitAll : public QPushButton {

  Q_OBJECT

private:
  int_MessageQueue *m_quit_queue;
public:
  QuitAll(
    int_MessageQueue &quit_queue,
    QWidget *parent = 0,
    const char *name = 0
  )
  : QPushButton(parent, name)
  {
    m_quit_queue = &quit_queue;

    setText("Quit");
    connect(
      this, SIGNAL(clicked()), SLOT(quitMessage())
    );
  }
public slots:
  void quitMessage(void){
    m_quit_queue->send(0);
  }
};

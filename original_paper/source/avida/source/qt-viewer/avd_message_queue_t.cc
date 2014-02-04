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

#include "avd_message_queue_t.hh"


int main(int argc, char *argv[]){

  QApplication app( argc, argv );
  QVBox *vbox = new QVBox();
  app.setMainWidget(vbox);

  int_MessageQueue *message_queue = new int_MessageQueue(-1);
  int_MessageQueue *quit_queue = new int_MessageQueue(-1);

  Consumer *consumer = new Consumer(
    *message_queue,
    vbox
  );
  Provider *provider = new Provider(
    *message_queue,
    *quit_queue,
    vbox
  );
  QuitAll *quit_all = new QuitAll(
    *quit_queue,
    vbox
  );

  app.connect(
    quit_all, SIGNAL(clicked()),
    &app, SLOT(quit())
  );

  vbox->show();
  app.exec();
  
  consumer->wait();
  provider->wait();
  
  message_queue->send(1);
  message_queue->urgent(2);
  int message = message_queue->receive();
  GenDebug(
    "<main>\n"
    " --- message %s\n",
    message
  );
  
  int pending = message_queue->getNumberPending();
  GenDebug(
    "<main>\n"
    " --- pending %s\n",
    pending
  );

  message_queue->flush();

  pending = message_queue->getNumberPending();
  GenDebug(
    "<main>\n"
    " --- pending %s\n",
    pending
  );

  delete message_queue;
  delete quit_queue;

  return 0;
}

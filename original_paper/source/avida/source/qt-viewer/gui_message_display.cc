#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif

#include <qstring.h>

cMessageClosure &
cMessageClosure::operator()(const QString &s){
  prefix();
  _msg << s.latin1();
  return *this;
}

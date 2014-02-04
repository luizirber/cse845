#ifndef MESSAGE_DISPLAY_HH
#define MESSAGE_DISPLAY_HH

#ifndef __STRSTREAM__
#include <strstream>
#endif

#ifndef __STRING__
#include <string>
#endif


/*
cMessageDisplay displays the message "final_msg" to the user/programmer.
cMessageDisplay is meant to be subclassed, where
  out(string final_msg)
is reimplemented to display "final_msg" in the desired manner. 

instances of the subclass may then be passed to
  setStandardOutDisplay(cMessageDisplay &)
and
  setStandardErrDisplay(cMessageDisplay &)
with expected results.
*/
class cMessageDisplay{
public:
  virtual void out(string final_msg){}
};
void setInfoDisplay(cMessageDisplay &info);
void setDebugDisplay(cMessageDisplay &debug);
void setErrorDisplay(cMessageDisplay &error);
void setFatalDisplay(cMessageDisplay &fatal);
void setNoPrefixDisplay(cMessageDisplay &noprefix);


class cMessageType;
class cMessageClass{
public:
  cMessageClass(
    const char *class_name,
    cMessageDisplay **msg_display,
    bool is_fatal,
    bool is_prefix,
    bool no_prefix
  );
public:
  void configure(cMessageType *message_type);
public:
  const char *const m_class_name;
  cMessageDisplay **m_msg_display;
  bool const m_is_fatal;
  bool const m_is_prefix;
  bool const m_no_prefix;
private:
  bool _configured;
};


struct cMessage{
  const int m_error;
  const char *m_text;
};


class cMessages{
public:
  cMessages(int len, cMessage messages[])
  : m_messages(messages), m_message_count(len) {}
  const char *describeError(int num);
protected:
  const cMessage *m_messages;
  const int m_message_count;
};


class cMessageType{
public:
  cMessageType(
    const char *type_name,
    cMessageClass &message_class,
    cMessages *messages = 0
  );
public:
  string m_type_name;
  const cMessageClass &m_message_class;
  cMessages *m_messages;
  bool m_is_active;
};


class QString;
class cMessageClosure{
public:
  cMessageClosure(
    cMessageType &type,
    const char *function_name,
    const char *file_name,
    int line_number
  );
  ~cMessageClosure(void);
public:
  ostream &operator()(void){ prefix(); return _msg; }

  /*
  any kind of input can be placed behind the operator(), really, but try
  to not overcrowd the class...

  the operator is meant to convert its input into a string for display.
  if for example you implement
    cMessageClosure &operator()(const myClass &instance);
  then you may write a message thus:
    DemoDebug("this is a myClass instance: ")(my_instance);
  with the result:
    "Debug [Demo] this is a myClass instance: stuff in my_instance."

  look at the file "message_display.cc" for example implementation.
  */
  cMessageClosure &operator()(int error);
  cMessageClosure &operator()(const string &s);
  cMessageClosure &operator()(const char *fmt, ...);
  cMessageClosure &operator()(const QString &s);
private:
  void prefix(void);
private:
  cMessageType &_type;
  const char *_function;
  const char *_file;
  int _line;
  int _error;
  int _op_count;
  strstream _msg;
};


/*
you probably don't need to touch these macros...
*/
#define _MSG_CLOSURE_FL(x)\
  cMessageClosure(\
    x,\
    __PRETTY_FUNCTION__,\
    __FILE__,\
    __LINE__\
  )

#define _INFO_MSG(type)\
  if (Info_ ## type ## _Msg.m_is_active)\
    _MSG_CLOSURE_FL(Info_ ## type ## _Msg)

#define _DEBUG_MSG(type)\
  if (Debug_ ## type ## _Msg.m_is_active)\
    _MSG_CLOSURE_FL(Debug_ ## type ## _Msg)

#define _ERROR_MSG(type)\
  if (Error_ ## type ## _Msg.m_is_active)\
    _MSG_CLOSURE_FL(Error_ ## type ## _Msg)

#define _FATAL_MSG(type)\
  if (Fatal_ ## type ## _Msg.m_is_active)\
    _MSG_CLOSURE_FL(Fatal_ ## type ## _Msg)

#define _PLAIN_MSG(type)\
  if (type ## _Msg.m_is_active)\
    _MSG_CLOSURE_FL(type ## _Msg)

/*
Declaration of the five message classes.
*/
extern cMessageClass MCInfo;
extern cMessageClass MCDebug;
extern cMessageClass MCError;
extern cMessageClass MCFatal;
extern cMessageClass MCNoPrefix;

/*
I've made the one message type "NoPrefix_Msg" permanently available, and
the macro to trigger such a message is "Message".

* added general message types GenInfo, GenDebug, GenError, GenFatal.

each message type that you define should be accompanied by a macro.
message types come in sets of four -- the Info, Debug, Error, and Fatal
types.  for example, to define a "Demo" message type set, declare the
message types like so:
  extern cMessageType Info_Demo_Msg;
  extern cMessageType Debug_Demo_Msg;
  extern cMessageType Error_Demo_Msg;
  extern cMessageType Fatal_Demo_Msg;
with corresponding macros
  #define DemoInfo _INFO_MSG(Demo)
  #define DemoDebug _DEBUG_MSG(Demo)
  #define DemoError _ERROR_MSG(Demo)
  #define DemoFatal _FATAL_MSG(Demo)
and definitions
  cMessageType Info_Demo_Msg("Demo", MCInfo);
  cMessageType Debug_Demo_Msg("Demo", MCDebug);
  cMessageType Error_Demo_Msg("Demo", MCError);
  cMessageType Fatal_Demo_Msg("Demo", MCFatal);
*/
extern cMessageType NoPrefix_Msg;
extern cMessageType Info_GEN_Msg;
extern cMessageType Debug_GEN_Msg;
extern cMessageType Error_GEN_Msg;
extern cMessageType Fatal_GEN_Msg;
#define Message _PLAIN_MSG(NoPrefix)
#define GenInfo _INFO_MSG(GEN)
#define GenDebug _DEBUG_MSG(GEN)
#define GenError _ERROR_MSG(GEN)
#define GenFatal _FATAL_MSG(GEN)


#endif /* !MESSAGE_DISPLAY_HH */

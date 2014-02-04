#include <string.h>
#include <limits.h>
#include <stdarg.h>

#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif


/*
This subclass of cMessageDisplay just prints to stdout and stderr.

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
class cDefaultMessageDisplay : public cMessageDisplay{
public:
  cDefaultMessageDisplay(ostream *stream):_out(stream){}
  void out(string final_msg){ *_out << final_msg; _out->flush(); }
private:
  ostream *_out;
};


/*
default instantiations of cDefaultMessageDisplay...
*/
static cDefaultMessageDisplay default_info(&cout);
static cDefaultMessageDisplay default_debug(&cerr);
static cDefaultMessageDisplay default_error(&cerr);
static cDefaultMessageDisplay default_fatal(&cerr);
static cDefaultMessageDisplay default_noprefix(&cout);


/*
pointers to the cMessageDisplay instantiations actually in use.
each of the message classes gets a handle to one of the cMessageDisplay
instantiations, so that by using the manipulator functions
  void setStandardOutDisplay(cMessageDisplay &out)
and
  void setStandardErrDisplay(cMessageDisplay &err),
the output and error messages can be redirected from, say, the console
to a graphical window and back again (if you want).
*/
static cMessageDisplay *s_info_display = &default_info;
static cMessageDisplay *s_debug_display = &default_debug;
static cMessageDisplay *s_error_display = &default_error;
static cMessageDisplay *s_fatal_display = &default_fatal;
static cMessageDisplay *s_noprefix_display = &default_noprefix;


void setInfoDisplay(cMessageDisplay &info){
  s_info_display = &info;
}
void setDebugDisplay(cMessageDisplay &debug){
  s_debug_display = &debug;
}
void setErrorDisplay(cMessageDisplay &error){
  s_error_display = &error;
}
void setFatalDisplay(cMessageDisplay &fatal){
  s_fatal_display = &fatal;
}
void setNoPrefixDisplay(cMessageDisplay &noprefix){
  s_noprefix_display = &noprefix;
}


/*
these booleans determine whether to print the function, file, and line
number containing the message call.  the booleans are reset according to
the contents of the environment variable "DEBUG_MESSAGING", which should
be a comma-delimited list containing any of the three words
  "chatter,where,function".

I've wired this together so that message types in the classes MCFatal
and MCNoPrefix are always displayed; the remaining messages will only be
displayed if DEBUG_MESSAGING contains "chatter".

The line number and file containing the message call are printed if
DEBUG_MESSAGING contains "where".

The function containing the message call is printed only if
DEBUG_MESSAGING contains "function".

Note that message types of class "MCNoPrefix" will not print the
function, file, or line number.
*/
static bool show_function = false;
static bool show_where = false;
static bool be_quiet = true;


/*
Definition of the five message classes.
*/
cMessageClass MCInfo("Info", &s_info_display, false, false, false);
cMessageClass MCDebug("Debug", &s_debug_display, false, false, false);
cMessageClass MCError("Error", &s_error_display, false, true, false);
cMessageClass MCFatal("Fatal", &s_fatal_display, true, true, false);
cMessageClass MCNoPrefix("NoPrefix", &s_noprefix_display, false, true, true);


/*
Definition of the message type "NoPrefix_Msg".
*/
cMessageType NoPrefix_Msg("NoPrefix", MCNoPrefix);
cMessageType Info_GEN_Msg("General", MCInfo);
cMessageType Debug_GEN_Msg("General", MCDebug);
cMessageType Error_GEN_Msg("General", MCError);
cMessageType Fatal_GEN_Msg("General", MCFatal);


/*
cMessageClass constructor.
*/
cMessageClass::cMessageClass(
  const char *class_name,
  cMessageDisplay **msg_display,
  bool is_fatal,
  bool is_prefix,
  bool no_prefix
):m_class_name(class_name),
  m_msg_display(msg_display),
  m_is_fatal(is_fatal),
  m_is_prefix(is_prefix),
  m_no_prefix(no_prefix),
  _configured(false)
{}


/*
here we configure behavior of each cMessageClass and cMessageType
according the contents of the environment variable DEBUG_MESSAGING.
this is affected by setting the static booleans "show_function,
show_where, be_quiet".

these booleans determine whether to print the function, file, and line
number containing the message call.  the booleans are reset according to
the contents of the environment variable "DEBUG_MESSAGING", which should
be a comma-delimited list containing any of the three words
  "chatter,where,function".

I've wired this together so that message types in the classes MCFatal
and MCNoPrefix are always displayed; the remaining messages will only be
displayed if DEBUG_MESSAGING contains "chatter".

The line number and file containing the message call are printed if
DEBUG_MESSAGING contains "where".

The function containing the message call is printed only if
DEBUG_MESSAGING contains "function".

Note that message types of class "MCNoPrefix" will not print the
function, file, or line number.
*/
void
cMessageClass::configure(cMessageType *message_type){
  if(!_configured){
    _configured = true;
    char *denv = getenv("DEBUG_MESSAGING");
    if(denv){
      denv = strdup(denv);
      char *tok = strtok(denv, ",");
      while(tok){
        if(strcmp(tok,"where") == 0){
          show_where = true;
        } else if (strcmp(tok,"function") == 0){
          show_function = true;
        } else if (strcmp(tok,"chatter") == 0){
          be_quiet = false;
        }
        tok = strtok(NULL,",");
      }
      free(denv);
    }
  }
  if( (this == &MCInfo)
    ||(this == &MCDebug)
    ||(this == &MCError)
  ) message_type->m_is_active = !be_quiet;
}


const char *
cMessages::describeError(int num){
  if(0 <= num && num < m_message_count){ return m_messages[num].m_text; }
  return "Code needs work";
}


/*
cMessageType constructor.
*/
cMessageType::cMessageType(
  const char *type_name,
  cMessageClass &message_class,
  cMessages *messages
):m_type_name(type_name),
  m_message_class(message_class),
  m_messages(messages),
  m_is_active(true)
{ message_class.configure(this); }


/*
cMessageClosure constructor and destructor.
*/
cMessageClosure::cMessageClosure(
  cMessageType &type,
  const char *function_name,
  const char *file_name,
  int line_number
):_type(type),
  _function(function_name),
  _file(file_name),
  _line(line_number),
  _error(INT_MAX),
  _op_count(0)
{}

cMessageClosure::~cMessageClosure(void){
  if(_error != INT_MAX && _type.m_messages){
    _msg << ": ";
    _msg << _type.m_messages->describeError(_error);
    _msg << " (" << _error << ')';
  }
  _msg << '\n';
  _msg << '\0';
  string final_msg(_msg.str());
  _msg.freeze(false);
  (*_type.m_message_class.m_msg_display)->out(final_msg);
  if(_type.m_message_class.m_is_fatal) abort();
}


/*
pretty-printing for cMessageClosure.
*/
void
cMessageClosure::prefix(void){
  if(_op_count++ == 0 && !_type.m_message_class.m_no_prefix){
    if(_type.m_message_class.m_is_prefix){ _msg << _type.m_type_name << '-'; }
    _msg << _type.m_message_class.m_class_name;
    if(!_type.m_message_class.m_is_prefix || show_function || show_where){
      bool colon = false;
      _msg << " [" ;
      if(!_type.m_message_class.m_is_prefix){
        if(colon){_msg << ':';} else {colon = true;}
        _msg << _type.m_type_name;
      }
      if(show_function){
        if(colon){_msg << ':';} else {colon = true;}
        _msg << _function;
      }
      if(show_where){
        if(colon){_msg << ':';}
        _msg << _file << ':' << _line;
      }
      _msg << "] ";
    } else {
      _msg << ':';
    }
  }
}


/*
operator() definitions for cMessageClosure.

any kind of input can be placed behind the operator(), really, but try
to not overcrowd the class...

the operator() is meant to convert its input into a string for display.
if for example you implement
  cMessageClosure &operator()(const myClass &instance);
then you may write a message thus:
  DemoDebug("this is a myClass instance: ")(my_instance);
with the result:
  "Debug [Demo] this is a myClass instance: stuff in my_instance."
*/
cMessageClosure &
cMessageClosure::operator()(int error){
  prefix();
  _error = error;
  //prefix(); _msg << ": code needs work (" << error << ')';
  return *this;
}

cMessageClosure &
cMessageClosure::operator()(const string &s){
  prefix();
  _msg << s;
  return *this;
}

cMessageClosure &
cMessageClosure::operator()(const char *fmt, ...){
  prefix();
  va_list args;
  va_start(args, fmt);
  _msg.vform(fmt, args);
  va_end(args);
  return *this;
}

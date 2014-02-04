//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
// 
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif

#ifndef SETUP_FILE_HH
#include "setup_file.hh"
#endif

avd_SetupFile::avd_SetupFile(
  QString file_name,
  QObject *parent,
  const char *name
)
: m_file_name(file_name), QObject(parent, name)
{
  Message("<avd_SetupFile> constructor.");
  ;
}

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qlabel.h>
#include <qgrid.h>
#include <qframe.h>

#include "labeled_field_convenience.hh"


#ifndef DEBUG
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#else
//# define LOCAL_DEBUG 1
//# define USE_LOCAL_STRINGS 1
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#endif

#if USE_LOCAL_STRINGS
# define DSTR
#else
# define DSTR(ignore...) ("")
#endif


LabeledFieldConvenience::LabeledFieldConvenience(
  const QString &title,
  QGrid *layout_grid,
  const char *name
)
: QObject((QObject *)layout_grid, name)
{
  if(layout_grid == NULL){
    qFatal("<LabeledFieldConvenience> must have non-null layout_grid.\n");
  }
  m_title = new QLabel(title, layout_grid);
  m_data = new QLabel("", layout_grid);
  m_data->setFrameStyle(
    QFrame::Panel | QFrame::Sunken
  );
}

void
LabeledFieldConvenience::setText(const QString &text){
  m_data->setText(text);
}

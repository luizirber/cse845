//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LABELED_FIELD_CONVENIENCE_H
#define LABELED_FIELD_CONVENIENCE_H

#include <qobject.h>


class QGrid;
class QLabel;
class QString;


/**
 * A convenience class to contain both a data field and a label for the
 * data field.  Used by instances of avd_CellViewListItem.
 *
 * @short Convenience class for displaying data in avd_CellViewListItem.
 * @version $Id: labeled_field_convenience.hh,v 1.1 2002/02/12 12:27:46 kaben Exp $
 * @see avd_CellViewListItem
 */
class
LabeledFieldConvenience : public QObject {

  Q_OBJECT

public:

  /**
   * LabeledFieldConvenience constructor.
   *
   * @param title  The title label for the data field.
   * @param grid  The 2x2 layout grid of avd_CellViewListItem.
   * @param name  Qt identifier; used in Qt debugging messages and Designer.
   */
  LabeledFieldConvenience(
    const QString &title,
    QGrid *layout_grid,
    const char *name = 0
  );

  /**
   * Changes the contents of the data field.
   *
   * @param text  The new contents of the data field.
   */
  void setText(const QString &text);

private:

  /**
   * The title label for the data field.
   */
  QLabel *m_title;

  /**
   * The data field.
   */
  QLabel *m_data;

};

#endif /* !LABELED_FIELD_CONVENIENCE_H */

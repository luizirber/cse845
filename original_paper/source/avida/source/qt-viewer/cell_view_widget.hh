//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CELLVIEWWIDGET_HH
#define CELLVIEWWIDGET_HH

#include <qguardedptr.h>

#include "avd_widget_list.hh"


class cPopulation;
class cPopulationCell;
class MessagingPopulationWrapper;
class PopulationCellWrapper;
class PopulationCellListItem;


class
CellViewWidget : public avd_WidgetList {
  Q_OBJECT
public:
  CellViewWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  void setPopulationCellWrapper(
    PopulationCellWrapper *population_cell_wrapper
  );
  void setPopulationWrapper(MessagingPopulationWrapper *pop_wrap);
public slots:
  void updateState(void);
  void setPopulationCell(int cell);
signals:
  void cellSelected_sig(int cell);
  void stateChanged_sig(void);
private:
  QGuardedPtr<PopulationCellWrapper> m_population_cell_wrapper;
  PopulationCellListItem *m_population_cell_list_item;
};


#endif /* !CELLVIEWWIDGET_HH */

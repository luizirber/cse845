//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LAST_PROGRESS_LIST_ITEM_H
#define LAST_PROGRESS_LIST_ITEM_H

#include "avd_cell_view_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;

// avida declarations
class cPhenotype;


class LastProgressListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  LastProgressListItem(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  void setPopulationCellWrapper(
    PopulationCellWrapper *population_cell_wrapper
  );
public slots:
  void updateState(PopulationCellWrapper *cw);
protected:
  LabeledFieldConvenience *m_last_merit_base;
  LabeledFieldConvenience *m_last_bonus;
  LabeledFieldConvenience *m_last_num_errors;
};


#endif /* !LAST_PROGRESS_LIST_ITEM_H */

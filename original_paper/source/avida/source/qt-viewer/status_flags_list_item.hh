//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef STATUS_FLAGS_LIST_ITEM_H
#define STATUS_FLAGS_LIST_ITEM_H

#include "avd_cell_view_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;

// avida declarations
class cPhenotype;


class StatusFlagsListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  StatusFlagsListItem(
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
  LabeledFieldConvenience *m_is_injected;
  LabeledFieldConvenience *m_is_parasite;
  LabeledFieldConvenience *m_is_modifier;
  LabeledFieldConvenience *m_is_modified;
  LabeledFieldConvenience *m_is_fertile;
  LabeledFieldConvenience *m_is_mutated;
  LabeledFieldConvenience *m_parent_true;
};


#endif /* !STATUS_FLAGS_LIST_ITEM_H */

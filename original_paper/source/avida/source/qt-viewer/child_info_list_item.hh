//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CHILD_INFO_LIST_ITEM_H
#define CHILD_INFO_LIST_ITEM_H

#include "avd_cell_view_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;

// avida declarations
class cPhenotype;


class ChildInfoListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  ChildInfoListItem(
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
  LabeledFieldConvenience *m_copy_true;
  LabeledFieldConvenience *m_child_fertile;
  LabeledFieldConvenience *m_child_copied_size;
};


#endif /* !CHILD_INFO_LIST_ITEM_H */

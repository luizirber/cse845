//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef PHENOTYPE_LIST_ITEM_H
#define PHENOTYPE_LIST_ITEM_H

#include "avd_cell_view_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;

// avida declarations
class cPhenotype;

// subwidget declarations
class PhenotypeLastDivideListItem;
class CurrentProgressListItem;
class LastProgressListItem;
class LifetimeListItem;
class StatusFlagsListItem;
class ChildInfoListItem;


class PhenotypeListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  PhenotypeListItem(
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
  PhenotypeLastDivideListItem *m_last_divide_list_item;
  //CurrentProgressListItem *m_current_progress_list_item;
  LastProgressListItem *m_last_progress_list_item;
  LifetimeListItem *m_lifetime_list_item;
  StatusFlagsListItem *m_status_flags_list_item;
  ChildInfoListItem *m_child_info_list_item;
};


#endif /* !PHENOTYPE_LIST_ITEM_H */

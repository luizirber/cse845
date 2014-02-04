//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATIONCELLLISTITEM_HH
#define POPULATIONCELLLISTITEM_HH

#include "avd_cell_view_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;

// avida declarations
class cPopulationCell;

// subwidget declarations
class OrganismListItem;
class MutationRatesListItem;
class InputsListItem;


class PopulationCellListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  PopulationCellListItem(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  void setPopulationCellWrapper(
    PopulationCellWrapper *population_cell_wrapper
  );
public slots:
  void updateState(PopulationCellWrapper *cell);
protected:
  OrganismListItem *m_organism_list_item;
  MutationRatesListItem *m_mutation_rates_list_item;
  InputsListItem *m_inputs_list_item;

  LabeledFieldConvenience *m_cell_id;
  LabeledFieldConvenience *m_organism_count;
  LabeledFieldConvenience *m_IsOccupied;
};


#endif /* !POPULATIONCELLLISTITEM_HH */

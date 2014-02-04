//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef PHENOTYPE_LAST_DIVIDE_LIST_ITEM_H
#define PHENOTYPE_LAST_DIVIDE_LIST_ITEM_H

#include "avd_cell_view_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;

// avida declarations
class cPhenotype;


class PhenotypeLastDivideListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  PhenotypeLastDivideListItem(
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
  LabeledFieldConvenience *m_genome_length;
  LabeledFieldConvenience *m_copied_size;
  LabeledFieldConvenience *m_executed_size;
  LabeledFieldConvenience *m_gestation_time;
  LabeledFieldConvenience *m_gestation_start;
  LabeledFieldConvenience *m_fitness;
};


#endif /* !PHENOTYPE_LAST_DIVIDE_LIST_ITEM_H */

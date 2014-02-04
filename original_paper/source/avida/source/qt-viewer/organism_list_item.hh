//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ORGANISMLISTITEM_HH
#define ORGANISMLISTITEM_HH

#include "avd_cell_view_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;

// avida declarations
class cOrganism;

// subwidget declarations
class PhenotypeListItem;
class HardwareListItem;


class OrganismListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  OrganismListItem(
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
  HardwareListItem *m_hardware_list_item;
  PhenotypeListItem *m_phenotype_list_item;
  
  LabeledFieldConvenience *m_max_executed;
  LabeledFieldConvenience *m_lineage_label;
  LabeledFieldConvenience *m_cur_gestation;
  LabeledFieldConvenience *m_instance_count;
};


#endif /* !ORGANISMLISTITEM_HH */

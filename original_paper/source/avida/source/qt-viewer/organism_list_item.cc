//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

/* qt gui classes */
#include <qgrid.h>

/* avida gui classes */
#include "population_cell_wrapped_accessors.hh"
#include "population_cell_wrapper.hh"
#include "labeled_field_convenience.hh"

/* avida core classes */
#include "organism.hh"

/* main class declaration */
#include "organism_list_item.hh"

/* subwidgets */
#include "hardware_list_item.hh"
#include "phenotype_list_item.hh"


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

OrganismListItem::OrganismListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<OrganismListItem::OrganismListItem>\n"
  << " --- created new OrganismListItem.\n";
  #endif

  setLabelText("Organism");

  m_max_executed = new LabeledFieldConvenience(
    "max_executed",
    m_grid
  );

  m_lineage_label = new LabeledFieldConvenience(
    "lineage_label",
    m_grid
  );

  m_cur_gestation = new LabeledFieldConvenience(
    "cur_gestation",
    m_grid
  );

  m_instance_count = new LabeledFieldConvenience(
    "instance_count",
    m_grid
  );

  m_hardware_list_item = new HardwareListItem();
  insertItem(m_hardware_list_item);

  m_phenotype_list_item = new PhenotypeListItem();
  insertItem(m_phenotype_list_item);
}

void
OrganismListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<OrganismListItem::updateState>\n"
  << " --- caught signal.\n";
  #endif
  
  cOrganism *organism =
    GetOrganism(
      GetPopulationCell(cw)
    );

  if(organism == NULL){
    m_max_executed->setText("");
    m_lineage_label->setText("");
    m_cur_gestation->setText("");
    m_instance_count->setText("");
  }else{
    m_max_executed->setText(
      QString("%1").arg(
        organism->GetMaxExecuted()
      )
    );
    m_lineage_label->setText(
      QString("%1").arg(
        organism->GetLineageLabel()
      )
    );
    m_cur_gestation->setText(
      /*
      QString("%1").arg(
        organism->GetCurGestation()
      )
      */
      "(not yet used)"
    );
    m_instance_count->setText(
      QString("%1").arg(
        organism->GetInstanceCount()
      )
    );
  }
}


void
OrganismListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_hardware_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_phenotype_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
}


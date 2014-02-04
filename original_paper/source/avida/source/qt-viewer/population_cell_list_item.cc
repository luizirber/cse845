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
#include "population_cell.hh"

/* main class declaration */
#include "population_cell_list_item.hh"

/* subwidgets */
#include "organism_list_item.hh"
#include "mutation_rates_list_item.hh"
#include "inputs_list_item.hh"


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


PopulationCellListItem::PopulationCellListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<PopulationCellListItem::PopulationCellListItem>\n"
  << " --- created new PopulationCellListItem.\n";
  #endif

  setLabelText("Population Cell");

  m_cell_id = new LabeledFieldConvenience(
    "cell_id",
    m_grid
  );

  m_organism_count = new LabeledFieldConvenience(
    "organism_count",
    m_grid
  );

  m_IsOccupied = new LabeledFieldConvenience(
    "IsOccupied",
    m_grid
  );

  m_organism_list_item = new OrganismListItem();
  insertItem(m_organism_list_item);

  m_mutation_rates_list_item = new MutationRatesListItem();
  insertItem(m_mutation_rates_list_item);

  m_inputs_list_item = new InputsListItem();
  insertItem(m_inputs_list_item);
}


void
PopulationCellListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<PopulationCellListItem::updateState>\n"
  << " --- caught signal.\n";
  #endif

  cPopulationCell *cell = GetPopulationCell(cw);

  if(cell == NULL){
    m_cell_id->setText("");
    m_organism_count->setText("");
    m_IsOccupied->setText("");
  }else{
    m_cell_id->setText(
      QString("%1").arg(
        cell->GetID()
      )
    );
    m_organism_count->setText(
      QString("%1").arg(
        cell->GetOrganismCount()
      )
    );
    if(cell->IsOccupied()){
      m_IsOccupied->setText("true");
    }else{
      m_IsOccupied->setText("false");
    }
  }
}


void
PopulationCellListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_organism_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_mutation_rates_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_inputs_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
}


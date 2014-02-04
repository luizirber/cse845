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
#include "phenotype.hh"

/* main class declaration */
#include "phenotype_list_item.hh"

/* subwidgets */
#include "phenotype_last_divide_list_item.hh"
//#include "current_progress_list_item.hh"
#include "last_progress_list_item.hh"
#include "lifetime_list_item.hh"
#include "status_flags_list_item.hh"
#include "child_info_list_item.hh"


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


PhenotypeListItem::PhenotypeListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<PhenotypeListItem::PhenotypeListItem>\n"
  << " --- created new PhenotypeListItem.\n";
  #endif

  setLabelText("Phenotype");

  m_last_divide_list_item = new PhenotypeLastDivideListItem();
  insertItem(m_last_divide_list_item);

  //m_current_progress_list_item = new CurrentProgressListItem();
  //insertItem(m_current_progress_list_item);

  m_last_progress_list_item = new LastProgressListItem();
  insertItem(m_last_progress_list_item);

  m_lifetime_list_item = new LifetimeListItem();
  insertItem(m_lifetime_list_item);

  m_status_flags_list_item = new StatusFlagsListItem();
  insertItem(m_status_flags_list_item);

  m_child_info_list_item = new ChildInfoListItem();
  insertItem(m_child_info_list_item);
}


void
PhenotypeListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<PhenotypeListItem::updateState>\n"
  << " --- caught signal.\n";
  #endif
}


void
PhenotypeListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_last_divide_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
  //m_current_progress_list_item->setPopulationCellWrapper(
  //  population_cell_wrapper
  //);
  m_last_progress_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_lifetime_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_status_flags_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
  m_child_info_list_item->setPopulationCellWrapper(
    population_cell_wrapper
  );
}



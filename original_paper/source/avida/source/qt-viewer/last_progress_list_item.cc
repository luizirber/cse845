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
#include "last_progress_list_item.hh"



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

LastProgressListItem::LastProgressListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<LastProgressListItem::LastProgressListItem>\n"
  << " --- created new CurrentProgressListItem.\n";
  #endif

  setLabelText("last progress");

  m_last_merit_base = new LabeledFieldConvenience(
    "last merit base",
    m_grid
  );
  m_last_bonus = new LabeledFieldConvenience(
    "last bonus",
    m_grid
  );
  m_last_num_errors = new LabeledFieldConvenience(
    "last number of errors",
    m_grid
  );
}

void
LastProgressListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<LastProgressListItem::LastProgressListItem>\n"
  << " --- caught signal.\n";
  #endif
  
  cPhenotype *phenotype =
    GetPhenotype(
      GetOrganism(
        GetPopulationCell(cw)
      )
    );

  if(phenotype == NULL){
    m_last_merit_base->setText("");
    m_last_bonus->setText("");
    m_last_num_errors->setText("");
  }else{
    m_last_merit_base->setText(
      QString("%1").arg(
        phenotype->GetLastMeritBase()
      )
    );
    m_last_bonus->setText(
      QString("%1").arg(
        phenotype->GetLastBonus()
      )
    );
    m_last_num_errors->setText(
      QString("%1").arg(
        phenotype->GetCurNumErrors()
      )
    );
  }
}

void
LastProgressListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
}


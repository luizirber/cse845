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
#include "status_flags_list_item.hh"


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

StatusFlagsListItem::StatusFlagsListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<StatusFlagsListItem::StatusFlagsListItem>\n"
  << " --- created new StatusFlagsListItem.\n";
  #endif

  setLabelText("status flags");

  m_is_injected = new LabeledFieldConvenience(
    "is_injected",
    m_grid
  );
  m_is_parasite = new LabeledFieldConvenience(
    "is_parasite",
    m_grid
  );
  m_is_modifier = new LabeledFieldConvenience(
    "is_modifier",
    m_grid
  );
  m_is_modified = new LabeledFieldConvenience(
    "is_modified",
    m_grid
  );
  m_is_fertile = new LabeledFieldConvenience(
    "is_fertile",
    m_grid
  );
  m_is_mutated = new LabeledFieldConvenience(
    "is_mutated",
    m_grid
  );
  m_parent_true = new LabeledFieldConvenience(
    "parent_true",
    m_grid
  );
}

void
StatusFlagsListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<StatusFlagsListItem::updateState>\n"
  << " --- caught signal.\n";
  #endif
  
  cPhenotype *phenotype =
    GetPhenotype(
      GetOrganism(
        GetPopulationCell(cw)
      )
    );

  if(phenotype == NULL){
    m_is_injected->setText("");
    m_is_parasite->setText("");
    m_is_modifier->setText("");
    m_is_modified->setText("");
    m_is_fertile->setText("");
    m_is_mutated->setText("");
    m_parent_true->setText("");
  }else{
    if (phenotype->IsInjected()) m_is_injected->setText("true");
    else m_is_injected->setText("false");

    if (phenotype->IsParasite()) m_is_parasite->setText("true");
    else m_is_parasite->setText("false");

    if (phenotype->IsModifier()) m_is_modifier->setText("true");
    else m_is_modifier->setText("false");

    if (phenotype->IsModified()) m_is_modified->setText("true");
    else m_is_modified->setText("false");

    if (phenotype->IsFertile()) m_is_fertile->setText("true");
    else m_is_fertile->setText("false");

    if (phenotype->IsMutated()) m_is_mutated->setText("true");
    else m_is_mutated->setText("false");

    if (phenotype->ParentTrue()) m_parent_true->setText("true");
    else m_parent_true->setText("false");
  }
}

void
StatusFlagsListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
}


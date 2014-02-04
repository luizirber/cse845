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

ChildInfoListItem::ChildInfoListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<ChildInfoListItem::ChildInfoListItem>\n"
  << " --- created new ChildInfoListItem.\n";
  #endif

  setLabelText("child info");

  m_copy_true = new LabeledFieldConvenience(
    "copy_true",
    m_grid
  );
  m_child_fertile = new LabeledFieldConvenience(
    "child_fertile",
    m_grid
  );
  m_child_copied_size = new LabeledFieldConvenience(
    "child_copied_size",
    m_grid
  );
}

void
ChildInfoListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<ChildInfoListItem::updateState>\n"
  << " --- caught signal.\n";
  #endif
  
  cPhenotype *phenotype =
    GetPhenotype(
      GetOrganism(
        GetPopulationCell(cw)
      )
    );

  if(phenotype == NULL){
    m_copy_true->setText("");
    m_child_fertile->setText("");
    m_child_copied_size->setText("");
  }else{
    if (phenotype->CopyTrue()) m_copy_true->setText("true");
    else m_copy_true->setText("false");

    if (phenotype->ChildFertile()) m_child_fertile->setText("true");
    else m_child_fertile->setText("false");

    m_child_copied_size->setText(
      QString("%1").arg(
        phenotype->GetChildCopiedSize()
      )
    );
  }
}

void
ChildInfoListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
}


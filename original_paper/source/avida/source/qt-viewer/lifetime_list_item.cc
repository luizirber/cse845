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
#include "lifetime_list_item.hh"



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

LifetimeListItem::LifetimeListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<LifetimeListItem::LifetimeListItem>\n"
  << " --- created new LifetimeListItem.\n";
  #endif

  setLabelText("lifetime");

  m_num_divides = new LabeledFieldConvenience(
    "num_divides",
    m_grid
  );
  m_generation = new LabeledFieldConvenience(
    "generation",
    m_grid
  );
  m_time_used = new LabeledFieldConvenience(
    "time_used",
    m_grid
  );
  m_age = new LabeledFieldConvenience(
    "age",
    m_grid
  );
  m_fault_desc = new LabeledFieldConvenience(
    "fault_desc",
    m_grid
  );
  m_neutral_metric = new LabeledFieldConvenience(
    "neutral_metric",
    m_grid
  );
}

void
LifetimeListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<LifetimeListItem::updateState>\n"
  << " --- caught signal.\n";
  #endif
  
  cPhenotype *phenotype =
    GetPhenotype(
      GetOrganism(
        GetPopulationCell(cw)
      )
    );

  if(phenotype == NULL){
    m_num_divides->setText("");
    m_generation->setText("");
    m_time_used->setText("");
    m_age->setText("");
    m_fault_desc->setText("");
    m_neutral_metric->setText("");
  }else{
    m_num_divides->setText(
      QString("%1").arg(
        phenotype->GetNumDivides()
      )
    );
    m_generation->setText(
      QString("%1").arg(
        phenotype->GetGeneration()
      )
    );
    m_time_used->setText(
      QString("%1").arg(
        phenotype->GetTimeUsed()
      )
    );
    m_age->setText(
      QString("%1").arg(
        phenotype->GetAge()
      )
    );
    m_fault_desc->setText(
      phenotype->GetFault().GetData()
    );
    m_neutral_metric->setText(
      QString("%1").arg(
        phenotype->GetNeutralMetric()
      )
    );
  }
}

void
LifetimeListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
}


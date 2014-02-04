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
#include "phenotype_last_divide_list_item.hh"



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

PhenotypeLastDivideListItem::PhenotypeLastDivideListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<PhenotypeLastDivideListItem::PhenotypeLastDivideListItem>\n"
  << " --- created new PhenotypeLastDivideListItem.\n";
  #endif

  setLabelText("Last Divide");

  m_genome_length = new LabeledFieldConvenience(
    "max_executed",
    m_grid
  );

  m_copied_size = new LabeledFieldConvenience(
    "copied_size",
    m_grid
  );

  m_executed_size = new LabeledFieldConvenience(
    "executed_size",
    m_grid
  );

  m_gestation_time = new LabeledFieldConvenience(
    "gestation_time",
    m_grid
  );

  m_gestation_start = new LabeledFieldConvenience(
    "gestation_start",
    m_grid
  );

  m_fitness = new LabeledFieldConvenience(
    "fitness",
    m_grid
  );
}

void
PhenotypeLastDivideListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<PhenotypeLastDivideListItem::updateState>\n"
  << " --- caught signal.\n";
  #endif
  
  cPhenotype *phenotype =
    GetPhenotype(
      GetOrganism(
        GetPopulationCell(cw)
      )
    );

  if(phenotype == NULL){
    m_genome_length->setText("");
    m_copied_size->setText("");
    m_executed_size->setText("");
    m_gestation_time->setText("");
    m_gestation_start->setText("");
    m_fitness->setText("");
  }else{
    m_genome_length->setText(
      QString("%1").arg(
        phenotype->GetGenomeLength()
      )
    );
    m_copied_size->setText(
      QString("%1").arg(
        phenotype->GetCopiedSize()
      )
    );
    m_executed_size->setText(
      QString("%1").arg(
        phenotype->GetExecutedSize()
      )
    );
    m_gestation_time->setText(
      QString("%1").arg(
        phenotype->GetGestationTime()
      )
    );
    m_gestation_start->setText(
      QString("%1").arg(
        phenotype->GetGestationStart()
      )
    );
    m_fitness->setText(
      QString("%1").arg(
        phenotype->GetFitness()
      )
    );
  }
}

void
PhenotypeLastDivideListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
}


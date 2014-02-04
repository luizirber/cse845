//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

/* qt gui classes */
#include <qgrid.h>

/* avida core classes */
#include "mutations.hh"

/* avida gui classes */
#include "population_cell_wrapped_accessors.hh"
#include "population_cell_wrapper.hh"
#include "labeled_field_convenience.hh"

/* avida core classes */
#include "mutations.hh"

/* main class declaration */
#include "mutation_rates_list_item.hh"

/* subwidgets */



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

MutationRatesListItem::MutationRatesListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<GetMutationRates::GetMutationRates>\n"
  << " --- created new GetMutationRates.\n";
  #endif

  setLabelText("Mutation Rates");

  m_copy_mut_prob = new LabeledFieldConvenience(
    "CopyMutProb",
    m_grid
  );

  m_ins_mut_prob = new LabeledFieldConvenience(
    "InsMutProb",
    m_grid
  );

  m_del_mut_prob = new LabeledFieldConvenience(
    "DelMutProb",
    m_grid
  );

  m_point_mut_prob = new LabeledFieldConvenience(
    "PointMutProb",
    m_grid
  );

  m_divide_mut_prob = new LabeledFieldConvenience(
    "DivideMutProb",
    m_grid
  );

  m_divide_ins_prob = new LabeledFieldConvenience(
    "DivideInsProb",
    m_grid
  );

  m_divide_del_prob = new LabeledFieldConvenience(
    "DivideDelProb",
    m_grid
  );

  m_crossover_prob = new LabeledFieldConvenience(
    "CrossoverProb",
    m_grid
  );

  m_aligned_cross_prob = new LabeledFieldConvenience(
    "AlignedCrossProb",
    m_grid
  );

  m_exe_err_prob = new LabeledFieldConvenience(
    "ExeErrProb",
    m_grid
  );
}

void
MutationRatesListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<OrganismListItem::updateState>\n"
  << " --- caught signal.\n";
  #endif
  
  cMutationRates *mutations =
    GetMutationRates(
      GetPopulationCell(cw)
    );

  if(mutations == NULL){
    m_copy_mut_prob->setText("");
    m_ins_mut_prob->setText("");
    m_del_mut_prob->setText("");
    m_point_mut_prob->setText("");
    m_divide_mut_prob->setText("");
    m_divide_ins_prob->setText("");
    m_divide_del_prob->setText("");
    m_crossover_prob->setText("");
    m_aligned_cross_prob->setText("");
    m_exe_err_prob->setText("");
  }else{
    m_copy_mut_prob->setText(
      QString("%1").arg(
        mutations->GetCopyMutProb()
      )
    );
    m_ins_mut_prob->setText(
      QString("%1").arg(
        mutations->GetInsMutProb()
      )
    );
    m_del_mut_prob->setText(
      QString("%1").arg(
        mutations->GetDelMutProb()
      )
    );
    m_point_mut_prob->setText(
      QString("%1").arg(
        mutations->GetPointMutProb()
      )
    );
    m_divide_mut_prob->setText(
      QString("%1").arg(
        mutations->GetDivideMutProb()
      )
    );
    m_divide_ins_prob->setText(
      QString("%1").arg(
        mutations->GetDivideInsProb()
      )
    );
    m_divide_del_prob->setText(
      QString("%1").arg(
        mutations->GetDivideDelProb()
      )
    );
    m_crossover_prob->setText(
      QString("%1").arg(
        mutations->GetCrossoverProb()
      )
    );
    m_aligned_cross_prob->setText(
      QString("%1").arg(
        mutations->GetAlignedCrossProb()
      )
    );
    m_exe_err_prob->setText(
      QString("%1").arg(
        mutations->GetExeErrProb()
      )
    );
  }
}

void
MutationRatesListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
}


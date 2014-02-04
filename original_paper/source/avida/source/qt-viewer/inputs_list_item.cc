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
#include "inputs_list_item.hh"

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

InputsListItem::InputsListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_CellViewListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<InputsListItem::InputsListItem>\n"
  << " --- created new InputsListItem.\n";
  #endif

  setLabelText("Inputs");

  m_input_vector.resize(IO_SIZE); // IO_SIZE comes from cpu/cpu_defs.hh
  #if LOCAL_DEBUG
  cout
  << "<InputsListItem::InputsListItem>\n"
  << " --- input_vector size: "
  << m_input_vector.size()
  << "\n";
  #endif

  for(int i = 0; i < IO_SIZE; i++){
    m_input_vector[i] = new LabeledFieldConvenience(
      QString("Input %1").arg(i),
      m_grid
    );
  }
}

void
InputsListItem::updateState(PopulationCellWrapper *cw){
  #if LOCAL_DEBUG
  cout
  << "<InputsListItem::InputsListItem>\n"
  << " --- caught signal.\n";
  #endif
  
  cPopulationCell *cell = GetPopulationCell(cw);

  if(cell == NULL){
    for(int i = 0; i < IO_SIZE; i++){
      m_input_vector[i]->setText("");
    }
  }else{
    for(int i = 0; i < IO_SIZE; i++){
      m_input_vector[i]->setText(
        QString("%1").arg(
          cell->GetInput(i)
        )
      );
    }
  }
}

void
InputsListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  avd_CellViewListItem::setPopulationCellWrapper(
    population_cell_wrapper
  );
}


//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qgrid.h>

#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif

#include "population_cell.hh"
#include "population_cell_wrapper.hh"

#include "avd_cell_view_list_item.hh"


avd_CellViewListItem::avd_CellViewListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_WidgetListItem(parent, name, f)
{

  GenDebug(
    "<avd_CellViewListItem::avd_CellViewListItem>\n"
    " --- created new avd_CellViewListItem.\n"
  );

  m_grid = new QGrid( 2, this);
  insertItem(m_grid);
}


void
avd_CellViewListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  GenDebug(
    "<avd_CellViewListItem::setPopulationCellWrapper>\n"
    " --- caught signal.\n"
    " --- population_cell_wrapper ptr: %s\n",
    population_cell_wrapper
  );

  if(m_population_cell_wrapper){
    disconnect(
      m_population_cell_wrapper, 0,
      this, 0
    );
  }
  m_population_cell_wrapper = population_cell_wrapper;

  /*
   * Note:  the SLOT updateState(cPopulationCell *) isn't actually
   * declared for this class!  trying to instantiate the class
   * won't break anything, but Qt will complain.  Subclasses of this
   * class work fine if they declare and define the slot.
   */
  connect(
    population_cell_wrapper,
    SIGNAL(stateChanged_sig(PopulationCellWrapper *)),
    this,
    SLOT(updateState(PopulationCellWrapper *))
  );
}


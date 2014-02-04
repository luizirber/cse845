//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "population_cell.hh"
//#include "population.hh"

#include "population_wrapper.hh"
#include "population_cell_wrapper.hh"
#include "population_cell_list_item.hh"

#include "cell_view_widget.hh"


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


CellViewWidget::CellViewWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_WidgetList(parent, name, f)
{
  #if LOCAL_DEBUG
  cout
  << "<CellViewWidget::CellViewWidget>\n"
  << " --- created new CellViewWidget.\n";
  #endif

  m_population_cell_list_item = new PopulationCellListItem(
    this,
    DSTR("m_population_cell_list_item")
  );
  insertItem(m_population_cell_list_item);

  setPopulationCellWrapper(
    new PopulationCellWrapper(
      this,
      DSTR("m_population_cell_wrapper")
    )
  );
  setPopulationWrapper(0);
}


void
CellViewWidget::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  #if LOCAL_DEBUG
  cout
  << "<CellViewWidget::setPopulationCellWrapper>\n"
  << " --- population_cell_wrapper ptr: "
  << population_cell_wrapper
  << "\n";
  #endif

  if(m_population_cell_wrapper){
    disconnect(
      this, 0,
      m_population_cell_wrapper, 0
    );
  }
  m_population_cell_wrapper = population_cell_wrapper;

  connect(
    this,
    SIGNAL(stateChanged_sig()),
    m_population_cell_wrapper,
    SLOT(updateState())
  );
  connect(
    this,
    SIGNAL(cellSelected_sig(int)),
    m_population_cell_wrapper,
    SLOT(setPopulationCell(int))
  );

  m_population_cell_list_item->setPopulationCellWrapper(
    m_population_cell_wrapper
  );
}


void
CellViewWidget::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  #if LOCAL_DEBUG
  cout
  << "<CellViewWidget::setPopulationWrapper>\n"
  << " --- entered.\n";
  #endif

  if(m_population_cell_wrapper){
    m_population_cell_wrapper->setPopulationWrapper(pop_wrap);
  }
}


void
CellViewWidget::updateState(void){
  #if LOCAL_DEBUG
  cout
  << "<CellViewWidget::updateState>\n"
  << " --- caught signal.\n";
  #endif
  
  emit(stateChanged_sig());
}


void
CellViewWidget::setPopulationCell(int cell){
  #if LOCAL_DEBUG
  cout
  << "<CellViewWidget::setPopulationCell>\n"
  << " --- caught signal.\n";
  #endif
  
  emit(cellSelected_sig(cell));
}


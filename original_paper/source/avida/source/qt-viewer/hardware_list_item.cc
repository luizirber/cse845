//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

/* qt gui classes */
#include <qvbox.h>

/* avida gui classes */
#include "population_cell_wrapped_accessors.hh"
#include "population_cell_wrapper.hh"
#include "labeled_field_convenience.hh"

/* avida core classes */
#include "organism.hh"
#include "hardware_cpu.hh"

// temporary
#include "population_cell.hh"
#include <qpushbutton.h>

/* main class declaration */
#include "hardware_list_item.hh"

/* subwidgets */
#include "code_widget.hh"


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


HardwareListItem::HardwareListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: avd_WidgetListItem(parent, name, f)
{

  #if LOCAL_DEBUG
  cout
  << "<HardwareListItem::HardwareListItem>\n"
  << " --- created new HardwareListItem.\n";
  #endif

  setLabelText("like, um... Hardware");

  m_vbox = new QVBox(this, DSTR("m_vbox"));
  insertItem(m_vbox);

  //m_code_view = new CodeWidget(m_vbox, DSTR("m_code_view"));


  m_temporary_start_button = new QPushButton("Start", 0);
  insertItem(m_temporary_start_button);

  m_temporary_stop_button = new QPushButton("Stop", 0);
  insertItem(m_temporary_stop_button);

  m_temporary_step_button = new QPushButton("Step", 0);
  connect(
    m_temporary_step_button, SIGNAL(clicked()),
    this, SLOT(temporaryStepSlot())
  );
  insertItem(m_temporary_step_button);

  m_temporary_update_button = new QPushButton("Update", 0);
  insertItem(m_temporary_update_button);
}


void
HardwareListItem::temporaryStepSlot(void){
  emit(
    temporaryStepSig(
      m_population_cell_wrapper->getPopulationCell()->GetID()
    )
  );
}


void
HardwareListItem::updateState(PopulationCellWrapper *cw){
  //if (m_code_view) m_code_view->updateState(cw);
}


void
HardwareListItem::setPopulationCellWrapper(
  PopulationCellWrapper *population_cell_wrapper
){
  if(m_population_cell_wrapper){
    disconnect(
      m_population_cell_wrapper, 0,
      this, 0
    );
    disconnect(
      m_temporary_start_button, 0,
      m_population_cell_wrapper, 0
    );
    disconnect(
      m_temporary_stop_button, 0,
      m_population_cell_wrapper, 0
    );
    disconnect(
      this, 0,
      m_population_cell_wrapper, 0
    );
    disconnect(
      m_temporary_update_button, 0,
      m_population_cell_wrapper, 0
    );
  }

  m_population_cell_wrapper = population_cell_wrapper;

  if(m_population_cell_wrapper){
    connect(
      population_cell_wrapper,
      SIGNAL(stateChanged_sig(PopulationCellWrapper *)),
      this,
      SLOT(updateState(PopulationCellWrapper *))
    );

    connect(
      m_temporary_start_button, SIGNAL(clicked()),
      m_population_cell_wrapper, SIGNAL(temporaryStartSig())
    );
    connect(
      m_temporary_stop_button, SIGNAL(clicked()),
      m_population_cell_wrapper, SIGNAL(temporaryStopSig())
    );
    connect(
      this, SIGNAL(temporaryStepSig(int)),
      m_population_cell_wrapper, SIGNAL(temporaryStepSig(int))
    );
    connect(
      m_temporary_update_button, SIGNAL(clicked()),
      m_population_cell_wrapper, SIGNAL(temporaryUpdateSig())
    );
  }
}


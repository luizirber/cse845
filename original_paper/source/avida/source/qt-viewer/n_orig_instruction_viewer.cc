//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qvbox.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qpushbutton.h>

#include "messaging_population_wrapper.hh"
#include "n_orig_instruction_view_widget.hh"

#ifndef N_ORIG_INSTRUCTION_BUTTONS_WIDGET_HH
#include "n_orig_instruction_buttons_widget.hh"
#endif

#ifndef N_ORIG_INSTRUCTION_CPU_WIDGET_HH
#include "n_orig_instruction_cpu_widget.hh"
#endif

#include "n_orig_instruction_viewer.hh"


N_Instruction_Viewer::N_Instruction_Viewer(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  m_hboxlayout1 = new QHBoxLayout(this);

  m_vboxlayout1 = new QVBoxLayout(m_hboxlayout1);

  m_instruction_view_widget = new N_Instruction_ViewWidget(this);
  //m_instruction_view_widget->setSizePolicy(
  //  QSizePolicy(
  //    QSizePolicy::Fixed,
  //    QSizePolicy::Ignored
  //  )
  //);
  m_vboxlayout1->addWidget(m_instruction_view_widget);

  m_instruction_buttons_widget = new N_Instruction_ButtonsWidget(this);
  m_vboxlayout1->addWidget(m_instruction_buttons_widget);

  m_vboxlayout2 = new QVBoxLayout(m_hboxlayout1);

  m_instruction_cpu_widget = new N_Instruction_CPUWidget(this);
  m_vboxlayout2->addWidget(m_instruction_cpu_widget);

  m_vboxlayout2->addItem(new QSpacerItem(20, 20));

  //m_instruction_view_widget->adjustSize();
  //setFixedWidth(m_instruction_view_widget->width());

  setCaption("Instruction Viewer");
}

N_Instruction_Viewer::~N_Instruction_Viewer(void){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_Viewer::~N_Instruction_Viewer> destructor called.\n";
  #endif
}

void
N_Instruction_Viewer::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  m_pop_wrap = pop_wrap;
  m_instruction_view_widget->setPopulationWrapper(pop_wrap);
  m_instruction_buttons_widget->setPopulationWrapper(pop_wrap);
  m_instruction_cpu_widget->setPopulationWrapper(pop_wrap);

  // prepare connections from avida.
  connect(
    pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaBreakpointSig(int)),
    this, SLOT(setPopulationCell(int))
  );


  // disable unsafe buttons while avida is working.
  connect(
    pop_wrap, SIGNAL(startAvidaSig()),
    this, SLOT(disableUnsafeButtonsSlot())
  );
  connect(
    pop_wrap, SIGNAL(updateAvidaSig()),
    this, SLOT(disableUnsafeButtonsSlot())
  );
  connect(
    pop_wrap, SIGNAL(stepAvidaSig(int)),
    this, SLOT(disableUnsafeButtonsSlot())
  );

  // enable unsafe buttons when avida is done.
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(enableUnsafeButtonsSlot())
  );
  connect(
    pop_wrap, SIGNAL(avidaBreakpointSig(int)),
    this, SLOT(enableUnsafeButtonsSlot())
  );
  connect(
    pop_wrap, SIGNAL(avidaStoppedSig()),
    this, SLOT(enableUnsafeButtonsSlot())
  );

  connect(
    pop_wrap, SIGNAL(guiStatusSig(bool)),
    this, SLOT(guiStatusSlot(bool))
  );
  connect(
    this, SIGNAL(queryGUISig()),
    pop_wrap, SLOT(queryGUISlot())
  );

  emit queryGUISig();
}

void
N_Instruction_Viewer::localStepSlot(void){
  emit localStepSig(m_cell_id);
}

void
N_Instruction_Viewer::setPopulationCell(int cell_id){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_Viewer::setPopulationCell> cell_id "
  << cell_id << ".\n";
  #endif

  m_cell_id = cell_id;
  m_instruction_view_widget->setPopulationCell(cell_id);
  m_instruction_buttons_widget->setPopulationCell(cell_id);
  m_instruction_cpu_widget->setPopulationCell(cell_id);
}

void
N_Instruction_Viewer::updateState(void){
  m_instruction_view_widget->updateState();
}

void
N_Instruction_Viewer::disableUnsafeButtonsSlot(void){
  #if LOCAL_DEBUG
  cout << "<N_Instruction_Viewer::disableUnsafeButtonsSlot>.\n";
  #endif

  m_instruction_view_widget->buttonsSafe(false);
}

void
N_Instruction_Viewer::enableUnsafeButtonsSlot(void){
  #if LOCAL_DEBUG
  cout << "<N_Instruction_Viewer::enableUnsafeButtonsSlot>.\n";
  #endif

  m_instruction_view_widget->buttonsSafe(true);
}


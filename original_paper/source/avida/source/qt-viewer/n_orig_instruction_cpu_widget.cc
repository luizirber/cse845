
//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qwidgetstack.h>

#include "messaging_population_wrapper.hh"

#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif

#ifndef N_ORIG_INSTRUCTION_CPU_WIDGET_HH
#include "n_orig_instruction_cpu_widget.hh"
#endif

#ifndef POPULATION_CELL_WRAPPED_ACCESSORS_H
#include "population_cell_wrapped_accessors.hh"
#endif

#include "genotype.hh"
#include "hardware_cpu.hh"
#include "organism.hh"
#include "phenotype.hh"
#include "population.hh"
#include "population_cell.hh"


cLabeledField::cLabeledField(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  Message("<cLabeledField> constructor.");

  m_hboxlayout = new QHBoxLayout(this);

  m_title_label = new QLabel(this);
  m_hboxlayout->addWidget(m_title_label);

  m_data1_label = new QLabel(this);
  m_data1_label->setAlignment(
    ( m_data1_label->alignment()
      &
      !(Qt::AlignRight)
    )
    |
    Qt::AlignRight
  );
  m_data1_label->setSizePolicy(
    QSizePolicy(
      QSizePolicy::MinimumExpanding,
      QSizePolicy::Fixed
    )
  );
  m_hboxlayout->addWidget(m_data1_label);

  m_data2_label = new QLabel(this);
  m_data2_label->setAlignment(
    ( m_data2_label->alignment()
      &
      !(Qt::AlignRight)
    )
    |
    Qt::AlignRight
  );
  m_data2_label->setSizePolicy(
    QSizePolicy(
      QSizePolicy::MinimumExpanding,
      QSizePolicy::Fixed
    )
  );
  m_hboxlayout->addWidget(m_data2_label);
}

void
cLabeledField::setTitle(const QString &title){
  m_title_label->setText(title);
}

void
cLabeledField::setData1Text(const QString &text){
  m_data1_label->setText(text);
}

void
cLabeledField::setData2Text(const QString &text){
  m_data2_label->setText(text);
}

void
cLabeledField::setData2Binary(unsigned int value){
  m_data2_label->setText(
    QString("[%1]").arg(value, 0, 2)
  );
}

void
cLabeledField::setData2BinaryWidth(void){
  int zwidth = QApplication::fontMetrics().width("0");
  m_data2_label->setMinimumWidth(34 * zwidth);
}

void
cLabeledField::setData1DecimalWidth(void){
  int zwidth = QApplication::fontMetrics().width("0");
  m_data1_label->setMinimumWidth(10 * zwidth);
}

void
cLabeledField::showData2Text(bool yes){
  if(yes) m_data2_label->show();
  else m_data2_label->hide();
}


cSummaryWidget::cSummaryWidget(
  QWidget *parent,
  const char *name
)
: QGroupBox(parent, name)
{
  Message("<cSummaryWidget>");

  setTitle("Summary");
  setColumns(1);

  m_location = new cLabeledField(this);
  m_genotype_id = new cLabeledField(this);
  m_genotype_name = new cLabeledField(this);
  m_faults = new cLabeledField(this);
  m_offspring = new cLabeledField(this);
  m_thread = new cLabeledField(this);

  m_location->setTitle("Location:");
  m_genotype_id->setTitle("Genotype ID:");
  m_genotype_name->setTitle("Genotype Name:");
  m_faults->setTitle("Faults:");
  m_offspring->setTitle("Offspring:");
  m_thread->setTitle("Thread:");

  m_location->showData2Text(false);
  m_genotype_id->showData2Text(false);
  m_genotype_name->showData2Text(false);
  m_faults->showData2Text(false);
  m_offspring->showData2Text(false);
  m_thread->showData2Text(false);
}

void
cSummaryWidget::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  Message("<cSummaryWidget::setPopulationWrapper>");
  m_pop_wrap = pop_wrap;

  connect(
    pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(updateState())
  );
}

void
cSummaryWidget::setPopulationCell(int cell_id){
  Message("<cSummaryWidget::setPopulationCell>");
  m_cell_id = cell_id;
  updateState();
}

void
cSummaryWidget::updateState(void){
  Message("<cSummaryWidget::updateState>");

  cPopulation *population = m_pop_wrap->getPopulation();

  cPopulationCell *population_cell =
     GetPopulationCell(
       m_pop_wrap,
       m_cell_id
     );
  
  cGenotype *genotype =
    GetGenotype(
      GetOrganism(
        GetPopulationCell(
          m_pop_wrap,
          m_cell_id
        )
      )
    );

  cPhenotype *phenotype =
    GetPhenotype(
      GetOrganism(
        GetPopulationCell(
          m_pop_wrap,
          m_cell_id
        )
      )
    );
  
  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(
          m_pop_wrap,
          m_cell_id
        )
      )
    );

  // Location:
  if(0 == population || 0 == population_cell){
    m_location->setData1Text("");
  } else {
    m_location->setData1Text(
      QString("[%1, %2]").arg(
        population_cell->GetID() % population->GetWorldX()
      ).arg(
        population_cell->GetID() / population->GetWorldY()
      )
    );
  }

  // Genotype ID:
  // Genotype Name:
  if(0 == genotype){
    m_genotype_id->setData1Text("");
    m_genotype_name->setData1Text("");
  } else {
    m_genotype_id->setData1Text(
      QString("%1").arg(genotype->GetID())
    );
    m_genotype_name->setData1Text(
      QString(genotype->GetName())
    );
  }

  // Faults:
  // Offspring:
  if(0 == phenotype){
    m_faults->setData1Text("");
    m_offspring->setData1Text("");
  } else {
    m_faults->setData1Text(
      QString("%1").arg(phenotype->GetCurNumErrors())
    );
    m_offspring->setData1Text(
      QString("%1").arg(phenotype->GetNumDivides())
    );
  }

  // Thread:
  if(0 == hardware){
    m_thread->setData1Text("");
  } else {
    m_thread->setData1Text(
      QString("%1/%2").arg(
        hardware->GetCurThread() + 1
      ).arg(
        hardware->GetNumThreads()
      )
    );
  }
}



cRegistersWidget::cRegistersWidget(
  QWidget *parent,
  const char *name
)
: QGroupBox(parent, name)
{
  Message("<cRegistersWidget> constructor.");

  setTitle("Registers");
  setColumns(1);

  m_registers.setAutoDelete(TRUE);
  for(int i = 0; i < NUM_REGISTERS; i++){
    cLabeledField *labeled_field = new cLabeledField(this);
    char title = 'A' + i;
    labeled_field->setTitle(QString("%1:").arg(title));
    labeled_field->setData1DecimalWidth();
    labeled_field->setData2BinaryWidth();
    m_registers.append(labeled_field);
  }
}

void
cRegistersWidget::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  Message("<cRegistersWidget::setPopulationWrapper>");
  m_pop_wrap = pop_wrap;

  connect(
    pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(updateState())
  );
}

void
cRegistersWidget::setPopulationCell(int cell_id){
  Message("<cRegistersWidget::setPopulationCell>");
  m_cell_id = cell_id;
  updateState();
}

void
cRegistersWidget::updateState(void){
  Message("<cRegistersWidget::updateState>");

  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(
          m_pop_wrap,
          m_cell_id
        )
      )
    );

  if(0 == hardware){
    int i = 0;
    cLabeledField *labeled_field = m_registers.first();
    for(
      ;
      i < NUM_REGISTERS && 0 != labeled_field;
      i++, labeled_field = m_registers.next()
    ){
      labeled_field->setData1Text("");
      labeled_field->setData2Text("");
    }
  } else {
    int i = 0;
    cLabeledField *labeled_field = m_registers.first();
    for(
      ;
      i < NUM_REGISTERS && 0 != labeled_field;
      i++, labeled_field = m_registers.next()
    ){
      labeled_field->setData1Text(QString("%1").arg(hardware->Register(i)));
      labeled_field->setData2Binary(hardware->Register(i));
    }
  }
}

void
cRegistersWidget::showData2Text(bool should_show){
  int i = 0;
  cLabeledField *labeled_field = m_registers.first();
  for(
    ;
    i < NUM_REGISTERS && 0 != labeled_field;
    i++, labeled_field = m_registers.next()
  ){
    labeled_field->showData2Text(should_show);
  }
}

cInputsWidget::cInputsWidget(
  QWidget *parent,
  const char *name
)
: QGroupBox(parent, name)
{
  Message("<cInputsWidget> constructor.");

  setTitle("Inputs");
  setColumns(1);

  m_inputs.setAutoDelete(TRUE);
  for(int i = 0; i < NUM_REGISTERS; i++){
    cLabeledField *labeled_field = new cLabeledField(this);
    labeled_field->setTitle(QString("%1:").arg(i + 1));
    labeled_field->setData1DecimalWidth();
    labeled_field->setData2BinaryWidth();
    m_inputs.append(labeled_field);
  }
}

void
cInputsWidget::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  Message("<cInputsWidget::setPopulationWrapper>");
  m_pop_wrap = pop_wrap;

  connect(
    pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(updateState())
  );
}

void
cInputsWidget::setPopulationCell(int cell_id){
  Message("<cInputsWidget::setPopulationCell>");
  m_cell_id = cell_id;
  updateState();
}

void
cInputsWidget::updateState(void){
  Message("<cInputsWidget::updateState>");

  cPopulationCell *pop_cell =
    GetPopulationCell(
      m_pop_wrap,
      m_cell_id
    );

  if(0 == pop_cell){
    int i = 0;
    cLabeledField *labeled_field = m_inputs.first();
    for(
      ;
      i < IO_SIZE && 0 != labeled_field;
      i++, labeled_field = m_inputs.next()
    ){
      labeled_field->setData1Text("");
      labeled_field->setData2Text("");
    }
  } else {
    int i = 0;
    cLabeledField *labeled_field = m_inputs.first();
    for(
      ;
      i < IO_SIZE && 0 != labeled_field;
      i++, labeled_field = m_inputs.next()
    ){
      labeled_field->setData1Text(QString("%1").arg(pop_cell->GetInput(i)));
      labeled_field->setData2Binary(pop_cell->GetInput(i));
    }
  }
}

void
cInputsWidget::showData2Text(bool should_show){
  int i = 0;
  cLabeledField *labeled_field = m_inputs.first();
  for(
    ;
    i < IO_SIZE && 0 != labeled_field;
    i++, labeled_field = m_inputs.next()
  ){
    labeled_field->showData2Text(should_show);
  }
}

cStackField::cStackField(
  QWidget *parent,
  const char *name,
  WFlags f
)
: cLabeledField(parent, name, f)
{
  Message("<cStackField>");


  m_popup_pbutton = new QPushButton("...", this);
  connect(
    m_popup_pbutton, SIGNAL(pressed()),
    this, SLOT(displayPopupSlot())
  );
  m_hboxlayout->addWidget(m_popup_pbutton);

  m_stack_popup = new cStackPopup(this);
}

void
cStackField::useFrame(bool use_frame){
  Message("<cStackField::useFrame>");

  if( use_frame
  ){
    if (m_data1_label->frameStyle() != (QFrame::Box | QFrame::Plain))
      m_data1_label->setFrameStyle(QFrame::Box | QFrame::Plain);
  } else if (m_data1_label->frameStyle() != QFrame::NoFrame)
    m_data1_label->setFrameStyle(QFrame::NoFrame);
}

void
cStackField::displayPopupSlot(void){
  Message("<cStackField::displayPopupSlot>");

  if(m_stack_popup->isVisible()){
    m_stack_popup->hide();
  } else {
    QDesktopWidget *d = QApplication::desktop();

    int w=d->width();
    int h=d->height();
    
    QPoint popup_pt(
      mapToGlobal(
        QPoint(
          m_data1_label->x(),
          m_data1_label->y()
        )
      )
    );

    if(popup_pt.x() + m_stack_popup->width() >= w){
      popup_pt.setX(w - (m_stack_popup->width() + 5));
    }
    if(popup_pt.y() + m_stack_popup->height() >= h){
      popup_pt.setY(h - (m_stack_popup->height() + 5));
    }

    m_stack_popup->move(popup_pt);
    m_stack_popup->show();
    m_stack_popup->updateState();
  }
  m_popup_pbutton->setDown(false);
}

void
cStackField::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  Message("<cStackField::setPopulationWrapper>");
  m_pop_wrap = pop_wrap;
  m_stack_popup->setPopulationWrapper(pop_wrap);

  connect(
    pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(updateState())
  );
}

void
cStackField::setPopulationCell(int cell_id){
  Message("<cStackField::setPopulationCell>");
  m_cell_id = cell_id;
  m_stack_popup->setPopulationCell(cell_id);

  updateState();
}

void
cStackField::setStackNumber(int stack_number){
  Message("<cStackField::setStackNumber>");
  m_stack_no = stack_number;
  m_stack_popup->setStackNumber(stack_number);

  updateState();
}

void
cStackField::updateState(void){
  Message("<cStackField::updateState>");

  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(
          m_pop_wrap,
          m_cell_id
        )
      )
    );

  if(0 == hardware){
    Message(" --- 0 == hardware");
    setData1Text("");
    setData2Text("");
    useFrame(false);
  } else {
    setData1Text(QString("%1").arg(hardware->GetStack(0, m_stack_no)));
    setData2Binary(hardware->GetStack(0, m_stack_no));

    Message(" --- m_stack_no %d", m_stack_no);
    Message(" --- m_cell_id %d", m_cell_id);
    Message(" --- stack top %d", hardware->GetStack(0, m_stack_no));

    if (hardware->GetActiveStackID() == m_stack_no){
      Message(" --- use frame");
      useFrame(true);
    } else {
      Message(" --- don't use frame");
      useFrame(false);
    }
  }
}

void
cStackField::showData2Text(bool should_show){
  m_stack_popup->showData2Text(should_show);
  cLabeledField::showData2Text(should_show);  
}

cStackPopup::cStackPopup(
  QWidget *parent,
  const char *name
)
: QWidget(parent, name, WType_Popup)
{
  Message("<cStackPopup> constructor.");

  m_vboxlayout = new QVBoxLayout(this);

  m_scrollview = new QScrollView(this);
  m_vboxlayout->addWidget(m_scrollview);

  m_layout_widget = new QWidget(m_scrollview->viewport());
  m_scrollview->addChild(m_layout_widget);

  m_sv_layout = new QVBoxLayout(m_layout_widget);

  //m_sv_layout->addWidget(new QPushButton("Hello, world", m_layout_widget));

  m_stacklines.setAutoDelete(TRUE);
  for(int i = 0; i < STACK_SIZE; i++){
    cLabeledField *labeled_field = new cLabeledField(m_layout_widget);
    m_sv_layout->addWidget(labeled_field);
    labeled_field->setTitle(QString("%1:").arg(i + 1));
    labeled_field->setData1DecimalWidth();
    labeled_field->setData2BinaryWidth();
    m_stacklines.append(labeled_field);
  }
}

void
cStackPopup::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  Message("<cStackPopup::setPopulationWrapper>");
  m_pop_wrap = pop_wrap;

  connect(
    pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(updateState())
  );
}

void
cStackPopup::setPopulationCell(int cell_id){
  Message("<cStackPopup::setPopulationCell>");
  m_cell_id = cell_id;
  updateState();
}

void
cStackPopup::setStackNumber(int stack_number){
  Message("<cStackPopup::setStackNumber>");
  m_stack_no = stack_number;
  updateState();
}

void
cStackPopup::updateState(void){
  Message("<cStackPopup::updateState>");

  if(!isVisible()) return;

  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(
          m_pop_wrap,
          m_cell_id
        )
      )
    );

  if(0 == hardware){
    int i = 0;
    cLabeledField *labeled_field = m_stacklines.first();
    for(
      ;
      i < STACK_SIZE && 0 != labeled_field;
      i++, labeled_field = m_stacklines.next()
    ){
      labeled_field->setData1Text("");
      labeled_field->setData2Text("");
    }
  } else {
    Message(" --- valid data it seems.");
    int i = 0;
    cLabeledField *labeled_field = m_stacklines.first();
    for(
      ;
      i < STACK_SIZE && 0 != labeled_field;
      i++, labeled_field = m_stacklines.next()
    ){
      labeled_field->setData1Text(
        QString("%1").arg(
          hardware->GetStack(i, m_stack_no)
        )
      );
      labeled_field->setData2Binary(hardware->GetStack(i, m_stack_no));
    }
  }
}

void
cStackPopup::showData2Text(bool should_show){
  int i = 0;
  cLabeledField *labeled_field = m_stacklines.first();
  for(
    ;
    i < STACK_SIZE && 0 != labeled_field;
    i++, labeled_field = m_stacklines.next()
  ){
    labeled_field->showData2Text(should_show);
  }
}


cStacksWidget::cStacksWidget(
  QWidget *parent,
  const char *name
)
: QGroupBox(parent, name)
{
  Message("<cStacksWidget> constructor.");

  setTitle("Stack");
  setColumns(1);

  m_stackln1 = new cStackField(this);
  m_stackln2 = new cStackField(this);

  m_stackln1->setTitle("A:");
  m_stackln2->setTitle("B:");

  m_stackln1->setStackNumber(0);
  m_stackln2->setStackNumber(1);

  m_stackln1->setData1DecimalWidth();
  m_stackln1->setData2BinaryWidth();

  m_stackln2->setData1DecimalWidth();
  m_stackln2->setData2BinaryWidth();
}

void
cStacksWidget::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  Message("<cStacksWidget::setPopulationWrapper>");
  m_pop_wrap = pop_wrap;
  
  m_stackln1->setPopulationWrapper(pop_wrap);
  m_stackln2->setPopulationWrapper(pop_wrap);

  connect(
    pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(updateState())
  );
}

void
cStacksWidget::setPopulationCell(int cell_id){
  Message("<cStacksWidget::setPopulationCell>");
  m_cell_id = cell_id;

  m_stackln1->setPopulationCell(cell_id);
  m_stackln2->setPopulationCell(cell_id);

  updateState();
}

void
cStacksWidget::updateState(void){
  Message("<cStacksWidget::updateState>");
}

void
cStacksWidget::showData2Text(bool should_show){
  m_stackln1->showData2Text(should_show);
  m_stackln2->showData2Text(should_show);
}


cHideShowBinary::cHideShowBinary(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  Message("<cHideShowBinary>");

  QHBoxLayout *layout = new QHBoxLayout(this);
  
  layout->addItem(new QSpacerItem(20, 20));

  m_widgetstack = new QWidgetStack(this); 
  layout->addWidget(m_widgetstack);

  layout->addItem(new QSpacerItem(20, 20));

  m_show_binary_pbutton = new QPushButton("Show Binary", m_widgetstack);
  m_hide_binary_pbutton = new QPushButton("Hide Binary", m_widgetstack);

  connect(
    m_show_binary_pbutton, SIGNAL(clicked()),
    this, SLOT(showBinary())
  );

  connect(
    m_hide_binary_pbutton, SIGNAL(clicked()),
    this, SLOT(hideBinary())
  );

  //m_widgetstack->raiseWidget(m_show_binary_pbutton);
}

void
cHideShowBinary::showBinary(void){
  Message("<cHideShowBinary::showBinary>");
  m_widgetstack->raiseWidget(m_hide_binary_pbutton);

  emit showBinarySig(true);
}

void
cHideShowBinary::hideBinary(void){
  Message("<cHideShowBinary::hideBinary>");
  m_widgetstack->raiseWidget(m_show_binary_pbutton);

  emit showBinarySig(false);
}

N_Instruction_CPUWidget::N_Instruction_CPUWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  Message("<N_Instruction_CPUWidget> constructor.");

  m_vboxlayout = new QVBoxLayout(this);

  m_summary = new cSummaryWidget(this);
  m_vboxlayout->addWidget(m_summary);

  m_registers = new cRegistersWidget(this);
  m_vboxlayout->addWidget(m_registers);

  m_inputs = new cInputsWidget(this);
  m_vboxlayout->addWidget(m_inputs);

  m_stacks = new cStacksWidget(this);
  m_vboxlayout->addWidget(m_stacks);

  m_hideshow_binary = new cHideShowBinary(this);
  m_vboxlayout->addWidget(m_hideshow_binary);

  connect(
    m_hideshow_binary, SIGNAL(showBinarySig(bool)),
    this, SLOT(showData2Text(bool))
  );

  m_hideshow_binary->hideBinary();
}

void
N_Instruction_CPUWidget::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  Message("<N_Instruction_CPUWidget::setPopulationWrapper>");
  m_pop_wrap = pop_wrap;

  m_summary->setPopulationWrapper(pop_wrap);
  m_registers->setPopulationWrapper(pop_wrap);
  m_stacks->setPopulationWrapper(pop_wrap);
  m_inputs->setPopulationWrapper(pop_wrap);

  connect(
    pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(updateState())
  );
  connect(
    pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(updateState())
  );
}

void
N_Instruction_CPUWidget::setPopulationCell(int cell_id){
  Message("<N_Instruction_CPUWidget::setPopulationCell>");
  m_cell_id = cell_id;

  m_summary->setPopulationCell(cell_id);
  m_registers->setPopulationCell(cell_id);
  m_stacks->setPopulationCell(cell_id);
  m_inputs->setPopulationCell(cell_id);
}

void
N_Instruction_CPUWidget::updateState(void){
  Message("<N_Instruction_CPUWidget::updateState>");
}

void
N_Instruction_CPUWidget::showData2Text(bool should_show){
  m_registers->showData2Text(should_show);
  m_stacks->showData2Text(should_show);
  m_inputs->showData2Text(should_show);
}

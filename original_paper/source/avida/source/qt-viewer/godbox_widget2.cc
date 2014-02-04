//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

// including <float.h> to get DBL_DIG
// -- Number of decimal digits of precision in a double
#include <float.h>

#include <qcombobox.h>
#include <qdir.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qvalidator.h>
#include <qwidget.h>

#include "config.hh"
#include "environment.hh"
#include "event_list.hh"
#include "genome.hh"
#include "inst_util.hh"
#include "population.hh"
#include "population_cell.hh"
#include "string.hh"

#include "apocalypse_widget.hh"
#include "avd_rate_slider2.hh"
#include "avd_rate_validator2.hh"
#include "avd_lograte_widget.hh"
#include "messaging_population_wrapper.hh"
#include "repopulate_widget.hh"

#include "godbox_widget2.hh"

/*
static class variables
*/
QString GodBoxWidget2::s_preset_organisms_dir;


/*
public member functions
*/

GodBoxWidget2::GodBoxWidget2(
  QWidget *parent,
  const char *name,
  WFlags f
)
//: GodBox2(parent, name, f)
: QWidget(parent, name, f)
{
  cout << "<GodBoxWidget2> constructor.\n";

  m_pop_wrap = 0;

  setCaption(trUtf8("Play God"));
  setupLayout();
  minimizeLayout();
  setupRates();
}

GodBoxWidget2::~GodBoxWidget2(void)
{
  cout << "<~GodBoxWidget2> destructor.\n";

}

void
GodBoxWidget2::GodBoxWidget2_dynamicConfig(void)
{
  cout << "<GodBoxWidget2::GodBoxWidget2_dynamicConfig> entered.\n";

  /*
  FIXME:  preset_organisms shouldn't be hard-wired!  -- kaben.
  */

  s_preset_organisms_dir =
    QString(cConfig::GetDefaultDir()) + "preset_organisms/";

  cout << " --- s_preset_organisms_dir " << s_preset_organisms_dir << endl;
}

void
GodBoxWidget2::setPopulationWrapper(MessagingPopulationWrapper *pop_wrap)
{
  cout << "<GodBoxWidget2::setPopulationWrapper> entered.\n";

  m_pop_wrap = pop_wrap;
}

void
GodBoxWidget2::setup(void){
  cout << "<GodBoxWidget2::setup> entered.\n";

  if(m_pop_wrap){
    setupConnects();
    loadMutRates();
    loadCreatureList();
  }
}

void
GodBoxWidget2::setupLayout(void)
{
  cout << "<GodBoxWidget2::setupLayout> entered.\n";

  m_vbox_layout = new QVBoxLayout(
    this,
    11,
    6,
    "<GodBoxWidget2::m_vbox_layout>"
  );

  m_hbox_controls_layout = new QHBoxLayout(0);

  m_mutation_rates_groupbox = new QGroupBox(
    1,
    Qt::Vertical, 
    this
  );
  m_mutation_rates_groupbox->setTitle(trUtf8("Mutation Rates"));
  m_hbox_controls_layout->addWidget(m_mutation_rates_groupbox);

  m_pt_mutation_widget = new avd_LogRateWidget(m_mutation_rates_groupbox);
  m_pt_mutation_widget->setLabel(trUtf8("Cosmic Rays:"));

  m_ins_del_mutation_widget = new avd_LogRateWidget(m_mutation_rates_groupbox);
  m_ins_del_mutation_widget->setLabel(trUtf8("Insert/Delete:"));

  m_cp_mutation_widget = new avd_LogRateWidget(m_mutation_rates_groupbox);
  m_cp_mutation_widget->setLabel(trUtf8("Copy:"));

  m_apocalypse_groupbox = new QGroupBox(
    1,
    Qt::Vertical, 
    this
  );
  m_apocalypse_groupbox->setTitle(trUtf8("Apocalypse"));
  m_hbox_controls_layout->addWidget(m_apocalypse_groupbox);

  m_apocalypse_widget = new ApocalypseWidget(m_apocalypse_groupbox);
  m_apocalypse_widget->setLabel(trUtf8("Death Rate:"));
  m_apocalypse_widget->setButtonText(trUtf8("Kill Creatures"));

  m_repopulate_groupbox = new QGroupBox(
    1,
    Qt::Vertical, 
    this
  );
  m_repopulate_groupbox->setTitle(trUtf8("Repopulate"));
  m_hbox_controls_layout->addWidget(m_repopulate_groupbox);

  m_repopulate_widget = new RepopulateWidget(m_repopulate_groupbox);
  m_repopulate_widget->setLabel(trUtf8("New Creature Type:"));
  m_repopulate_widget->setInjectButtonText(trUtf8("Inject One Creature"));
  m_repopulate_widget->setInjectAllButtonText(trUtf8("Replace Population"));


  m_hbox_buttons_layout = new QHBoxLayout(0);

  m_help_button = new QPushButton(this);
  m_help_button->setText(trUtf8("Help"));
  m_hbox_buttons_layout->addWidget(m_help_button);

  QSpacerItem *m_button_spacer = new QSpacerItem(
    20,
    20,
    QSizePolicy::Expanding,
    QSizePolicy::Minimum
  );
  m_hbox_buttons_layout->addItem(m_button_spacer);

  m_close_button = new QPushButton(this);
  m_close_button->setText(trUtf8("Close"));
  m_hbox_buttons_layout->addWidget(m_close_button);

  m_vbox_layout->addLayout(m_hbox_controls_layout);
  m_vbox_layout->addLayout(m_hbox_buttons_layout);
}

void
GodBoxWidget2::minimizeLayout(void)
{
  cout << "<GodBoxWidget2::minimizeLayout> entered.\n";
  
  int minimum_widget_width = 0;

  /*
  set widths of mutation subwidgets to max. of their min. widths.
  */
  m_pt_mutation_widget->adjustSize();
  m_ins_del_mutation_widget->adjustSize();
  m_cp_mutation_widget->adjustSize();

  if (minimum_widget_width < m_pt_mutation_widget->width())
    minimum_widget_width = m_pt_mutation_widget->width();

  if (minimum_widget_width < m_ins_del_mutation_widget->width())
    minimum_widget_width = m_ins_del_mutation_widget->width();

  if (minimum_widget_width < m_cp_mutation_widget->width())
    minimum_widget_width = m_cp_mutation_widget->width();

  m_pt_mutation_widget->setFixedWidth(minimum_widget_width);
  m_ins_del_mutation_widget->setFixedWidth(minimum_widget_width);
  m_cp_mutation_widget->setFixedWidth(minimum_widget_width);

  /*
  minimize the rest of the subwidgets.
  */
  m_apocalypse_widget->adjustSize();
  m_repopulate_widget->adjustSize();

  /*
  this sets a minimum size for whole widget.
  */
  m_repopulate_widget->setFixedHeight(200);

  /*
  minimize the rest of the container subwidgets.
  */
  m_mutation_rates_groupbox->adjustSize();
  m_apocalypse_groupbox->adjustSize();
  m_repopulate_groupbox->adjustSize();
  m_hbox_controls_layout->activate();

  //adjustSize();
  cout << " --- main width " << width() << "\n";

  //setFixedWidth(width());
  //setMinimumHeight(300);
}

/*
private member functions
*/

void
GodBoxWidget2::setupRates(void)
{
  cout << "<GodBoxWidget2::setupRates> entered.\n";

  m_rate_validator = new avd_RateValidator2(this);

  m_pt_mutation_widget->setRateValidator(m_rate_validator);
  m_ins_del_mutation_widget->setRateValidator(m_rate_validator);
  m_cp_mutation_widget->setRateValidator(m_rate_validator);
  m_apocalypse_widget->setRateValidator(m_rate_validator);

  m_pt_mutation_widget->setMinRate(0.00001);
  m_ins_del_mutation_widget->setMinRate(0.00001);
  m_cp_mutation_widget->setMinRate(0.00001);
  m_apocalypse_widget->setMaxRate(0.99);

  m_apocalypse_widget->setRate(0.99);

  m_pt_mutation_widget->setLineStep(0.005);
  m_ins_del_mutation_widget->setLineStep(0.005);
  m_cp_mutation_widget->setLineStep(0.005);
  //m_apocalypse_widget

  m_pt_mutation_widget->setPageStep(0.05);
  m_ins_del_mutation_widget->setPageStep(0.05);
  m_cp_mutation_widget->setPageStep(0.05);
  //m_apocalypse_widget
}

void
GodBoxWidget2::setupConnects(void)
{
  cout << "<GodBoxWidget2::setupConnects> entered.\n";

  connect(
    m_pt_mutation_widget, SIGNAL(rateChanged(double)),
    this, SLOT(ptMutRateWidgetChanged(double))
  );

  connect(
    m_ins_del_mutation_widget, SIGNAL(rateChanged(double)),
    this, SLOT(insDelMutRateWidgetChanged(double))
  );

  connect(
    m_cp_mutation_widget, SIGNAL(rateChanged(double)),
    this, SLOT(cpMutRateWidgetChanged(double))
  );

  connect(
    m_apocalypse_widget, SIGNAL(clicked(void)),
    this, SLOT(apocalypsePushButtonClicked(void))
  );

  connect(
    m_repopulate_widget, SIGNAL(injectOneButtonClicked(void)),
    this, SLOT(injectOneButtonClicked(void))
  );
  connect(
    m_repopulate_widget, SIGNAL(injectAllButtonClicked(void)),
    this, SLOT(injectAllButtonClicked(void))
  );

  connect(
    m_help_button, SIGNAL(clicked(void)),
    this, SLOT(helpButtonClicked(void))
  );
  connect(
    m_close_button, SIGNAL(clicked(void)),
    this, SLOT(closeButtonClicked(void))
  );

  connect(
    m_pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SLOT(stateChanged())
  );
  connect(
    m_pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SLOT(stateChanged())
  );

  cout << "<GodBoxWidget2::setupConnects> done.\n";
}

void
GodBoxWidget2::loadMutRates(void)
{
  if (!m_pt_mutation_widget->hasFocus()){
    m_pt_mutation_widget->setRate(cConfig::GetPointMutProb());
  }

  if (!m_ins_del_mutation_widget->hasFocus()){
    m_ins_del_mutation_widget->setRate(cConfig::GetInsMutProb());
  }

  if (!m_cp_mutation_widget->hasFocus()){
    m_cp_mutation_widget->setRate(cConfig::GetCopyMutProb());
  }
}

void
GodBoxWidget2::loadCreatureList(void)
{
  cout << "<GodBoxWidget2::loadCreatureList> entered.\n";

  m_repopulate_widget->clear();
  m_repopulate_widget->insertItem("-default-");

  QDir preset_organisms_dir_access(s_preset_organisms_dir); 
  preset_organisms_dir_access.setFilter(
      QDir::Files
    | QDir::Readable
  );
  preset_organisms_dir_access.setSorting(
      QDir::Name
  );

  cout << " --- path " << preset_organisms_dir_access.path() << endl;
  cout << " --- absPath " << preset_organisms_dir_access.absPath() << endl;
  cout
  << " --- canonicalPath "
  << preset_organisms_dir_access.canonicalPath() << endl;
  cout << " --- dirName " << preset_organisms_dir_access.dirName() << endl;

  cout << " --- available creatures: " << endl;

  QStringList preset_organism(
    preset_organisms_dir_access.entryList()
  );

  for(
    QStringList::Iterator it = preset_organism.begin();
    it != preset_organism.end();
    ++it
  ){
    cout << " --- " << *it << endl;
    m_repopulate_widget->insertItem(*it);
  }
}

/*
private slots
*/

void
GodBoxWidget2::ptMutRateWidgetChanged(double rate)
{
  cout << "<GodBoxWidget2::ptMutRateWidgetChanged> rate " << rate << ".\n";

  m_pop_wrap->lock();
  for (int i = 0; i < m_pop_wrap->getPopulation()->GetSize(); i++) {
    m_pop_wrap->
      getPopulation()->
        GetCell(i).MutationRates().SetPointMutProb(rate);
  }
  cConfig::SetPointMutProb(rate);
  m_pop_wrap->unlock();
}

void
GodBoxWidget2::insDelMutRateWidgetChanged(double rate)
{
  cout << "<GodBoxWidget2::insDelMutRateWidgetChanged> rate " << rate << ".\n";

  m_pop_wrap->lock();
  for (int i = 0; i < m_pop_wrap->getPopulation()->GetSize(); i++) {
    m_pop_wrap->
      getPopulation()->
        GetCell(i).MutationRates().SetInsMutProb(rate);
    m_pop_wrap->
      getPopulation()->
        GetCell(i).MutationRates().SetDelMutProb(rate);
  }
  cConfig::SetInsMutProb(rate);
  cConfig::SetDelMutProb(rate);
  m_pop_wrap->unlock();
}

void
GodBoxWidget2::cpMutRateWidgetChanged(double rate)
{
  cout << "<GodBoxWidget2::cpMutRateWidgetChanged> rate " << rate << ".\n";

  m_pop_wrap->lock();
  for (int i = 0; i < m_pop_wrap->getPopulation()->GetSize(); i++) {
    m_pop_wrap->
      getPopulation()->
        GetCell(i).MutationRates().SetCopyMutProb(rate);
  }
  cConfig::SetCopyMutProb(rate);
  m_pop_wrap->unlock();
}

void
GodBoxWidget2::apocalypsePushButtonClicked(void)
{
  cout << "<GodBoxWidget2::apocalypsePushButtonClicked> entered.\n";

  double kill_prob = m_apocalypse_widget->rate();
  cout << " --- kill_prob " << kill_prob << "\n";

  m_pop_wrap->lock();
  for (int i = 0; i < m_pop_wrap->getPopulation()->GetSize(); i++) {
    cPopulationCell & cell = m_pop_wrap->getPopulation()->GetCell(i);
    if (cell.IsOccupied() == false)
      continue;
    if (g_random.P(kill_prob))
      m_pop_wrap->getPopulation()->KillOrganism(cell);
  }
  m_pop_wrap->unlock();

  /* cheat -- force all widgets to update. */
  m_pop_wrap->avidaUpdatedSlot();
}

void
GodBoxWidget2::injectOneButtonClicked(void)
{
  cout << "<GodBoxWidget2::injectOneButtonClicked> entered.\n";

  cString fname;
  int cell_id = 0;
  double merit = -1;
  int lineage_label = 0;
  double neutral_metric = 0;


  /*
  FIXME:
  code duplication between
  injectOneButtonClicked() and injectAllButtonClicked().

  -- kaben.
  */
  if(m_repopulate_widget->currentText() == "-default-"){
    fname = cConfig::GetStartCreature();
  }else{
    fname = (
      s_preset_organisms_dir + m_repopulate_widget->currentText()
    ).latin1();
  }

  cGenome genome = cInstUtil::LoadGenome(
    fname,
    m_pop_wrap->getPopulation()->GetEnvironment().GetInstLib()
  );

  if(genome.GetSize() == 1){
    /*
    FIXME:
    need error handling here.

    -- kaben.
    */
    cout << " --- bogus genome." << endl;
  } else {
    cout << " --- fname: " << fname << endl;
    
    m_pop_wrap->lock();
    m_pop_wrap->getPopulation()->Inject(
      genome,
      cell_id,
      merit,
      lineage_label,
      neutral_metric
    );
    m_pop_wrap->unlock();

    /* cheat -- force all widgets to update. */
    m_pop_wrap->avidaUpdatedSlot();
  }
}

void
GodBoxWidget2::injectAllButtonClicked(void)
{
  cout << "<GodBoxWidget2::injectAllButtonClicked> entered.\n";

  cString fname;
  int cell_id = 0;
  double merit = -1;
  int lineage_label = 0;
  double neutral_metric = 0;

  /*
  FIXME:
  code duplication between
  injectOneButtonClicked() and injectAllButtonClicked().

  -- kaben.
  */
  if(m_repopulate_widget->currentText() == "-default-"){
    fname = cConfig::GetStartCreature();
  }else{
    fname = (
      s_preset_organisms_dir + m_repopulate_widget->currentText()
    ).latin1();
  }

  cGenome genome = cInstUtil::LoadGenome(
    fname,
    m_pop_wrap->getPopulation()->GetEnvironment().GetInstLib()
  );

  if(genome.GetSize() == 1){
    /*
    FIXME:
    need error handling here.

    -- kaben.
    */
    cout << " --- bogus genome." << endl;
  } else {
    cout << " --- fname: " << fname << endl;
    
    m_pop_wrap->lock();
    for (int i = 0; i < m_pop_wrap->getPopulation()->GetSize(); i++) {
      m_pop_wrap->getPopulation()->Inject(
        genome,
        i,
        merit,
        lineage_label,
        neutral_metric
      );
    }
    m_pop_wrap->getPopulation()->SetSyncEvents(true);
    m_pop_wrap->unlock();

    /* cheat -- force all widgets to update. */
    m_pop_wrap->avidaUpdatedSlot();
  }
}

void
GodBoxWidget2::helpButtonClicked(void)
{
  cout << "<GodBoxWidget2::helpButtonClicked> entered.\n";

}
void
GodBoxWidget2::closeButtonClicked(void)
{
  cout << "<GodBoxWidget2::closeButtonClicked> entered.\n";

  close();
}

void
GodBoxWidget2::stateChanged(void)
{
  cout << "<GodBoxWidget2::stateChanged> entered.\n";

  loadMutRates();
}

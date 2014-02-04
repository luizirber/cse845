#include "main_window.hh"

#include "../defs.hh"
#include "../main/config.hh"

#include "init_file_view.hh"
#include "map_view.hh"
#include "creature_view.hh"
#include "cell_view_widget.hh"
#include "n_orig_instruction_viewer.hh"
#include "event_view_widget.hh"
//#include "godbox_widget.hh"
#include "godbox_widget2.hh"
#include "avida_plot_widget.hh"
#include "plot_select_dia_impl.hh"

#include "messaging_population_wrapper.hh"

#include <iostream>
#include <stdlib.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qapplication.h>


MainWindow::MainWindow(
  MessagingPopulationWrapper *pop_wrap,
  QWidget *parent,
  int argc,
  const char * const argv[]
)
  : QMainWindow( parent ),
    m_argc( argc ), m_argv( argv ),
    m_pop_wrap( pop_wrap )
{
  // connect to driver via m_pop_wrap
  connect(
    this, SIGNAL(startAvidaSig()),
    m_pop_wrap, SLOT(startAvidaSlot())
  );
  connect(
    this, SIGNAL(stopAvidaSig()),
    m_pop_wrap, SLOT(stopAvidaSlot())
  );
  connect(
    this, SIGNAL(resetAvidaSig()),
    m_pop_wrap, SLOT(resetAvidaSlot())
  );
  connect(
    this, SIGNAL(exitAvidaSig()),
    m_pop_wrap, SLOT(exitAvidaSlot())
  );

  connect(
    m_pop_wrap, SIGNAL(avidaUpdatedSig()),
    this, SIGNAL(stateChanged())
  );
  connect(
    m_pop_wrap, SIGNAL(avidaSteppedSig(int)),
    this, SIGNAL(stateChanged())
  );
  
  // and the viewer to watch it.
  setupViewer();
  // now create the menu bar.
  setupMenuBar();
  
  // set some initial size
  resize( 550, 400);

}


MainWindow::~MainWindow()
{
  cleanupViewer();
  // m_timer is automatically destroyed.
}


void
MainWindow::setupViewer()
{
  /**
   ** XXX:  test code by kaben to test cell viewer.
   **       this gives Qt a path to pixmaps used in some buttons.  this
   **       kludge will change at some point.
   **/
  QMimeSourceFactory::defaultFactory()->addFilePath(".");
  /**/

  m_map_view = new MapView( m_pop_wrap, this );
  setCentralWidget( m_map_view );

  connect( this, SIGNAL( stateChanged() ),
	   m_map_view, SLOT( repaintMap() ) );
}

void
MainWindow::cleanupViewer()
{
  // delete all windows that have something to do with the Avida world.
  // everything else remain open.
  delete m_map_view;
  delete m_creature_view;
}

void
MainWindow::setupMenuBar()
{
  QPopupMenu* file = new QPopupMenu( this );
  file->insertItem("&Start", this, SLOT( startAvida() ) );
  file->insertItem("Sto&p", this, SLOT( stopAvida() ) );
  file->insertItem("&Reset", this, SLOT( resetAvida() ) );
  file->insertItem("E&xit", this, SLOT( exitProgram() ) );
  menuBar()->insertItem( "&File", file );

  QPopupMenu* view = new QPopupMenu( this );
  //view->insertItem("&Creature View", this, SLOT( creatureView() ) );
  /**
   ** XXX:  test code by kaben to test cell viewer.
   **/
  view->insertItem("Cell View", this, SLOT( cellView() ) );
  /**/
  view->insertItem("Instruction View", this, SLOT( N_Instruction_View() ) );
  view->insertItem("Event View", this, SLOT( eventView() ) );
  //view->insertItem("God Box", this, SLOT( godBox() ) );
  view->insertItem("God Box 2", this, SLOT( godBox2() ) );
  view->insertItem("View &Genesis File", this, SLOT( genesisView() ) );
  view->insertItem("View &Event List", this, SLOT( eventListView() ) );
  view->insertItem("View E&nvironment", this, SLOT( environmentView() ) );
  view->insertItem("View &Instruction Set", this, SLOT( instSetView() ) );
  menuBar()->insertItem( "&View", view );

  QPopupMenu* plot = new QPopupMenu( this );
  plot->insertItem("&Plot Graph", this, SLOT( createPlotView() ) );
  menuBar()->insertItem( "&Plot", plot );

}


void
MainWindow::N_Instruction_View(void){
  if ( !m_n_orig_instruction_viewer ){ // create the view if it doesn't exist
    int cell = 0;
    if ( m_map_view ) // do we have a map view already?
      cell = m_map_view->GetSelectedCell();
    m_n_orig_instruction_viewer = new N_Instruction_Viewer(
      0,
      0,
      /*
      XXX: destr-close of N_Instruction_Viewer fails without this.
      I'm default-setting the flag in the constructor declaration, but
      it doesn't seem to hold, don't know why...  -- K.
      */
      WDestructiveClose
    );
    m_n_orig_instruction_viewer->setPopulationWrapper(m_pop_wrap);
    m_n_orig_instruction_viewer->setPopulationCell(cell);

    connect(
      m_map_view,
      SIGNAL(cellSelected(int)),
	    m_n_orig_instruction_viewer,
      SLOT(setPopulationCell(int))
    );
  }
  m_n_orig_instruction_viewer->show();
  m_n_orig_instruction_viewer->raise();
}


void
MainWindow::eventView(void){
  if ( !m_event_view_widget ){ // create the view if it doesn't exist
    int cell = 0;
    if ( m_map_view ) // do we have a map view already?
    m_event_view_widget = new EventViewWidget(
      0,
      0,
      WDestructiveClose
    );
    m_event_view_widget->setPopulationWrapper(m_pop_wrap);
    //m_event_view_widget->setPopulationWrapper(m_pop_wrap);
  }
  m_event_view_widget->show();
  m_event_view_widget->raise();
}


/*
void
MainWindow::godBox(void){
  if ( !m_god_box_widget ){ // create the view if it doesn't exist
    m_god_box_widget = new GodBoxWidget(
      0,
      0
    );
    //m_event_view_widget->setPopulationWrapper(m_pop_wrap);
  }
  m_god_box_widget->setPopulationWrapper(m_pop_wrap);
  m_god_box_widget->setup();
  m_god_box_widget->show();
  m_god_box_widget->raise();
}
*/


void
MainWindow::godBox2(void){
  if ( !m_god_box_widget2 ){ // create the view if it doesn't exist
    m_god_box_widget2 = new GodBoxWidget2(
      0,
      0
    );
    //m_event_view_widget->setPopulationWrapper(m_pop_wrap);
    m_god_box_widget2->setPopulationWrapper(m_pop_wrap);
    m_god_box_widget2->setup();
  }
  m_god_box_widget2->show();
  m_god_box_widget2->raise();
}


//void
//MainWindow::creatureView()
//{
//  if ( !m_creature_view ){ // create the view if it doesn't exist
//    int cell = 0;
//    if ( m_map_view ) // do we have a map view already?
//      cell = m_map_view->GetSelectedCell();
//    m_creature_view = new CreatureView(
//      0, m_pop_wrap, cell );
//    connect( m_map_view, SIGNAL( cellSelected( int ) ),
//	     m_creature_view, SLOT( setCell( int ) ) );
//    connect( this, SIGNAL( stateChanged() ),
//	     m_creature_view, SLOT( drawView() ) );
//  }
//  m_creature_view->show();
//  m_creature_view->raise();
//}


void
MainWindow::cellView()
{
  if ( !m_cell_view_widget ){ // create the view if it doesn't exist
    int cell = 0;
    if ( m_map_view ) // do we have a map view already?
      cell = m_map_view->GetSelectedCell();
    m_cell_view_widget = new CellViewWidget();
    m_cell_view_widget->setPopulationWrapper(m_pop_wrap);
    m_cell_view_widget->setPopulationCell(cell);

    connect(
      m_map_view,
      SIGNAL(cellSelected(int)),
	    m_cell_view_widget,
      SLOT(setPopulationCell(int))
    );
    connect(
      this,
      SIGNAL(stateChanged()),
	    m_cell_view_widget,
      SLOT(updateState())
    );
    connect(
      m_pop_wrap, SIGNAL(avidaBreakpointSig(int)),
      m_cell_view_widget, SLOT(setPopulationCell(int))
    );
  }
  m_cell_view_widget->show();
  m_cell_view_widget->raise();
}

void
MainWindow::createPlotView()
{
  PlotSelectDiaImpl dia;

  if ( dia.exec() ){
    // The Avida Plot widgets are created with the flag Qt::WDestructiveClose,
    // hence we don't have to worry about memory leaks here.
    AvidaPlotWidget *w = new AvidaPlotWidget( m_pop_wrap, dia.result() );
    connect( this, SIGNAL( stateChanged() ), w, SLOT( updateData() ) );
    connect( this, SIGNAL( avidaDestroyed() ), w , SLOT( avidaDestroyed() ) );
    w->show();
  }
}


void
MainWindow::genesisView()
{
  if ( !m_genesis_view ){ // create the view if it doesn't exist
    m_genesis_view = new InitFileView(
      QString( cConfig::GetGenesisFilename() () ), 0 );
  }
  else
    m_genesis_view->setFile( ( cConfig::GetGenesisFilename() () ) );
  m_genesis_view->show();
  m_genesis_view->raise();
}

void
MainWindow::eventListView()
{
  if ( !m_event_file_view ){ // create the view if it doesn't exist
    m_event_file_view = new InitFileView(
      QString( cConfig::GetEventFilename() () ), 0 );
  }
  else
    m_event_file_view->setFile( ( cConfig::GetEventFilename() () ) );
  m_event_file_view->show();
  m_event_file_view->raise();
}

void
MainWindow::environmentView()
{
  if ( !m_environment_view ){ // create the view if it doesn't exist
    m_environment_view = new InitFileView(
      QString( cConfig::GetEnvironmentFilename() () ), 0 );
  }
  else
    m_environment_view->setFile( ( cConfig::GetEnvironmentFilename() () ) );
  m_environment_view->show();
  m_environment_view->raise();
}


void
MainWindow::instSetView()
{
  if ( !m_inst_set_view ){ // create the view if it doesn't exist
    m_inst_set_view = new InitFileView(
      QString( cConfig::GetInstFilename() () ), 0 );
  }
  else
    m_inst_set_view->setFile( ( cConfig::GetInstFilename() () ) );
  m_inst_set_view->show();
  m_inst_set_view->raise();
}


void
MainWindow::startAvida()
{
  emit startAvidaSig();
}

void
MainWindow::stopAvida()
{
  emit stopAvidaSig();
}

void
MainWindow::resetAvida()
{
  // stop the current run
  stopAvida();
  // delete everything...
  cleanupViewer();

  // here, Avida should be reseted. That's not yet implemented...
  emit resetAvidaSig();

  // ... and set up anew
  setupViewer();
}

void
MainWindow::exitProgram()
{
  emit exitAvidaSig();
}

//#include "main_window.moc"



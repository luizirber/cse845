#include "creature_view.hh"

#include "../main/population.hh"
#include "../main/population_cell.hh"
#include "../main/organism.hh"
#include "../main/genotype.hh"
#include "../main/phenotype.hh"

#include "messaging_population_wrapper.hh"

#include <iostream>

#include <qlabel.h>
#include <qstring.h>
#include <qlayout.h>

CreatureView::CreatureView(
  QWidget *parent,
  MessagingPopulationWrapper *pop_wrap,
  int cell
) :
  QWidget( parent ), m_pop_wrap( pop_wrap )
{


  // set the window caption
  setCaption( "Avida - Creature View" );

  // setup all the rest
  setup();

  // set the starting cell
  setCell( cell );
}
			
			
CreatureView::~CreatureView()
{
}


void
CreatureView::setup()
{
  QGridLayout *layout = new QGridLayout( this );

  // cell position
  layout->addWidget( new QLabel( "Cell: ", this ), 0, 0 );
  m_cell_l = new QLabel( this );
  m_cell_l->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  layout->addWidget( m_cell_l, 0, 1 );
  // merit
  layout->addWidget( new QLabel( "Merit: ", this ), 1, 0 );
  m_merit_l = new QLabel( this );
  m_merit_l->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  layout->addWidget( m_merit_l, 1, 1 );
  // fitness
  layout->addWidget( new QLabel( "Fitness: ", this ), 2, 0 );
  m_fitness_l = new QLabel( this );
  m_fitness_l->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  layout->addWidget( m_fitness_l, 2, 1 );
  // gestation time
  QLabel *gest_t = new QLabel( "Gestation Time: ", this );
  // we set a max. size for this label
  gest_t->setMaximumWidth( gest_t->sizeHint().width() );
  layout->addWidget( gest_t, 3, 0 );
  m_gestation_time_l = new QLabel( this );
  m_gestation_time_l->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  layout->addWidget( m_gestation_time_l, 3, 1 );
  // length
  layout->addWidget( new QLabel( "Length: ", this ), 4, 0 );
  m_length_l = new QLabel( this );
  m_length_l->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  layout->addWidget( m_length_l, 4, 1 );
  // age
  layout->addWidget( new QLabel( "Age: ", this ), 5, 0 );
  m_age_l = new QLabel( this );
  m_age_l->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  layout->addWidget( m_age_l, 5, 1 );
  // genotype
  layout->addWidget( new QLabel( "Genotype: ", this ), 6, 0 );
  m_genotype_l = new QLabel( this );
  m_genotype_l->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  layout->addWidget( m_genotype_l, 6, 1 );
  // lineage
  layout->addWidget( new QLabel( "Lineage: ", this ), 7, 0 );
  m_lineage_l = new QLabel( this );
  m_lineage_l->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  layout->addWidget( m_lineage_l, 7, 1 );

  // set the minimum size of one of the labels:
  m_genotype_l->setMinimumWidth( 20*fontMetrics().width( "x" ) );

  layout->activate();

}



void
CreatureView::drawView()
{
  m_pop_wrap->lock();
  assert( m_cell == &(m_pop_wrap->getPopulation()->GetCell( m_cell_pos ) ));
  m_pop_wrap->lock();

  m_cell_l->setText( QString( "%1" ).arg( m_cell_pos ) );
  
  if ( m_cell->IsOccupied() ){
    cOrganism *organism = m_cell->GetOrganism();
  if ( organism != NULL ){ // is the cell occupied?
    m_merit_l->setText( QString( "%1" ).arg( organism->GetPhenotype().GetMerit().GetDouble() ) );
    m_fitness_l->setText( QString( "%1" ).arg( organism->GetPhenotype().GetFitness() ) );
    m_gestation_time_l->setText( QString( "%1" ).arg( organism->GetPhenotype().GetGestationTime() ) );
    m_age_l->setText( QString( "%1" ).arg(
					  organism->GetPhenotype().GetAge() ) );
    m_lineage_l->setText( QString( "%1" ).arg( organism->GetLineageLabel() ) );
    cGenotype *genotype = organism->GetGenotype();
    assert( genotype != NULL );

    m_genotype_l->setText( genotype->GetName()() );
    m_length_l->setText( QString( "%1" ).arg( organism->GetGenotype()->GetLength() ) );

    }
  }
  else{ // draw information for an empty cell
    m_merit_l->setText( "" );
    m_fitness_l->setText( "" );
    m_gestation_time_l->setText( "" );
    m_age_l->setText( "" );
    m_lineage_l->setText( "" );
    m_genotype_l->setText( "" );
    m_length_l->setText( "" );
  }
}

void
CreatureView::setCell( int cell )
{
  m_cell_pos = cell;
  m_pop_wrap->lock();
  m_cell = &( m_pop_wrap->getPopulation()->GetCell( cell ) );
  m_pop_wrap->unlock();
  assert( m_cell != 0 );
  drawView();
}


//#include "creature_view.moc"

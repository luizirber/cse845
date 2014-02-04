#include "avida_plot_widget.hh"

#include "../tools/file.hh"
#include "../main/population.hh"
#include "../main/stats.hh"

#include "messaging_population_wrapper.hh"


AvidaPlotWidget::AvidaPlotWidget(
  MessagingPopulationWrapper *pop_wrap,
  AvidaData::eTypes type,
  QWidget *parent,
  const char * name, WFlags f
)
: PlotWidget( parent, name, f ),
  m_pop_wrap( pop_wrap ),
  m_type( type ),
  m_entry( AvidaData::findEntry( type ) )
{
  setCaption( m_entry.getDescription() );
  
  setYLabel( m_entry.getDescription() );
  setXLabel( "Time [updates]" );
  
  // load in those data points that have already been created while
  // avida was running...
  load( m_entry.getDataFilename(), 1, m_entry.getColumn() );
}


AvidaPlotWidget::~AvidaPlotWidget()
{
}


void
AvidaPlotWidget::load( const char* filename, int colx, int coly )
{
  assert( colx > 0 );
  assert( coly > 0 );

  clear( false );

  cInitFile file( filename );
  file.Load();
  file.Compress();
  cString line;
  cString valx;
  cString valy;

  if (file.GetNumLines() > 0) {
    line = file.GetLine();
    file.RemoveLine();
    valx = line.GetWord( colx - 1 );
    valy = line.GetWord( coly - 1 );
    addPoint( DataPoint( valx.AsDouble(), valy.AsDouble() ), false );
  }

  while ( file.GetNumLines() > 0 ){
    line = file.GetLine();
    file.RemoveLine();
    valx = line.GetWord( colx - 1 );
    valy = line.GetWord( coly - 1 );
    addPoint( DataPoint( valx.AsDouble(), valy.AsDouble() ), false );
  }
  addPoint( DataPoint( valx.AsDouble(), valy.AsDouble() ), false );

  repaint( false );
}


void
AvidaPlotWidget::updateData()
{

  // if we don't have an active Avida object, do nothing.
  if ( m_pop_wrap == 0 || m_pop_wrap->getPopulation() == 0)
    return;


  //m_pop_wrap->lock();
  // the x value is the time;
  double x = m_pop_wrap->getPopulation()->GetStats().GetUpdate();
  // the y value depends on the data type chosen
  double y = AvidaData::getValue( m_type, m_pop_wrap->getPopulation() );
  //m_pop_wrap->unlock();

  addPoint( DataPoint( x, y ) );
}


void
AvidaPlotWidget::avidaDestroyed()
{
  // we set the avida object to zero. The plot widget is essentially
  // frozen now.
  m_pop_wrap = 0;

}

//#include "avida_plot_widget.moc"

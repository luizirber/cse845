#include "plot_widget.hh"

#include <qapplication.h>


//
// Create and display our widget.
//

int main( int argc, char **argv )
{
  QApplication app( argc, argv );

  PlotWidget w;
  app.setMainWidget( &w );
  //w.show();
  for ( int i=0; i<2000; i++ ){
    w.addPoint( DataPoint( i, sin(i/100.) ) );
    app.processEvents();
    for ( int j=0; j<100; j++ )
      cos( j );
  }
  w.show();
  return app.exec();
}

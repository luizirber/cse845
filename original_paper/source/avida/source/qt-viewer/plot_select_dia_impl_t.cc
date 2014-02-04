#include "plot_select_dia_impl.hh"

#include <qapplication.h>
#include <iostream.h>

//
// Create and display our widget.
//

/* changed to int.  -- kaben. */
//void main( int argc, char **argv )
int main( int argc, char **argv )
{
  QApplication app( argc, argv );

  PlotSelectDiaImpl w;
  app.setMainWidget( &w );

  if ( w.exec() ){
    cout << "accepted" << endl;
    cout << "result: " << w.result() << endl;
  }
  else {
    cout << "rejected" << endl;
  }
  /* added return.  -- kaben. */
  return 0;
}

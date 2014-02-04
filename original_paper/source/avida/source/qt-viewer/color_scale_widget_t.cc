#include "color_scale_widget.hh"

#include <qapplication.h>
#include <qvaluevector.h>

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    ColorScaleWidget w( 0 );
    w.resize( 200, 200 );
    w.setCaption( "Continuous Color Scale" );
    w.setRange( 0, 30 );
    w.activate();

    ColorScaleWidget w2( 0 );
    w2.resize( 200, 200 );
    w2.setCaption( "Discrete Color Scale" );
    QValueVector<QString> str_vect;
    QValueVector<QColor> color_vect;
    color_vect.push_back( Qt::black );
    color_vect.push_back( Qt::blue );
    color_vect.push_back( Qt::red );
    color_vect.push_back( Qt::green );
    color_vect.push_back( Qt::yellow );
    color_vect.push_back( Qt::white );

    str_vect.push_back( "black color" );
    str_vect.push_back( "blue color" );
    str_vect.push_back( QString::null );
    str_vect.push_back( "" );
    str_vect.push_back( "yellow color" );

    w2.setColorList( color_vect, str_vect );
    w2.activate();



    app.setMainWidget( &w2 );
    w.show();
    w2.show();
    return app.exec();
}



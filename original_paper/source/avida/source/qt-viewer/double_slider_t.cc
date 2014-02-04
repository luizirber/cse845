#include "double_slider.hh"

#include <qapplication.h>
#include <qslider.h>
#include <qstring.h>

#include <iostream>

class A : public QObject 
{
  Q_OBJECT
private:
  QString m_text;
  DoubleSlider *m_slider;
  
public:
  A( const QString &t, DoubleSlider *s );
  ~A();
  
public slots:  
  void outputValues();
};


A::A( const QString &t, DoubleSlider *s )
  : m_text( t ), m_slider( s )
{
  connect( s, SIGNAL( stateChanged( ) ),
	   this, SLOT( outputValues() ) );
}

A::~A()
{
}


void 
A::outputValues()
{
  pair<double, double> p = m_slider->getPositions();
  cout << m_text.latin1() << ": " << p.first << " " << p.second << endl;
}


int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    DoubleSlider w( Qt::Horizontal, 0 );
    w.resize( 200, 200 );
    w.setCaption( "Horizontal" );
    A a( "Horizontal", &w );
    
    DoubleSlider w2( Qt::Vertical, 0 );
    w2.resize( 200, 200 );
    w2.setCaption( "Vertical" );
    A a2( "Vertical", &w2 );
    
    QSlider w3 ( Qt::Horizontal, 0 );
    w3.resize( 200, 200 );

    QSlider w4 ( Qt::Vertical, 0 );
    w4.resize( 200, 200 );


    app.setMainWidget( &w );
    w.show();
    w2.show();
    w3.show();
    w4.show();

    return app.exec();
}


#include "double_slider_t.moc"

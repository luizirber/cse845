#include "init_file_view.hh"

#ifndef IOSTREAM_STL
#include <iostream>
#define IOSTREAM_STL
#endif

#include <qfile.h>
#include <qtextstream.h>
#include <qstylesheet.h>


InitFileView::InitFileView( QString file_name, QWidget * parent )
  : QTextView( parent ), m_comment_regexp( "#.*$" ),
    m_no_white_regexp( "[^\\s]" ), m_white_regexp( "\\s" )
{
  setCaption( file_name );
  setFile( file_name );
  resize( sizeHint() );
}


InitFileView::~InitFileView()
{
}


QSize
InitFileView::sizeHint()
{
  return QSize( fontMetrics().width( "x" ) * 100, fontMetrics().height() * 30 );
}

void
InitFileView::setFile( QString file_name )
{
  readFile( file_name );
}


void
InitFileView::readFile( QString file_name )
{
  QString text;

  QFile f( file_name );

  if ( f.open(IO_ReadOnly) ) {    // file opened successfully
    text = "<nobr><pre><tt>";
    QTextStream t( &f );        // use a text stream
    while ( !t.eof() ) {        // until end of file...
      text += highlightLine( t.readLine() );  // line of text excluding '\n'
    }
    f.close();
    text += "</tt></pre></nobr>\n";
  }
  else {
    text = "File <tt>" + file_name + "</tt> could not be opened!\n";
  }
  //  cout << "Text: " << text << endl;
  setText( text );
}

QString
InitFileView::highlightLine( QString line )
{
  QString result;
  int cpos = m_comment_regexp.match( line );
  QString comment = line;
  comment.remove( 0, cpos );
  line.truncate( cpos );

  // find the first occurrence of non-whitespace
  int pos = m_no_white_regexp.match( line );
  if ( pos >= 0 ){
    pos = m_white_regexp.match( line, pos );
    QString value = line;
    value.remove( 0, pos );
    line.truncate( pos );
    if ( !value.isEmpty() ){
      line = "<b>" + line + "</b>" + value;
    }
  }

  if ( comment.isEmpty() )
    result = line + "\n";
  else
    result = line + "<i><font face = \"Courier\" color = \"red\"> " + comment + "</font></i>\n";
  return result;
}




//#include "init_file_view.moc"




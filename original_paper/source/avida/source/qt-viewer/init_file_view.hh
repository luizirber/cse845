#ifndef INIT_FILE_VIEW_HH
#define INIT_FILE_VIEW_HH


#include <qtextview.h>
#include <qregexp.h>


/**
 * This class can be used to display an initialization file, such as the
 * genesis file, in a window. 
 **/

class InitFileView : public QTextView {
  Q_OBJECT
private:
  // regular expression that we need frequently
  const QRegExp m_comment_regexp;
  const QRegExp m_no_white_regexp;
  const QRegExp m_white_regexp;
  
protected:
  void readFile( QString file_name );
  QString highlightLine( QString line );
  
  
public:
  InitFileView( QString file_name, QWidget * parent=0 );
  ~InitFileView();
  
  /**
   * Returns a recommended size. Overloaded from QWidget.
   **/
  QSize sizeHint();
  
public slots:
  /**
   * Changes the contents of the window to the file given by file_name.
   **/
  void setFile( QString file_name );
  
};








#endif

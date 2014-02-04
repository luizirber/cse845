//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

/* qt gui classes */
#include <qpainter.h>
#include <qpixmap.h>
#include <qsimplerichtext.h>
#include <qstylesheet.h>
#include <qfont.h>
#include <qstring.h>
#include <qregion.h>
#include <qrect.h>
#include <qbrush.h>

#include "code_widget.hh"

#include "population_cell_wrapped_accessors.hh"
#include "population_cell_wrapper.hh"
#include "organism.hh"
#include "hardware_cpu.hh"
#include "population_cell.hh"


#ifndef DEBUG
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#else
//# define LOCAL_DEBUG 1
//# define USE_LOCAL_STRINGS 1
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#endif

#if USE_LOCAL_STRINGS
# define DSTR
#else
# define DSTR(ignore...) ("")
#endif



CodeWidget::CodeWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QScrollView(parent, name, f)
{
  m_pixmap = new QPixmap();
  m_style_sheet = new QStyleSheet(this);
  m_rich_text = 0;
  m_pm_width = 0;
  m_pm_height = 0;

  viewport()->setBackgroundColor( white );

  setText("");
}

//
// Clean up
//
CodeWidget::~CodeWidget()
{
  delete m_pixmap;
}


void
CodeWidget::drawViewport()
{
  viewport()->repaint( false );
}


void
CodeWidget::adjustSize(void){
  setText(
    "<nobr><pre><code>\n<font size=2 color=\"darkGray\">"
    +
    codeLine(
      1,
      "xxxxxxxxxxxxxxxxxxxx",
      "    ",
      "    ",
      false,
      false,
      false
    )
    +
    "</font><font color=\"black\">"
  );
  setMinimumWidth(m_pm_width);
  //setMinimumHeight(16 * m_pm_height);
  setText("");
}


void
CodeWidget::setText(
  const QString &text
)
{
  bool resize_pm = false;
  if ( m_rich_text )
    delete m_rich_text;

  QFont font("courier");
  font.setPointSize(9);
  //m_rich_text = new QSimpleRichText( text, QFont( "courier" ) );
  m_rich_text = new QSimpleRichText(text, font);
  m_rich_text->adjustSize();
  m_changed = true;
  //m_width = m_rich_text->width();
  m_width = m_rich_text->widthUsed();
  m_height = m_rich_text->height();

  if ( m_width > m_pm_width ){
    m_pm_width = m_width;
    resize_pm = true;
  }
  
  if ( m_height > m_pm_height ){
    m_pm_height = m_height;
    resize_pm = true;
  }
  // resize the pixmap
  m_pixmap->resize( m_pm_width, m_pm_height );
  
  // resize the contents of the scroll widget
  resizeContents( m_width, m_height );
  
  drawViewport();
}


void
CodeWidget::drawContents(
  QPainter *p,
  int clipx,
  int clipy,
  int clipw,
  int cliph
)
{
  // we always redraw the whole viewport. Otherwise, it doesn't really
  // work right now.
  clipx = contentsX();
  clipy = contentsY();
  clipw = visibleWidth();
  cliph = visibleHeight();

  QBrush b( white );
  
  if ( m_changed ){ // we have to redraw the rich-text

    m_pixmap->fill( white );

    QPainter p2( m_pixmap );

    m_rich_text->draw( &p2, 0, 0, QRegion(0, 0, m_width, m_height),
         colorGroup(), &b );
    p2.end(); 
    m_changed = false;
       
    p->drawPixmap( 0, 0, *m_pixmap );
  }
  else
    p->drawPixmap( 0, 0, *m_pixmap );
}


QString
CodeWidget::codeLine(
  int line,
  const QString &command,
  const QString &heads,
  const QString &flags,
  bool brk,
  bool exec,
  bool highlit
){
  QString text;
  text += (brk)?("  b   "):("      ");
  text += QString("%1").arg(line, -5);
  QString h = heads + "  ";
  h.truncate(6);
  text += h;
  h = flags + "  ";
  h.truncate(6);
  text += h + command;

  text = m_style_sheet->convertFromPlainText(
    text
  );

  if(highlit){
    return "<font color=\"red\"><b>" + text + "</b></font>";
  }else if (exec){
    return "<font color=\"blue\">" + text + "</font>";
  }else{
    return "<font color=\"black\">" + text + "</font>";
  }
}

void
CodeWidget::updateState(PopulationCellWrapper *cw){
  updateState(
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(cw)
      )
    )
  );
}


void
CodeWidget::updateState(cHardwareCPU *hardware){
  if(hardware == NULL){
    setUpdatesEnabled(false);
    setText("<nobr><pre><code>\n<font size=2 color=\"darkGray\"></font><font color=\"black\">");
    setUpdatesEnabled(true);
    repaint();

  }else{
    const cCPUMemory &memory = hardware->IP().GetMemory();
    const cInstLib &inst_lib = hardware->GetInstLib();
    //QString text = "<nobr><pre><code>";
    QString text;
    QString heads;
    QString flags;
    int cur_line = 0;
    bool hilit, exec;
    int ih_pos = hardware->GetHead(HEAD_IP).GetPosition();
    int rh_pos = hardware->GetHead(HEAD_READ).GetPosition();
    int wh_pos = hardware->GetHead(HEAD_WRITE).GetPosition();
    int fh_pos = hardware->GetHead(HEAD_FLOW).GetPosition();
    for (int i = 0; i < memory.GetSize(); i++){
      heads = "    ";
      flags = "    ";
      hilit = false;

      if (i == ih_pos){
        heads.at(0) = 'I';
        hilit = true;
        cur_line = i;
      }
      if (i == rh_pos){
        heads.at(1) = 'R';
      } 
      if (i == wh_pos){
        heads.at(2) = 'W';
      } 
      if (i == fh_pos){
        heads.at(3) = 'F';
      }

      if (memory.FlagCopied(i) == true){
        flags.at(0) = 'c';
      }
      if (memory.FlagMutated(i) == true){
        flags.at(1) = 'm';
      }
      if (memory.FlagExecuted(i) == true){
        flags.at(2) = 'e';
      }
      /*
      if (memory.FlagBreakpoint(i) == true){
        flags.at(3) = 'b';
      }
      */

      text += codeLine(
        i+1,
        inst_lib.GetName(memory[i])(),
        heads,
        flags,
        memory.FlagBreakpoint(i),
        exec,
        hilit
      );
    }
    //text += "</code></pre></nobr>\n";
    setUpdatesEnabled(false);
    setText(text);
    QFontMetrics fm = fontMetrics();
    setUpdatesEnabled(true);
    if (cur_line > 5 ){
      ensureVisible(
        0,
        (cur_line + 2) * fm.lineSpacing(),
        0,
        3 * fm.lineSpacing()
      );
    }else{
      ensureVisible(
        0,
        cur_line * fm.lineSpacing()
      );
    }
    repaint();
  }
} 




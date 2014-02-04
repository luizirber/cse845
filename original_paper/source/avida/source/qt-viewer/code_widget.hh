//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CODEWIDGET_HH
#define CODEWIDGET_HH

#include <qscrollview.h>

// gui declarations
class QPixmap;
class QSimpleRichText;
class QStyleSheet;
class PopulationCellWrapper;
class cHardwareCPU;


class CodeWidget : public QScrollView {
  Q_OBJECT
public:
  CodeWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  ~CodeWidget(void);
  void adjustSize(void);
  void setText(const QString &text);
public slots:
  void updateState(PopulationCellWrapper *cw);
  void updateState(cHardwareCPU *hardware);
private:
  QString
  codeLine(
    int line,
    const QString &command,
    const QString &heads,
    const QString &flags,
    bool brk,
    bool exec,
    bool highlit
  );
public slots:
  void drawViewport(void);
protected:
  void drawContents(
    QPainter *,
    int clipx,
    int clipy,
    int clipw,
    int cliph
  );
private:
  QPixmap *m_pixmap;
  QSimpleRichText *m_rich_text;
  QStyleSheet *m_style_sheet;
  bool m_changed;
  int m_width;
  int m_height;

  int m_pm_width;
  int m_pm_height;
};


#endif /* !CODEWIDGET_HH */

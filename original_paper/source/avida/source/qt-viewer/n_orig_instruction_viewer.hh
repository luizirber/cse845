//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef N_ORIG_INSTRUCTION_VIEWER_HH
#define N_ORIG_INSTRUCTION_VIEWER_HH

#include <qwidget.h>
#include <qguardedptr.h>

class MessagingPopulationWrapper;
class QVBox;
class QHBox;
class QVBoxLayout;
class QHBoxLayout;
class QScrollView;
class N_Instruction_ViewWidget;
class N_Instruction_ButtonsWidget;
class N_Instruction_CPUWidget;
class QPushButton;

class N_Instruction_Viewer : public QWidget {
  Q_OBJECT
private:
  QGuardedPtr<MessagingPopulationWrapper> m_pop_wrap;
  int m_cell_id;
  QHBoxLayout *m_hboxlayout1;
    QVBoxLayout *m_vboxlayout1;
      N_Instruction_ViewWidget *m_instruction_view_widget;
      N_Instruction_ButtonsWidget *m_instruction_buttons_widget;
    QVBoxLayout *m_vboxlayout2;
      N_Instruction_CPUWidget *m_instruction_cpu_widget;
      
public:
  N_Instruction_Viewer(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WDestructiveClose
  );
  ~N_Instruction_Viewer(void);
  void setPopulationWrapper(MessagingPopulationWrapper *pop_wrap);
public slots:
  void setPopulationCell(int cell_id);
  void updateState(void);
private slots:
  void disableUnsafeButtonsSlot(void);
  void enableUnsafeButtonsSlot(void);
  void guiStatusSlot(bool stopped){
    if(stopped){enableUnsafeButtonsSlot();}
    else{disableUnsafeButtonsSlot();}
  }
  void localStepSlot(void);
signals:
  void buttonsSafeSig(bool safe);
  void queryGUISig(void);
  
  void localStepSig(int);
};


#endif /* !N_ORIG_INSTRUCTION_VIEWER_HH */


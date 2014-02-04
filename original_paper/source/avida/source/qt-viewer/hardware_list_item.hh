//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARELISTITEM_HH
#define HARDWARELISTITEM_HH

#include <qguardedptr.h>

#include "avd_widget_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;
class QVBox;

// temporary
class QPushButton;

// avida declarations
class cPopulationCell;
class cOrganism;

// subwidget declarations
class CodeWidget;

class HardwareListItem : public avd_WidgetListItem {
  Q_OBJECT
public:
  HardwareListItem(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  void setPopulationCellWrapper(
    PopulationCellWrapper *population_cell_wrapper
  );
public slots:
  void updateState(PopulationCellWrapper *cw);
protected:
protected:
  QGuardedPtr<PopulationCellWrapper> m_population_cell_wrapper;
  QVBox *m_vbox;
  CodeWidget *m_code_view;
signals:
  void temporaryStepSig(int cell_id);
public slots:
  void temporaryStepSlot(void);
private:
  QPushButton *m_temporary_start_button;
  QPushButton *m_temporary_stop_button;
  QPushButton *m_temporary_step_button;
  QPushButton *m_temporary_update_button;
};


#endif /* !HARDWARELISTITEM_HH */

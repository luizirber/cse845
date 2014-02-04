//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INPUTSLISTITEM_H
#define INPUTSLISTITEM_H

#include <qvaluevector.h>

#include "avd_cell_view_list_item.hh"


class cPopulationCell;
class PopulationCellWrapper;
class LabeledFieldConvenience;

class InputsListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  InputsListItem(
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
  QValueVector<LabeledFieldConvenience *> m_input_vector;
};


#endif /* !INPUTSLISTITEM_H */

//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATIONRATESLISTITEM_H
#define MUTATIONRATESLISTITEM_H

#include "avd_cell_view_list_item.hh"


// gui declarations
class PopulationCellWrapper;
class LabeledFieldConvenience;

// avida declarations
class cMutationRates;

// subwidget declarations


class MutationRatesListItem : public avd_CellViewListItem {
  Q_OBJECT
public:
  MutationRatesListItem(
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
  LabeledFieldConvenience *m_copy_mut_prob;
  LabeledFieldConvenience *m_ins_mut_prob;
  LabeledFieldConvenience *m_del_mut_prob;
  LabeledFieldConvenience *m_point_mut_prob;
  LabeledFieldConvenience *m_divide_mut_prob;
  LabeledFieldConvenience *m_divide_ins_prob;
  LabeledFieldConvenience *m_divide_del_prob;
  LabeledFieldConvenience *m_crossover_prob;
  LabeledFieldConvenience *m_aligned_cross_prob;
  LabeledFieldConvenience *m_exe_err_prob;
private:
};


#endif /* !MUTATIONRATESLISTITEM_H */

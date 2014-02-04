//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_CELLVIEWLISTITEM_H
#define AVD_CELLVIEWLISTITEM_H

#include <qguardedptr.h>

#include "avd_widget_list_item.hh"


class cPopulationCell;
class PopulationCellWrapper;
class QGrid;


/**
 * The avd_CellViewListItem subclasses a widget list item, adding
 * management of an instance of the PopulationCellWrapper class to help
 * update data fields when the tracked Avida population cell data
 * changes.  This class also contains a QGrid in its container area to
 * simplify the display of data using instances of the
 * LabeledFieldConvenience class.
 *
 * Subclasses of avd_CellViewListItem should define one instance of
 * LabeledFieldConvenience for each Avida population cell datum to be
 * tracked by the subclass; the subclass should provide the member
 * function <pre>updateState(cPopulationCell *cell)</pre>, which will be
 * called whenever the cell data changes, and which should update the
 * data fields appropriately.  As an example -- in the constructor, the
 * following sequence might appear:
 *
 * <pre>
 *   m_data_field = new LabeledFieldConvenience(
 *     "the data:",
 *     m_grid
 *   );
 * </pre>
 *
 * and in the <pre>updateState(cPopulationCell *cell)</pre> function,
 * something like the following should then appear:
 *
 * <pre>
 *   if(cell == NULL)
 *     m_data_field->setText("");
 *   else
 *     m_data_field->setText(
 *       QString("%1").arg(
 *         cell->GetData()
 *       )
 *     );
 * </pre>
 *
 * The subclass should also provide the member function
 * <pre>setPopulationCellWrapper(PopulationCellWrapper *wrapper)</pre>
 * with at least the following chaining:
 *
 * <pre>
 *   avd_CellViewListItem::setPopulationCellWrapper(wrapper);
 * </pre>
 *
 * For each subcelllistitem of the subclass, this function should also
 * call:
 *
 * <pre>
 *   subcelllistitem->setPopulationCellWrapper(wrapper);
 * </pre>
 *
 * @short avd_WidgetListItem subclass for viewing population cell data.
 * @version $Id: avd_cell_view_list_item.hh,v 1.1 2002/02/12 12:27:46 kaben Exp $
 * @see avd_WidgetListItem
 * @see PopulationCellWrapper
 * @see LabeledFieldConvenience
 */
class avd_CellViewListItem : public avd_WidgetListItem {

public:

  /**
   * avd_CellViewListItem constructor.
   *
   * @param parent  The parent widget.  Often NULL or omitted.
   * @param name  Qt identifier; used in Qt debugging messages and Designer.
   * @param f  Qt widget flags; typically 0 or omitted.
   * @see avd_WidgetListItem
   */
  avd_CellViewListItem(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );

  /**
   * Sets the PopulationCellWrapper used to receive signals of changes
   * to the tracked Avida population cell data.
   */
  void setPopulationCellWrapper(
    PopulationCellWrapper *population_cell_wrapper
  );

protected:

  /**
   * The PopulationCellWrapper used to receive signals of changes to the
   * tracked Avida population cell data.  Note that this is remembered
   * only so that its signals may be disconnected if need be.  It is
   * otherwise never referenced.
   */
  QGuardedPtr<PopulationCellWrapper> m_population_cell_wrapper;

  QGrid *m_grid;

};

#endif /* !AVD_CELLVIEWLISTITEM_H */

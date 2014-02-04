#ifndef CREATURE_VIEW_HH
#define CREATURE_VIEW_HH

#include <qwidget.h>


class QLabel;

class MessagingPopulationWrapper;
class cPopulation;
class cPopulationCell;
class cOrganism;

class CreatureView : public QWidget
{
  Q_OBJECT
private:
  int m_cell_pos;
  MessagingPopulationWrapper *m_pop_wrap; // pointer to the Avida world
  cPopulationCell *m_cell; // pointer to the cell currently under display.

  QLabel *m_cell_l;
  QLabel *m_merit_l;
  QLabel *m_fitness_l;
  QLabel *m_gestation_time_l;
  QLabel *m_length_l;
  QLabel *m_age_l;
  QLabel *m_genotype_l;
  QLabel *m_lineage_l;

  // set up all the sub-widgets
  void setup();
public:
  CreatureView(
    QWidget *parent,
    MessagingPopulationWrapper *pop_wrap,
    int cell = 0
  );
  ~CreatureView();

public slots:

  /**
   * Updates the view with the newest information.
   **/
  void drawView();

  /**
   * The the cell that contains the creature this widget should show.
   **/
  void setCell( int cell );
};





#endif

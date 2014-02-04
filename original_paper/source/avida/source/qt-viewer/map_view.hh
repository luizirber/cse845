#ifndef MAP_VIEW_HH
#define MAP_VIEW_HH

#ifndef MAP_VIEW_WIDGET_HH
#include "map_view_widget.hh"
#endif

#include <qwidget.h>

class QSlider;
class QComboBox;

//class cPopulation;
class MessagingPopulationWrapper;

class MapView : public QWidget
{
  Q_OBJECT
private:
  //cPopulation *m_population;  // a pointer to the avida population object
  MessagingPopulationWrapper *m_pop_wrap;  // pointer to avida population
  MapViewWidget *m_map_view_widget; // the main widget
  ColorScaleWidget *m_color_scale_widget; // the color scale widget
  QSlider *m_zoom_slider;
  QSlider *m_center_x_slider;
  QSlider *m_center_y_slider;
  QComboBox *m_mode_combo;

  void setup();

protected slots:
  void generateZoomValue( int value );
  void setMapMode( int value );

public:
  MapView(
    //cPopulation *population,
    MessagingPopulationWrapper *pop_wrap,
    QWidget * parent=0 );
  ~MapView();

  /**
   * This function returns the cell that is currently selected.
   **/
  int GetSelectedCell() const {
    return m_map_view_widget->GetSelectedCell(); }

public slots:
  void repaintMap();

signals:
  /**
   * This signal is emited if the zoom value has changed.
   **/
  void zoomValueChanged( double );

  /**
   * This signal is emited if a particular cell has been selected
   * by clicking on it.
   **/
  void cellSelected( int );
};


#endif



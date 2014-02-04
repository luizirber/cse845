//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "messaging_population_wrapper.hh"

#include "event_view_widget.hh"


#ifndef STRING_HH
#include "../tools/string.hh"
#endif

#ifndef INIT_FILE_HH
#include "../tools/file.hh"
#endif

#include "../event/event_list.hh"
//#include <sstream>
#include <strstream>
#include <qstringlist.h>
#include <qregexp.h>

#include <qlineedit.h>
#include <qlabel.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qtextview.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qevent.h>


//event descriptions
#include "../event/cPopulation_descr.hi"



EventItem::EventItem( QListView * parent )
  : QListViewItem( parent )
{
  m_ordering = listView()->childCount();
  m_trigg = "update";
  m_start = "0";
  m_step = "10";
  m_stop = "end";
  m_event = 1;
}


EventItem::EventItem( QListView * parent, EventItem *after )
  : QListViewItem( parent, after )
{
  if ( after )
    m_ordering = after->getOrder() + 1;
  else m_ordering = listView()->childCount();
  m_trigg = "update";
  m_start = "0";
  m_step = "10";
  m_stop = "end";
  m_event = 1;
}


EventItem::EventItem( QListView * parent, int ordering )
  : m_ordering(ordering), QListViewItem( parent )
{
  m_trigg = "update";
  m_start = "0";
  m_step = "10";
  m_stop = "end";
  m_event = 1;
}


EventItem::~EventItem()
{
  #if LOCAL_DEBUG
  cout << "<EventItem::~EventItem> closing.\n";
  #endif
}


QString
EventItem::text( int column ) const
{
  switch ( column ){
  case 0:
    return m_trigg;
  case 1:
    return m_start;
  case 2:
    return m_step;
  case 3:
    return m_stop;
  case 4:
    return cEventDescrs::entries[m_event].GetName();
  case 5:
    return m_param;
  default:
    return QString::null;
  }
}


QString
EventItem::fileFormat() const
{
  QString result = m_trigg;

  if ( !m_start.isEmpty() )
    result += " " + m_start;
  if ( !m_step.isEmpty() )
    result += ":" + m_step;
  if ( !m_stop.isEmpty() )
    result += ":" + m_stop;

  result += " " + QString( cEventDescrs::entries[m_event].GetName() ) + " " + m_param + "\n";
  return result;
}


/*
 *  Constructs a EventViewWidget which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
EventViewWidget::EventViewWidget( QWidget* parent,  const char* name, WFlags fl )
  : EventListEditor( parent, name, fl )
{
  m_events_lv->setAllColumnsShowFocus( true );
  m_events_lv->setSorting(-1);
  m_document_changed = false;

  m_event_name_lookup = new QDict<int>( 107 );
  m_event_name_lookup->setAutoDelete( true );

  QString event;
  for ( int i=0; i<cEventDescrs::num_of_events; i++ ){
    event = cEventDescrs::entries[i].GetName();
    m_event_cb->insertItem( event );
    // no memory leak here, because we have AutoDelete...
    m_event_name_lookup->insert( event, new int(i) );
  }
  updateEventDescr( 0 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
EventViewWidget::~EventViewWidget()
{
  // no need to delete child widgets, Qt does it all for us
  #if LOCAL_DEBUG
  cout << "<EventViewWidget::~EventViewWidget> closing.\n";
  #endif
}

void
EventViewWidget::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  m_pop_wrap = pop_wrap;
  #if LOCAL_DEBUG
  cout << "<EventViewWidget::setPopulationWrapper> m_pop_wrap set.\n";
  #endif
}

/*
 * protected slot
 */
void
EventViewWidget::updateEventItem(EventItem *item)
{
  if ( !item )
    return;

  //m_start_cb->blockSignals( true );
  //m_step_cb->blockSignals( true );
  //m_stop_cb->blockSignals( true );
  switch( m_trigger_group->id( m_trigger_group->selected() ) ){
  case 0:
    item->setTrigger( "immediate" );
    m_start_cb->lineEdit()->setText( "" );
    m_step_cb->lineEdit()->setText( "" );
    m_stop_cb->lineEdit()->setText( "" );
    m_interval_group->setEnabled(false);
    break;
  case 1:
    item->setTrigger( "update" );
    m_interval_group->setEnabled(true);
    break;
  case 2:
    item->setTrigger( "generation" );
    m_interval_group->setEnabled(true);
    break;
  //case 3:
  //  item->setTrigger( "r" );
  //  break;
  //case 4:
  //  item->setTrigger( "e" );
  //  break;
  }

  item->setStart( m_start_cb->currentText() );
  item->setStep( m_step_cb->currentText() );
  item->setStop( m_stop_cb->currentText() );
  item->setParameters( m_parameters_inp->text() );
  item->setEvent( m_event_cb->currentItem() );

  item->repaint();

  // the document has changed now.
  m_document_changed = true;
  m_file_name = QString::null;
}

/*
 * protected slot
 */
void
EventViewWidget::updateEventLV()
{
  EventItem *item = static_cast<EventItem *>( m_events_lv->currentItem() );
  cout << "<EventViewWidget::updateEventLV> item " << item << "\n";
  updateEventItem(item);
}

/*
 * protected slot
 */
void
EventViewWidget::updateEventDescr( int item )
{
  cout << "<EventViewWidget::updateEventDescr> item " << item << "\n";
  if(m_events_lv->currentItem() == 0) return;
  m_event_descr_tv->setText( cEventDescrs::entries[item].GetDescription() );
}

/*
 * protected slot
 */
void
EventViewWidget::updateEventInp()
{
  
  
  QListViewItem *item = m_events_lv->currentItem();
  if ( !item )
    return;

  m_trigger_group->blockSignals( true );
  QString trig = item->text( 0 );
  if ( trig == "i" || trig == "immediate" )
    m_imm_rb->setChecked( true );
  else if ( trig == "u" || trig == "update" )
    m_upd_rb->setChecked( true );
  else if ( trig == "g" || trig == "generation" )
    m_gen_rb->setChecked( true );
  //else if ( trig == "r" )
  //  m_realt_rb->setChecked( true );
  //else if ( trig == "e" )
  //  m_exec_rb->setChecked( true );
  m_trigger_group->blockSignals( false );

  //m_start_cb->blockSignals( true );
  //m_step_cb->blockSignals( true );
  //m_stop_cb->blockSignals( true );
  if(m_imm_rb->isOn()){
    m_start_cb->lineEdit()->setText( "" );
    m_step_cb->lineEdit()->setText( "" );
    m_stop_cb->lineEdit()->setText( "" );
    m_interval_group->setEnabled(false);
  } else {
    m_interval_group->setEnabled(true);
    m_start_cb->lineEdit()->setText( item->text( 1 ) );
    m_step_cb->lineEdit()->setText( item->text( 2 ) );
    m_stop_cb->lineEdit()->setText( item->text( 3 ) );
  }
  //m_start_cb->blockSignals( false );
  //m_step_cb->blockSignals( false );
  //m_stop_cb->blockSignals( false );

  m_start_cb->lineEdit()->setText( item->text( 1 ) );
  m_step_cb->lineEdit()->setText( item->text( 2 ) );
  m_stop_cb->lineEdit()->setText( item->text( 3 ) );

  m_parameters_inp->blockSignals( true );
  m_parameters_inp->setText( item->text( 5 ) );
  m_parameters_inp->blockSignals( false );

  m_event_cb->blockSignals( true );
  int i = static_cast<EventItem *>( item )->getEvent();
  m_event_cb->setCurrentItem( i );
  m_event_cb->blockSignals( false );
  updateEventDescr( i );
}

/*
 * protected slot
 */
void
EventViewWidget::deleteEvent()
{
  QListViewItem *item = m_events_lv->selectedItem();
  if ( !item )
    return;

  QListViewItem *prev_item = item->itemAbove();
  QListViewItem *next_item = item->itemBelow();

  /*
  reorder (by decreasing its ordering by one) each item below the one
  we're about to delete.
  */
  QListViewItemIterator it( item );
  it++;
  while ( it.current() ){
    EventItem *event_item = (EventItem *)(it.current());
    event_item->setOrder(
      event_item->getOrder() - 1
    );
    it++;
  }
  
  delete item;

  /*
  if there was an item below the one we just deleted, select it;
  otherwise, if there was an item above the one we deleted, select it;
  otherwise do nothing.
  */
  if( next_item ){
    m_events_lv->setCurrentItem( next_item );
    m_events_lv->setSelected( next_item, true );
  } else if( prev_item ){
    m_events_lv->setCurrentItem( prev_item );
    m_events_lv->setSelected( prev_item, true );
  }

  // the document has changed now.
  m_document_changed = true;
}

/*
 * protected slot
 */
void
EventViewWidget::lowerEvent()
{
  EventItem *item = (EventItem *)m_events_lv->currentItem();
  if ( !item )
    return;

  QListViewItemIterator it( item );
  QListViewItem *parent = item->parent();
  it++;
  while ( it.current() ) {
    if ( it.current()->parent() == parent )
      break;
    it++;
  }

  if ( !it.current() )
    return;
  EventItem *other = (EventItem *)it.current();

  item->moveItem( other );

  int orderswap = item->getOrder();
  item->setOrder(other->getOrder());
  other->setOrder(orderswap);

  // the document has changed now.
  m_document_changed = true;

}
/*
 * protected slot
 */
void
EventViewWidget::newEvent()
{
  EventItem *current_item = (EventItem *)m_events_lv->selectedItem();
  EventItem *prev_item = 0;
  if (current_item) {
    prev_item = (EventItem *)current_item->itemAbove();
  }

  //QListViewItemIterator it( item );

  /*
  if an item was already selected, try to place the new item immediately
  before it; otherwise place the new item at the bottom.
  */
  cout << "<EventViewWidget::newEvent> inserting...\n";
  EventItem *item;
  if( prev_item ){
    item = new EventItem( m_events_lv, prev_item );
  } else if (current_item) {
    item = new EventItem( m_events_lv, current_item->getOrder() );
  } else {
    item = new EventItem( m_events_lv );
  }
  cout << "<EventViewWidget::newEvent> done inserting...\n";

  /*
  unless the new item was placed at the bottom of the list, there are
  items below it whose orderings need to be incremented by one.
  */
  cout << "<EventViewWidget::newEvent> reordering...\n";
  if (current_item){
    QListViewItemIterator it( item );
    it++;
    while ( it.current() ){
      EventItem *event_item = (EventItem *)(it.current());
      event_item->setOrder( event_item->getOrder() + 1);
      it++;
    }
  }
  cout << "<EventViewWidget::newEvent> done reordering...\n";

  /*
  select the new item.
  */
  cout << "<EventViewWidget::newEvent> selecting...\n";
  item->setText( 0, "Item" );
  m_events_lv->setCurrentItem( item );
  m_events_lv->setSelected( item, TRUE );
  updateEventItem(item);
  cout << "<EventViewWidget::newEvent> done selecting...\n";

  // the document has changed now.
  m_document_changed = true;

  cout << "<EventViewWidget::newEvent> done.\n";
}

/*
 * protected slot
 */
void
EventViewWidget::raiseEvent()
{
  EventItem *item = (EventItem *)m_events_lv->currentItem();
  if ( !item )
    return;

  QListViewItemIterator it( item );
  QListViewItem *parent = item->parent();
  --it;
  while ( it.current() ) {
    if ( it.current()->parent() == parent )
      break;
    --it;
  }

  if ( !it.current() )
    return;
  EventItem *other = (EventItem *)it.current();

  other->moveItem( item );

  int orderswap = item->getOrder();
  item->setOrder(other->getOrder());
  other->setOrder(orderswap);

  // the document has changed now.
  m_document_changed = true;

}

/*
 * protected slot
 */
void
EventViewWidget::clearClicked()
{
  if ( !discardSafely() ) // we don't clear if save was aborted.
    return;

  m_events_lv->clear();
  m_document_changed = false;
}


/*
 * protected slot
 */
void
EventViewWidget::openClicked()
{
  if ( !discardSafely() ) // we don't load if save was aborted.
    return;

  QString file_name = QFileDialog::getOpenFileName( QString::null,
  "Event List (*.el *.cfg)", this );

  if ( !file_name.isNull() )                  // got a file name
    loadFromFile( file_name );  // loadFromFile updates m_file_name
  updateEventInp();
}


void
EventViewWidget::loadFromFile( const QString &file_name )
{
  cInitFile event_file( file_name.latin1() );

  // Load in the proper event list and set it up.
  event_file.Load();
  event_file.Compress();

  m_events_lv->clear();

  EventItem *last_item = 0;

  cout << "<EventViewWidget::loadFromFile> inserting events...\n";
  while ( event_file.GetNumLines()>0 ) {
    last_item = addEventFileFormat(event_file.RemoveLine(), last_item);
  }
  cout << "<EventViewWidget::loadFromFile> done inserting events...\n";
  cout << "<EventViewWidget::loadFromFile> sorting...\n";
  m_events_lv->sort();
  cout << "<EventViewWidget::loadFromFile> done sorting...\n";

  m_document_changed = false;
  m_file_name = file_name;
  m_file_name_l->setText( m_file_name );
  cout << "<EventViewWidget::loadFromFile> done.\n";
}


/*
 * protected slot
 */
void
EventViewWidget::loadFromMemoryClicked()
{
  //#if LOCAL_DEBUG
  cout << "<EventViewWidget::loadFromMemoryClicked> called.\n";
  //#endif
  if ( !discardSafely() ) // we don't load if save was aborted.
    return;

  loadFromMemory();
  updateEventInp();
}


void
EventViewWidget::loadFromMemory()
{
  if(m_pop_wrap == 0)
    return;

  m_pop_wrap->lock();
  if(m_pop_wrap->getEventList() == 0){
    m_pop_wrap->unlock();
    return;
  }

  m_events_lv->clear();

  //#if LOCAL_DEBUG
  cout << "<EventViewWidget::loadFromMemory> starting iterator test.\n";
  //#endif

  EventItem *last_item = 0;

  for(
    cEventListIterator it = m_pop_wrap->getEventList()->begin();
    it != m_pop_wrap->getEventList()->end();
    ++it
  ){

    #if LOCAL_DEBUG
    it.PrintEvent();
    #endif

    /*
    ugly but functional conversion from stringstream to QString to,
    finally, EventItem via addEventFileFormat().  -- K.
    */
    strstream tmp_sstream;
    it.PrintEvent(tmp_sstream);
    QString event_string(&tmp_sstream.str()[0]);
    event_string.truncate(tmp_sstream.pcount());
    last_item = addEventFileFormat(event_string, last_item);
  }

  //#if LOCAL_DEBUG
  cout << "<EventViewWidget::loadFromMemory> iterator test done.\n";
  //#endif

  m_pop_wrap->unlock();

  //m_events_lv->sort();
  m_document_changed = false;
}


EventItem *
EventViewWidget::addEventFileFormat(
  const cString& in_line,
  EventItem *last_item
){
  return addEventFileFormat(QString(in_line), last_item);
}


EventItem *
EventViewWidget::addEventFileFormat(
  const QString& in_line,
  EventItem *last_item
){
  #define LOCAL_DEBUG 1
  #if LOCAL_DEBUG
  cout
  << "<EventViewWidget::addEventFileFormat> "
  << "active:  " <<
  "06:18:46"
  << "\n";
  cout
  << "<EventViewWidget::addEventFileFormat>\n"
  << " --- in_line:  \"" << in_line << "\"\n"
  << " --- *last_item:  \"" << last_item << "\"\n";
  #endif

  // Timing
  QString trigger = "immediate";
  QString start = "begin";
  QString interval = "once";
  QString stop = "end";
  QString event = "";
  QString params = "";

  bool is_immediate = true;

  QStringList word_list(
    QStringList::split(
      QRegExp("\\s"),
      QString(in_line)
    )
  ); 

  QStringList::Iterator cur_word = word_list.begin();

  if(cur_word == word_list.end()) return 0;
  //if( *cur_word == "i"
  //    ||
  //    *cur_word == "immediate" 
  //    ||
  if( *cur_word == "u"
      ||
      *cur_word == "update"
      ||
      *cur_word == "g"
      ||
      *cur_word == "generation"
  ){
    trigger = *cur_word;
    ++cur_word;
    is_immediate = false;

    #if LOCAL_DEBUG
    cout << " --- trigger:  \"" << trigger << "\".\n";
    #endif
  }else if( *cur_word == "i"
      ||
      *cur_word == "immediate" 
  ){
    trigger = *cur_word;
    ++cur_word;
    is_immediate = true;

    #if LOCAL_DEBUG
    cout << " --- trigger:  \"" << trigger << "\".\n";
    #endif
  }else{
    // If Trigger is skipped then assume IMMEDIATE
    trigger = "immediate";
    is_immediate = true;
    #if LOCAL_DEBUG
    cout << " --- immediate trigger assumed.\n";
    #endif
  }
  
  /*
  FIXME:  handle error condition below.
  */
  if(cur_word != word_list.end()){
    // Do we now have timing specified?
    // Parse the Timing; get the start:interval:stop
    QStringList timing_list(
      QStringList::split(
        ":",
        *cur_word   
      )
    ); 
    QStringList::Iterator timing_str = timing_list.begin();

    // If first value is valid, we are getting a timing.
    if(timing_str == timing_list.end()){
      // We don't have timing, so assume IMMEDIATE
      trigger = "immediate";
      is_immediate = true;
      #if LOCAL_DEBUG
      cout << " --- timing_str is null; assuming immediate.\n";
      #endif
    } else {
      bool ok;
      int number;
      number = (*timing_str).toInt(&ok);
      if(ok || *timing_str == "begin"){
        // First number is start.
        start = *timing_str;

        #if LOCAL_DEBUG
        cout << " --- start:  " << start << ".\n";
        #endif

        if(++timing_str != timing_list.end()){
          interval = *timing_str;

          #if LOCAL_DEBUG
          cout << " --- interval:  " << interval << ".\n";
          #endif

          if(++timing_str != timing_list.end()){
            stop = *timing_str;

            #if LOCAL_DEBUG
            cout << " --- stop:  " << stop << ".\n";
            #endif
          } else {
            // If no other words... is "start:interval" syntax
            #if LOCAL_DEBUG
            cout << " --- assuming \"start:interval\" syntax.\n";
            #endif
          }
        } else {
          // If no other words... is "start" syntax
          #if LOCAL_DEBUG
          cout << " --- assuming \"start\" syntax.\n";
          #endif
        }
        // timing provided, so get next word
        ++cur_word;
      } else {
        // We don't have timing, so assume IMMEDIATE
        trigger = "immediate";
        is_immediate = true;
        #if LOCAL_DEBUG
        cout << " --- no timing; assuming immediate.\n";
        #endif
      }
    }
  }

  if(cur_word != word_list.end()){
    event = *cur_word;
    ++cur_word;
  }
  while(cur_word != word_list.end()){
    params += *cur_word + " ";
    ++cur_word;
  }

  #if LOCAL_DEBUG
  cout << " --- event:  " << event << ".\n";
  cout << " --- params:  " << params << ".\n";
  #endif

  int* event_ind = m_event_name_lookup->find( event );
  EventItem *item = 0;
  if ( event_ind ){
    item = new EventItem(m_events_lv, last_item);
    item->setTrigger( trigger );
    if(is_immediate){
      item->setStart( "" );
      item->setStep( "" );
      item->setStop( "" );
    } else {
      item->setStart( start );
      item->setStep( interval );
      item->setStop( stop );
    }
    item->setEvent( *event_ind );
    item->setParameters( params );
  }
  return item;
  #define LOCAL_DEBUG 0
}


/*
 * protected slot
 */
void
EventViewWidget::saveClicked()
{
  // first we check whether we have something to save...
  if ( !m_events_lv->firstChild() )
    return;

  if ( m_file_name.isEmpty() )
    saveAsClicked(); // we really need 'Save as' now
  else
    saveToFile( m_file_name );
}

/*
 * protected slot
 */
void
EventViewWidget::saveAsClicked()
{
  // first we check whether we have something to save...
  if ( !m_events_lv->firstChild() )
    return;


  // get a file name
  QString file_name = m_file_name;

  if ( file_name.isEmpty() )
    file_name = "event_list.cfg";

  file_name = QFileDialog::getSaveFileName( file_name, "Event List (*.el *.cfg)", this );

  if ( !file_name.isNull() )                  // got a file name
    saveToFile( file_name );  // saveToFile updates m_file_name
}

void
EventViewWidget::saveToFile( const QString &file_name )
{
  // first we check whether we have something to save...
  EventItem *item = static_cast<EventItem *>( m_events_lv->firstChild() );
  if ( !item )
    return;

  if ( !file_name.isNull() ) {                 // got a file name
    QFile f( file_name );

    if ( f.open(IO_WriteOnly) ) {    // file opened successfully
      // update the file name
      m_file_name = file_name;
      m_file_name_l->setText( m_file_name );

      // now write the file
      QTextStream t( &f );        // use a text stream
      t.setEncoding( QTextStream::Latin1 );

      do {
	      t << item->fileFormat();
	      item = static_cast<EventItem *>( item->nextSibling() );
      }
      while ( item );       // until end of the list
      f.close();

      // the document is saved now.
      m_document_changed = false;
    }
  }
}


/*
 * protected slot
 */
void
EventViewWidget::commitToMemoryClicked()
{
  //#if LOCAL_DEBUG
  cout << "<EventViewWidget::commitToMemoryClicked> called.\n";
  //#endif
  // first we check whether we have something to save...
  if ( !m_events_lv->firstChild() )
    return;

  commitToMemory();
}


void
EventViewWidget::commitToMemory()
{
  //#if LOCAL_DEBUG
  cout << "<EventViewWidget::commitToMemory> called.\n";
  //#endif

  // first we check whether we have something to save...
  EventItem *item = static_cast<EventItem *>( m_events_lv->firstChild() );
  if ( !item )
    return;

  if(m_pop_wrap == 0)
    return;
  m_pop_wrap->lock();
  if(m_pop_wrap->getEventList() == 0){
    m_pop_wrap->unlock();
    return;
  }

  m_pop_wrap->getEventList()->DeleteAll();
  do {
    if(!item->fileFormat()){
      ; // do nothing
    } else {
      m_pop_wrap->getEventList()->AddEventFileFormat(
        cString(item->fileFormat().latin1())
      );
    }

    //#if LOCAL_DEBUG
    cout << " --- " << item->fileFormat() << ".\n";
    //#endif

	  item = static_cast<EventItem *>( item->nextSibling() );
  } while(item);

  m_pop_wrap->unlock();
  m_document_changed = false;
}


bool
EventViewWidget::discardSafely()
{
  //#if LOCAL_DEBUG
  cout << "<EventViewWidget::discardSafely> called.\n";
  //#endif
  if ( m_document_changed ){
    switch( QMessageBox::warning( this, "Message",
				  "This Event List has been modified.\nWould you like to save it?\n",
				  "&Yes", "&No", "&Cancel",
				  0, 2 ) ){
    case 0: // save before discarding
      saveClicked();
      return !m_document_changed; // in case save didn't succeed.
    case 1: // don't save
      return true;
    case 2: // cancel
      return false;
    default:
      /*
      if we get here then something strange has happened.
      */
      return false;
      break; // not reached.
    }
  }
  return true;
}


void
EventViewWidget::closeEvent( QCloseEvent *e )
{
  if ( !discardSafely() ) // we don't load if save was aborted.
    return;


  EventListEditor::closeEvent( e );
}


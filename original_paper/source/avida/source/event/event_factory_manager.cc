#include "event_factory_manager.hh"

#ifndef STRING_HH
#include "../tools/string.hh"
#endif

#ifndef EVENT_HH
#include "event.hh"
#endif

#ifndef EVENT_FACTORY_HH
#include "event_factory.hh"
#endif




/////////////////
//  cEventFactoryManager
/////////////////

cEventFactoryManager::cEventFactoryManager()
{
}

cEventFactoryManager::~cEventFactoryManager(){
  vector<cEventFactory*>::iterator it = m_factory_list.begin();

  for( ; it != m_factory_list.end(); it++ )
    delete *it;
}


cEvent*
cEventFactoryManager::ConstructEvent(const cString name,
			      const cString & args,
			      int factory_id){
  cEvent* event = NULL;

  // factory_id < 0 => send to all factories
  if( factory_id < 0 ){
    vector<cEventFactory*>::iterator it;
    for( it = m_factory_list.begin(); it != m_factory_list.end(); it++ ){
      if( *it != NULL )
	event = (*it)->ConstructEvent(name,args);
      if ( event != NULL ) // if we have found one factory that can create the
	//                    event we want we stop.
	break;
    }
  }
  else{
    // send to particular factory
    if ( factory_id >= static_cast<int>( m_factory_list.size() ) )
      return NULL;
    if( m_factory_list[factory_id] != NULL )
      event = m_factory_list[factory_id]->ConstructEvent(name,args);
  }
  return event;
}


int
cEventFactoryManager::AddFactory( cEventFactory *factory )
{
  assert( factory != NULL );
  m_factory_list.push_back( factory );

  int id = m_factory_list.size();
  factory->SetFactoryId( id );

  return id;
}



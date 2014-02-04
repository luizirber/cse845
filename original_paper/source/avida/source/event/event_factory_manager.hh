#ifndef EVENT_FACTORY_MANAGER_HH
#define EVENT_FACTORY_MANAGER_HH


#include <vector>

#ifndef DEFS_HH
#include "../defs.hh"
#endif

class cEventFactory;
class cEvent;
class cString;


/**
 * This class serves as a collection of different event factories
 * (@ref cEventFactory). It is useful to have different event factories if
 * we want to have events with different base types, say one type that operates
 * on class A, while the other operates on class B.
 **/

class cEventFactoryManager {
private:
  vector<cEventFactory*> m_factory_list;


  // not implemented, prevents inadvertent wrong instantiation
  cEventFactoryManager( const cEventFactoryManager & );
  cEventFactoryManager& operator=( const cEventFactoryManager& );
public:
  cEventFactoryManager();
  ~cEventFactoryManager();

  int AddFactory(cEventFactory* factory);

  /**
   * This function is used to construct an event. It sends the event's name
   * to all registered factories if no factory id is given, otherwise it
   * sends it to the particular factory requested.
   **/
  cEvent* ConstructEvent(const cString name, const cString & args,
			 int factory_id = -1);

};


#endif // #ifndef EVENT_MANAGER_HH


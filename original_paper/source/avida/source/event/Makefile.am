
## This is a somewhat ugly hack to build the autogenerated code.
## We pretend that the autogenerated code is actually a program, then
## make builds it correctly. Any other way leads to unnecessary 
## recompilation of libevent.a even if nothing has changed.
cPopulation_enums_auto_ci_SOURCES = cPopulation.events

noinst_PROGRAMS = cPopulation_enums_auto.ci

##
## change by kaben...
## this change places all generated files *.ci
## in the build directory
## rather than in the source directory.
##
#$(srcdir)/cPopulation_enums_auto.ci: $(srcdir)/cPopulation.events
#	perl $(srcdir)/make_events.pl $(srcdir)/cPopulation
#$(srcdir)/population_event_factory.cc: $(srcdir)/cPopulation_enums_auto.ci
cPopulation_enums_auto.ci: cPopulation.events
	perl $(srcdir)/make_events.pl $(<:%.events=%)
population_event_factory.o: cPopulation_enums_auto.ci
##

## now come the actual sources for libevent.a
libevent_a_SOURCES = event.cc event.hh			\
		event_factory.cc event_factory.hh   	\
		event_factory_manager.cc event_factory_manager.hh        \
		event_list.cc event_list.hh              \
		population_event_factory.cc		 \
		population_event_factory.hh

noinst_LIBRARIES = libevent.a


## make sure that the autogenerated code gets removed on clean
CLEANFILES = cPopulation_enums_auto.ci 		\
	cPopulation_name2enum_auto.ci  		\
	cPopulation_process_auto.ci        	\
	cPopulation_construct_event_auto.ci 	\
	cPopulation_descr.hi			\
	cPopulation_descr.ci			\
	cPopulation_event_list

## we need this in the distribution:
EXTRA_DIST = cPopulation.events make_events.pl


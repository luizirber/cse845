#
# qmakeMakefile.inc is a makefile-include integrating GNU autoconf and
# automake with Trolltech's qmake tool.  this helps manage projects
# employing Trolltech's GUI library "Qt."
#
# for help using qmake, see http://doc.trolltech.com/3.0/qmake-guide.html
# for information about Trolltech and Qt, see http://www.trolltech.com
#
# this file and the accompanying example project were constructed
# December 2001 for use in the Avida project at Caltech.
#
# for information about Avida, see http://dllab.caltech.edu/avida/
#


#
# construction of the default qmake command.
#

# grabbing qmake options from configure...
if QMAKE_DEBUG
qmake_debug = debug
qmake_debug_def = DEBUG
else
qmake_debug = release
endif

if QMAKE_WARN
qmake_warn = warn_on
else
qmake_warn = warn_off
endif

# default arguments to qmake for .pro file generation.
QMAKE_PRO_ARGS  = "OBJECTS_DIR = ."
QMAKE_PRO_ARGS += "MOC_DIR = ."
QMAKE_PRO_ARGS += "UI_DIR = ."
QMAKE_PRO_ARGS += "DEFINES += PACKAGE=\\\"$(PACKAGE)\\\""
QMAKE_PRO_ARGS += "DEFINES += VERSION=\\\"$(VERSION)\\\""
QMAKE_PRO_ARGS += "DEFINES += $(qmake_debug_def)"
QMAKE_PRO_ARGS += "CONFIG += qt $(qmake_debug) $(qmake_warn)"
QMAKE_PRO_ARGS += "CONFIG += create_prl link_prl"
QMAKE_PRO_ARGS += "DEPENDPATH = $(srcdir)"
QMAKE_PRO_ARGS += "QMAKE_EXT_H += .hh"

# asks qmake to verify that Qt's multithreading library is available.
QMAKE_PRO_ADDITIONAL = \
  exists( $(QTDIR)/lib/*qt-mt* ) { \n\
    message( "Configuring $@ for multi-threaded Qt..." ) \n\
    CONFIG += thread \n\
  } \n\
  REQUIRES += thread

# the qmake command in project-file generation mode.
QMAKE_PRO = ${QMAKE} -project -nopwd $(QMAKE_PRO_ARGS) -o

# a function to canonicalize filenames; it only approximately satisfies
# automake filename canonicalization rules.
canonical=$(shell echo -n $(1) | tr -cs '[:alnum:]' '_')

# extra clean targets.
mostlyclean-local:
	rm -f *.pro *.libpro *.qmakefile *.qlibmakefile
	rm -f moc*.cpp


#
# the following two defines are kludges to permit parallel builds.
# use in a makefile as follows:
#
#	application_target: FORCE; $(QBUILDAPP)
#
#	library_target: FORCE; $(QBUILDLIB)
#

define QBUILDAPP
	@newer $@.pro Makefile || ( \
		echo "--- generating project file $@.pro..." \
		; \
		$(QMAKE_PRO) $@.pro -t app \
		"CONFIG += $($(call canonical, $@)_configs)" \
		"DEFINES += $($(call canonical, $@)_defines)" \
		"TARGET = $@" \
		"HEADERS = $($(call canonical, $@)_headers:%=$(srcdir)/%)" \
		"INCLUDEPATH += $($(call canonical, $@)_includepath:%=$(srcdir)/%)" \
		"FORMS = $($(call canonical, $@)_forms:%=$(srcdir)/%)" \
		"LEXSOURCES = $($(call canonical, $@)_lexsources:%=$(srcdir)/%)" \
		"LIBS += $($(call canonical, $@)_LDADD)" \
		"TARGETDEPS += $($(call canonical, $@)_LDADD)" \
		"REQUIRES = $($(call canonical, $@)_requires)" \
		"SOURCES = $($(call canonical, $@)_SOURCES:%=$(srcdir)/%)" \
		"VERSION = $($(call canonical, $@)_version)" \
		"YACCSOURCES = $($(call canonical, $@)_yaccsources:%=$(srcdir)/%)" \
		; \
		echo >> $@.pro; \
		echo -e "$(QMAKE_PRO_ADDITIONAL)" >> $@.pro; \
		echo -e "$($(call canonical, $@)_additional)" >> $@.pro \
		; \
		echo "--- generating makefile $@.qmakefile..."; \
		$(QMAKE) -o $@.qmakefile $@.pro -t app \
	)

	+$(MAKE) -f $@.qmakefile
endef

define QBUILDLIB
	@newer $@.libpro Makefile || ( \
		echo "--- generating project file $@.libpro..." \
		; \
		$(QMAKE_PRO) $@.libpro -t lib \
		"CONFIG += $($(call canonical, $@)_configs)" \
		"DEFINES += $($(call canonical, $@)_defines)" \
		"TARGET = $(@:lib%.a=%)" \
		"HEADERS = $($(call canonical, $@)_headers:%=$(srcdir)/%)" \
		"INCLUDEPATH += $($(call canonical, $@)_includepath:%=$(srcdir)/%)" \
		"FORMS = $($(call canonical, $@)_forms:%=$(srcdir)/%)" \
		"LEXSOURCES = $($(call canonical, $@)_lexsources:%=$(srcdir)/%)" \
		"LIBS += $($(call canonical, $@)_LDADD)" \
		"REQUIRES = $($(call canonical, $@)_requires)" \
		"SOURCES = $($(call canonical, $@)_SOURCES:%=$(srcdir)/%)" \
		"VERSION = $($(call canonical, $@)_version)" \
		"YACCSOURCES = $($(call canonical, $@)_yaccsources:%=$(srcdir)/%)" \
		; \
		echo >> $@.libpro; \
		echo -e "$(QMAKE_PRO_ADDITIONAL)" >> $@.libpro; \
		echo -e "$($(call canonical, $@)_additional)" >> $@.libpro \
		; \
		echo "--- generating library makefile $@.qlibmakefile..."; \
		$(QMAKE) -o $@.qlibmakefile $@.libpro -t lib \
	)

	+$(MAKE) -f $@.qlibmakefile
endef

.PHONY: FORCE


SUBDIRS_PRIMITIVE = tools cpu event main support

#select the correct subdirs depending on whether we build the viewer or not
if VIEWER
SUBDIRS_VIEWERS = $(SUBDIRS_PRIMITIVE) viewers
else
SUBDIRS_VIEWERS = $(SUBDIRS_PRIMITIVE)
endif

if QT_VIEWER
SUBDIRS = $(SUBDIRS_VIEWERS) qt-viewer
else
SUBDIRS = $(SUBDIRS_VIEWERS)
endif

EXTRA_DIST = defs.hh




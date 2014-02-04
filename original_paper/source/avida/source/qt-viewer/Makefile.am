include $(top_srcdir)/qmakeMakefile.inc

CPPFLAGS = @CPPFLAGS@

AVIDA_SOURCE_DIR = ..

AVIDA_CPU_DIR = $(AVIDA_SOURCE_DIR)/cpu
AVIDA_DRIVER_DIR = ./
AVIDA_EVENT_DIR = $(AVIDA_SOURCE_DIR)/event
AVIDA_MAIN_DIR = $(AVIDA_SOURCE_DIR)/main
AVIDA_TOOLS_DIR = $(AVIDA_SOURCE_DIR)/tools

QT_VIEWER_SOURCE_DIR = .

CELLVIEWWIDGET_DIR = $(QT_VIEWER_SOURCE_DIR)
EVENT_VIEW_WIDGET_DIR = $(QT_VIEWER_SOURCE_DIR)
GODBOX_WIDGET_DIR = $(QT_VIEWER_SOURCE_DIR)
INSTR_VIEW_WIDGET_DIR = $(QT_VIEWER_SOURCE_DIR)
SETUP_WIZARD_DIR = $(QT_VIEWER_SOURCE_DIR)
WIDGETLIST_DIR = $(QT_VIEWER_SOURCE_DIR)

INCLUDEPATH  = $(AVIDA_MAIN_DIR)
INCLUDEPATH += $(AVIDA_DRIVER_DIR)
INCLUDEPATH += $(CELLVIEWWIDGET_DIR)
INCLUDEPATH += $(AVIDA_CPU_DIR)
INCLUDEPATH += $(WIDGETLIST_DIR)
INCLUDEPATH += $(AVIDA_SOURCE_DIR)
INCLUDEPATH += $(INSTR_VIEW_WIDGET_DIR)
INCLUDEPATH += $(EVENT_VIEW_WIDGET_DIR)
# Note:  INCLUDEPATH is not the same as INCLUDES;
# INCLUDEPATH is a convenience variable that means nothing to automake.


noinst_LIBRARIES = libwidgetlist.a
libwidgetlist.a: FORCE; $(QBUILDLIB) # forces handling by qmakeMakefile.
libwidgetlist_a_configs = staticlib
libwidgetlist_a_SOURCES = \
	avd_hide_show_button.cc \
	avd_widget_list.cc \
	avd_widget_list_container.cc \
	avd_widget_list_item.cc \
	avd_widget_list_item_container.cc \
	code_widget.cc \
  gui_config.cc \
	labeled_field_convenience.cc \
  messaging_population_wrapper.cc \
  population_cell_wrapper.cc
noinst_HEADERS = $(libwidgetlist_a_headers)
libwidgetlist_a_headers = $(libwidgetlist_a_SOURCES:%.cc=%.hh)
libwidgetlist_a_includepath = \
  $(INCLUDEPATH) \
  $(AVIDA_TOOLS_DIR) 


noinst_LIBRARIES += libcellview.a
libcellview.a: FORCE; $(QBUILDLIB)
libcellview_a_configs = staticlib
libcellview_a_SOURCES = \
	avd_cell_view_list_item.cc \
	population_cell_wrapper.cc \
	population_cell_wrapped_accessors.cc \
	cell_view_widget.cc \
		population_cell_list_item.cc \
			organism_list_item.cc \
				hardware_list_item.cc \
				phenotype_list_item.cc \
					phenotype_last_divide_list_item.cc \
					last_progress_list_item.cc \
					lifetime_list_item.cc \
					status_flags_list_item.cc \
					child_info_list_item.cc \
		mutation_rates_list_item.cc \
		inputs_list_item.cc
noinst_HEADERS += $(libcellview_a_headers)
libcellview_a_headers = $(libcellview_a_SOURCES:%.cc=%.hh)
libcellview_a_includepath = \
  $(INCLUDEPATH) \
  $(AVIDA_TOOLS_DIR)


noinst_LIBRARIES += libinstview.a
libinstview.a: FORCE; $(QBUILDLIB)
libinstview_a_configs = staticlib
libinstview_a_SOURCES = \
  n_orig_instruction_field_widget.cc \
  n_orig_instruction_line_widget.cc \
	n_orig_instruction_scrollview.cc \
  n_orig_instruction_scrollview_config.cc \
  n_orig_instruction_pixmapstructs.cc \
	n_orig_instruction_view_widget.cc \
	n_orig_instruction_buttons_widget.cc \
	n_orig_instruction_cpu_widget.cc \
  n_orig_instruction_viewer.cc
noinst_HEADERS += $(libinstview_a_headers)
libinstview_a_headers = $(libinstview_a_SOURCES:%.cc=%.hh)
libinstview_a_includepath = \
  $(AVIDA_CPU_DIR) \
  $(AVIDA_MAIN_DIR) \
  $(AVIDA_SOURCE_DIR) \
  $(AVIDA_TOOLS_DIR) \
  $(INSTR_VIEW_WIDGET_DIR)



noinst_LIBRARIES += libeventview.a
libeventview.a: FORCE; $(QBUILDLIB)
libeventview_a_configs = staticlib
libeventview_a_SOURCES = \
  event_view_widget.cc
libeventview_a_forms = event_list_editor.ui
EXTRA_DIST = $(libeventview_a_forms)
CLEANFILES = $(libeventview_a_forms:%.ui=%.cpp) $(libeventview_a_forms:%.ui=%.hh)
noinst_HEADERS += $(libeventview_a_headers)
libeventview_a_headers = $(libeventview_a_SOURCES:%.cc=%.hh)
libeventview_a_includepath = $(INCLUDEPATH)



#noinst_LIBRARIES += libgodbox.a
#libgodbox.a: FORCE; $(QBUILDLIB)
#libgodbox_a_configs = staticlib
#libgodbox_a_SOURCES = \
#  godbox_widget.cc \
#  avd_rate_slider.cc
#libgodbox_a_forms = godbox.ui
#EXTRA_DIST += $(libgodbox_a_forms)
#CLEANFILES += $(libgodbox_a_forms:%.ui=%.cpp) $(libgodbox_a_forms:%.ui=%.h)
#noinst_HEADERS += $(libgodbox_a_headers)
#libgodbox_a_headers = $(libgodbox_a_SOURCES:%.cc=%.hh)
#libgodbox_a_includepath = $(INCLUDEPATH) $(AVIDA_EVENT_DIR) $(AVIDA_TOOLS_DIR)



noinst_LIBRARIES += libgodbox2.a
libgodbox2.a: FORCE; $(QBUILDLIB)
libgodbox2_a_configs = staticlib
libgodbox2_a_SOURCES = \
  apocalypse_widget.cc \
  avd_rate_validator2.cc \
  avd_lograte_widget.cc \
  avd_rate_slider2.cc \
  godbox2.cc \
  godbox_widget2.cc \
  repopulate_widget.cc
noinst_HEADERS += $(libgodbox2_a_headers)
libgodbox2_a_headers = $(libgodbox2_a_SOURCES:%.cc=%.hh)
libgodbox2_a_includepath = $(INCLUDEPATH) $(AVIDA_EVENT_DIR) $(AVIDA_TOOLS_DIR)


# noinst_LIBRARIES += libtestwizard.a
# libtestwizard.a: FORCE; $(QBUILDLIB)
# libtestwizard_a_configs = staticlib
# libtestwizard_a_SOURCES = \
# libtestwizard_a_forms = wizard.ui
# EXTRA_DIST += $(libtestwizard_a_forms)
# CLEANFILES += $(libtestwizard_a_forms:%.ui=%.cpp) $(libtestwizard_a_forms:%.ui=%.hh)
# noinst_HEADERS += $(libtestwizard_a_headers)
# libtestwizard_a_headers = \
#   testwizard.hh
# libtestwizard_a_includepath = \
#   $(AVIDA_TOOLS_DIR) \
#   $(AVIDA_MAIN_DIR)



noinst_LIBRARIES += libsetupwizard.a
libsetupwizard.a: FORCE; $(QBUILDLIB)
libsetupwizard_a_configs = staticlib
libsetupwizard_a_SOURCES = \
  testwizard.cc \
  testwizard_loadgen.cc \
  testwizard_validators.cc \
  setup_file.cc \
  setup_genesis.cc \
  setup_wizard.cc \
  setup_wizard2.cc \
  setup_wizard_config.cc \
  setup_wizard_constants.cc \
  setup_wizard_page.cc \
  setup_wizard_page2.cc
libsetupwizard_a_forms = wizard.ui
EXTRA_DIST += $(libsetupwizard_a_forms)
noinst_HEADERS += $(libsetupwizard_a_headers)
libsetupwizard_a_headers = \
  setup_file.hh \
  setup_genesis.hh \
  setup_wizard.hh \
  setup_wizard2.hh \
  setup_wizard_config.hh \
  setup_wizard_constants.hh \
  setup_wizard_page.hh \
  setup_wizard_page2.hh \
  testwizard.hh
libsetupwizard_a_includepath = \
  $(AVIDA_TOOLS_DIR) \
  $(AVIDA_MAIN_DIR)


noinst_LIBRARIES += libavidadriver.a
libavidadriver.a: FORCE; $(QBUILDLIB)
libavidadriver_a_configs = staticlib
libavidadriver_a_SOURCES = \
	avida_driver.cc \
  avida_driver_messages.cc \
  avida_thread_driver.cc \
  gui_thread_driver.cc
noinst_HEADERS += $(libavidadriver_a_headers)
libavidadriver_a_headers = avida_driver.hh
libavidadriver_a_includepath = \
  $(INCLUDEPATH) \
  $(AVIDA_TOOLS_DIR)



bin_PROGRAMS = qt_viewer
qt_viewer$(EXEEXT): FORCE; $(QBUILDAPP)
qt_viewer_SOURCES = \
	qt-viewer.cc \
	avida_data.cc \
	avida_plot_widget.cc \
	color_scale_widget.cc \
	creature_view.cc \
	double_slider.cc \
  gui_message_display.cc \
	init_file_view.cc \
	main_window.cc \
	map_view.cc \
	map_view_widget.cc \
	map_view_cell_entry.cc \
	plot_select_dia_impl.cc \
	plot_widget.cc \
  setup_avida.cc
qt_viewer_forms = plot_select_dia.ui
EXTRA_DIST += $(qt_viewer_forms)
CLEANFILES += $(qt_viewer_forms:%.ui=%.cpp) $(qt_viewer_forms:%.ui=%.hh)
noinst_HEADERS += $(qt_viewer_headers)
qt_viewer_headers = \
	avida_data.hh \
	avida_driver.hh \
	avida_plot_widget.hh \
	color_scale_widget.hh \
	creature_view.hh \
	double_slider.hh \
	init_file_view.hh \
  n_orig_instruction_view_widget.hh \
	main_window.hh \
	map_view.hh \
	map_view_widget.hh \
	map_view_cell_entry.hh \
  messaging_population_wrapper.hh \
	plot_select_dia_impl.hh \
	plot_widget.hh \
  population_wrapper.hh
qt_viewer_includepath = \
  $(INCLUDEPATH) \
  $(AVIDA_TOOLS_DIR)
qt_viewer_LDADD = ../main/libmain.a \
		../cpu/libcpu.a \
		../event/libevent.a \
		../main/libmain.a \
		../tools/libtools.a \
  $(CELLVIEWWIDGET_DIR)/libcellview.a \
  $(WIDGETLIST_DIR)/libwidgetlist.a \
  $(INSTR_VIEW_WIDGET_DIR)/libinstview.a \
  $(EVENT_VIEW_WIDGET_DIR)/libeventview.a \
  $(GODBOX_WIDGET_DIR)/libgodbox2.a \
  $(SETUP_WIZARD_DIR)/libsetupwizard.a \
  $(AVIDA_TOOLS_DIR)/libtools.a \
  $(AVIDA_DRIVER_DIR)/libavidadriver.a
#
# below is a workaround for following bug in qt-mac-commercial-3.0.4:
#
# the Darwin link-editor loads files and libraries in the exact order
# given on the command line, and, in the case if static libraries,
# examines them once, looking for symbols that are undefined *at the
# moment the the library is loaded*.  If it doesn't find any currently
# undefined symbols in the library, it forgets all about the library.
#
# so link commands like the following, that work under linux:
#
#   cc -Lsome/path -Lother/path -lfoo -lbar... -o program file1.o...
#
# must be rewritten for MacOSX/Darwin as:
#
#   cc -o program file1.o... -Lsome/path -Lother/path -lfoo -lbar...
#
# qt-mac-commercial-3.0.4 generates the former link order.  The
# following places -lqt-mt into the link command a second time, where
# the Darwin link-editor can use it.
#
qt_viewer_additional = \
  macx:LIBS += -lqt-mt

# test code
EXTRA_PROGRAMS = \
	test_cpu_widget \
	test_plot_widget \
	test_double_slider \
	test_color_scale_widget \
	test_plot_select_dia_impl

bin_PROGRAMS += avd_message_queue_t
avd_message_queue_t$(EXEEXT): FORCE; $(QBUILDAPP)
avd_message_queue_t_SOURCES = \
	avd_message_queue_t.cc
noinst_HEADERS += $(avd_message_queue_t_headers)
avd_message_queue_t_headers = \
	avd_message_queue.hh \
	avd_message_queue_t.hh
avd_message_queue_t_includepath = \
  $(AVIDA_TOOLS_DIR)
avd_message_queue_t_LDADD = \
  $(AVIDA_TOOLS_DIR)/libtools.a

# EXTRA_PROGRAMS += test_widget_list
# test_widget_list: FORCE; $(QBUILDAPP)
# test_widget_list_SOURCES = \
# 	test_list.cc \
# 	test_list2.cc \
# 	test_list_item.cc \
# 	test_list3.cc \
# 	test_list_subitem.cc \
# 	test_list_t.cc
# noinst_HEADERS += $(test_widget_list_headers)
# test_widget_list_headers = \
# 	test_list.hh \
# 	test_list2.hh \
# 	test_list_item.hh \
# 	test_list3.hh \
# 	test_list_subitem.hh
# test_widget_list_LDADD = libwidgetlist.a
# 
# 
# 
# EXTRA_PROGRAMS += test_cell_view
# test_cell_view: FORCE; $(QBUILDAPP)
# test_cell_view_SOURCES = test_cell_view_t.cc
# noinst_HEADERS += $(test_cell_view_headers)
# test_cell_view_headers = cell_view_widget.hh
# test_cell_view_includepath = $(INCLUDEPATH)
# #
# # note link order problems: libmain.a must be present twice in the
# # following list of libraries.
# #
# # good enough for now, tho.
# #
# test_cell_view_LDADD = \
# 	libcellview.a \
# 	libwidgetlist.a \
# 	$(AVIDA_MAIN_DIR)/libmain.a \
# 	$(AVIDA_CPU_DIR)/libcpu.a \
# 	$(AVIDA_EVENT_DIR)/libevent.a \
# 	$(AVIDA_MAIN_DIR)/libmain.a \
# 	$(AVIDA_TOOLS_DIR)/libtools.a


test_color_scale_widget$(EXEEXT): FORCE; $(QBUILDAPP)
test_color_scale_widget_SOURCES = color_scale_widget.cc \
         color_scale_widget_t.cc
test_color_scale_widget_headers = color_scale_widget.hh


test_cpu_widget$(EXEEXT): FORCE; $(QBUILDAPP)
test_cpu_widget_SOURCES = cpu-widget_t.cc
test_cpu_widget_LDADD = ../main/libmain.a \
		../cpu/libcpu.a \
		../event/libevent.a \
		../main/libmain.a \
		../tools/libtools.a


test_plot_select_dia_impl$(EXEEXT): FORCE; $(QBUILDAPP)
test_plot_select_dia_impl_SOURCES = \
	avida_data.cc \
	plot_select_dia_impl.cc \
	plot_select_dia_impl_t.cc
test_plot_select_dia_impl_headers = plot_select_dia_impl.hh
test_plot_select_dia_impl_forms = plot_select_dia.ui
# EXTRA_DIST += $(test_plot_select_dia_impl_forms)


test_plot_widget$(EXEEXT): FORCE; $(QBUILDAPP)
test_plot_widget_SOURCES = plot_widget_t.cc plot_widget.cc double_slider.cc
test_plot_widget_headers = plot_widget.hh double_slider.hh
test_plot_widget_LDADD = ../tools/libtools.a



test_double_slider$(EXEEXT): double_slider_t.moc FORCE; $(QBUILDAPP)
test_double_slider_SOURCES = double_slider_t.cc double_slider.cc
test_double_slider_headers = double_slider.hh


CLEANFILES += double_slider_t.moc
double_slider_t.moc: double_slider_t.cc
	$(QTDIR)/bin/moc -o $@ $<

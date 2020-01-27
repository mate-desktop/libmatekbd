NULL =
lib_LTLIBRARIES = libmatekbd.la libmatekbdui.la

common_CFLAGS =					\
	-I$(top_srcdir)				\
	$(GDK_CFLAGS)				\
	$(GIO_CFLAGS)				\
	$(LIBXKLAVIER_CFLAGS)			\
	$(XLIB_CFLAGS)				\
	-I$(top_srcdir)/intl			\
	-DG_LOG_DOMAIN=\"MateKbdIndicator\"	\
	-DDATADIR=\"$(datadir)\"		\
	-DSYSCONFDIR=\"$(sysconfdir)\"		\
	-DLIBDIR=\"$(libdir)\"			\
	$(NULL)

common_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

common_LIBADD =					\
	$(GDK_LIBS)				\
	$(GIO_LIBS)				\
	$(LIBXKLAVIER_LIBS)			\
	$(XLIB_LIBS)				\
	$(NULL)

libmatekbd_la_SOURCES =				\
	matekbd-desktop-config.c		\
	matekbd-keyboard-config.c		\
	matekbd-util.c				\
	$(NULL)
libmatekbd_la_CFLAGS =				\
	$(common_CFLAGS)			\
	$(NULL)
libmatekbd_la_LDFLAGS =				\
	$(common_LDFLAGS)			\
	$(NULL)
libmatekbd_la_LIBADD =				\
	$(common_LIBADD)			\
	$(NULL)

libmatekbdui_la_SOURCES =			\
	matekbd-indicator-config.c		\
	matekbd-indicator.c			\
	matekbd-status.c			\
	matekbd-indicator-marshal.c		\
	matekbd-keyboard-drawing-marshal.c	\
	matekbd-keyboard-drawing-resources.c	\
	matekbd-keyboard-drawing.c		\
	$(NULL)
libmatekbdui_la_CFLAGS =			\
	$(common_CFLAGS)			\
	$(GTK_CFLAGS)				\
	$(NULL)
libmatekbdui_la_LDFLAGS =			\
	$(common_LDFLAGS)			\
	$(NULL)
libmatekbdui_la_LIBADD = 			\
	libmatekbd.la				\
	$(common_LIBADD)			\
	$(GLIB_LIBS)				\
	$(GMODULE_LIBS)				\
	$(GTK_LIBS)				\
	$(NULL)

BUILT_SOURCES =					\
	matekbd-indicator-marshal.c		\
	matekbd-indicator-marshal.h		\
	matekbd-keyboard-drawing-marshal.c	\
	matekbd-keyboard-drawing-marshal.h	\
	matekbd-keyboard-drawing-resources.c	\
	matekbd-keyboard-drawing-resources.h	\
	$(NULL)

CLEANFILES = $(BUILT_SOURCES)

matekbdincdir = $(includedir)/libmatekbd
matekbdinc_HEADERS =				\
	matekbd-desktop-config.h		\
	matekbd-keyboard-config.h		\
	matekbd-indicator.h			\
	matekbd-status.h			\
	matekbd-indicator-config.h		\
	matekbd-keyboard-drawing.h		\
	matekbd-util.h				\
	$(NULL)

noinst_HEADERS =				\
	$(extra_nih)				\
	matekbd-config-private.h		\
	$(NULL)

gsettingsschema_in_files = org.mate.peripherals-keyboard-xkb.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

EXTRA_DIST =					\
	matekbd-indicator-marshal.list		\
	matekbd-keyboard-drawing-marshal.list	\
	$(gsettingsschema_in_files)		\
	org.mate.peripherals.keyboard.gresource.xml \
	show-layout.ui				\
	$(NULL)

DISTCLEANFILES = $(gsettings_SCHEMAS)

GLIB_GENMARSHAL = $(shell pkg-config --variable=glib_genmarshal glib-2.0)

matekbd-indicator-marshal.h: matekbd-indicator-marshal.list
	$(AM_V_GEN)$(GLIB_GENMARSHAL) --prefix=matekbd_indicator $(srcdir)/matekbd-indicator-marshal.list --header > $@

matekbd-indicator-marshal.c: matekbd-indicator-marshal.h
	$(AM_V_GEN)$(GLIB_GENMARSHAL) --prefix=matekbd_indicator $(srcdir)/matekbd-indicator-marshal.list --body > $@

matekbd-keyboard-drawing-marshal.h: matekbd-keyboard-drawing-marshal.list
	$(AM_V_GEN)( $(GLIB_GENMARSHAL) --prefix=matekbd_keyboard_drawing $(srcdir)/matekbd-keyboard-drawing-marshal.list \
                            --header > matekbd-keyboard-drawing-marshal.tmp \
          && mv matekbd-keyboard-drawing-marshal.tmp matekbd-keyboard-drawing-marshal.h ) \
        || ( rm -f matekbd-keyboard-drawing-marshal.tmp && exit 1 )

matekbd-keyboard-drawing-marshal.c: matekbd-keyboard-drawing-marshal.h
	$(AM_V_GEN)( $(GLIB_GENMARSHAL) --prefix=matekbd_keyboard_drawing $(srcdir)/matekbd-keyboard-drawing-marshal.list \
                            --body > matekbd-keyboard-drawing-marshal.tmp \
          && mv matekbd-keyboard-drawing-marshal.tmp matekbd-keyboard-drawing-marshal.c ) \
        || ( rm -f matekbd-keyboard-drawing-marshal.tmp && exit 1 )


matekbd-keyboard-drawing-resources.h matekbd-keyboard-drawing-resources.c: org.mate.peripherals.keyboard.gresource.xml Makefile $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir) $(srcdir)/org.mate.peripherals.keyboard.gresource.xml)
	$(AM_V_GEN) XMLLINT=$(XMLLINT) $(GLIB_COMPILE_RESOURCES) --target $@ --sourcedir $(srcdir) --generate --c-name matekbd $<

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Matekbd-1.0.gir
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = $(matekbdinc_HEADERS) \
        $(libmatekbd_la_SOURCES) \
        $(libmatekbdui_la_SOURCES)

Matekbd-1.0.gir: libmatekbd.la libmatekbdui.la
Matekbd_1_0_gir_INCLUDES = GObject-2.0 Gtk-3.0 xlib-2.0 Xkl-1.0
Matekbd_1_0_gir_CFLAGS = $(libmatekbdui_la_CFLAGS)
Matekbd_1_0_gir_LIBS = libmatekbd.la libmatekbdui.la
Matekbd_1_0_gir_FILES = $(introspection_sources)

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

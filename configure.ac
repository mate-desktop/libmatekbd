dnl ***************************************************************************
dnl *** configure.in for libmatekbd                                        ***
dnl ***************************************************************************
m4_define([libmatekbd_major_version], [1])
m4_define([libmatekbd_minor_version], [24])
m4_define([libmatekbd_micro_version], [1])
m4_define([libmatekbd_version],[libmatekbd_major_version.libmatekbd_minor_version.libmatekbd_micro_version])

AC_INIT([libmatekbd], [libmatekbd_version], [https://github.com/mate-desktop/libmatekbd])
AC_PREREQ(2.59)

VERSION_INFO=6:0:2
AC_SUBST(VERSION_INFO)

AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.9 no-dist-gzip dist-xz check-news])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR(m4)

GLIB_REQUIRED=2.50.0
GIO_REQUIRED=2.25.0
GTK_REQUIRED=3.22.0
GDK_REQUIRED=3.22.0
LIBXKLAVIER_REQUIRED=5.2

AC_SUBST([GIO_REQUIRED])
AC_SUBST([GDK_REQUIRED])
AC_SUBST([LIBXKLAVIER_REQUIRED])

MATE_COMMON_INIT
MATE_COMPILE_WARNINGS

AC_PROG_CC
AC_STDC_HEADERS
AM_PROG_LIBTOOL

# ****
# i18n
# ****

GETTEXT_PACKAGE=libmatekbd
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])

AM_GNU_GETTEXT_VERSION([0.19.8])
AM_GNU_GETTEXT([external])

# **********************
# GLIB_COMPILE_RESOURCES
# **********************

AC_ARG_VAR([GLIB_COMPILE_RESOURCES],[glib-compile-resources bin])
AC_PATH_PROG([GLIB_COMPILE_RESOURCES],[glib-compile-resources],[])
if test -z "$GLIB_COMPILE_RESOURCES"; then
  AC_MSG_ERROR([glib-compile-resources not found])
fi

# *******
# XMLLINT
# *******

AC_ARG_VAR([XMLLINT],[xmllint bin])
AC_PATH_PROG([XMLLINT],[xmllint],[])
if test -z "$XMLLINT"; then
  AC_MSG_ERROR([xmllint not found])
fi

# *****************************************************************************

AC_CHECK_FUNCS(setlocale)

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED)
PKG_CHECK_MODULES(GMODULE, gmodule-2.0, [GMODULE_ADD="gmodule-2.0"],[GMODULE_ADD=""])

AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(GMODULE_LIBS)

PKG_CHECK_MODULES(GIO, gio-2.0 >= $GIO_REQUIRED)

AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

PKG_CHECK_MODULES(GDK, gdk-3.0 >= $GDK_REQUIRED)

AC_SUBST(GDK_CFLAGS)
AC_SUBST(GDK_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-3.0 >= $GTK_REQUIRED gdk-x11-3.0 >= $GDK_REQUIRED)

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(LIBXKLAVIER, libxklavier >= $LIBXKLAVIER_REQUIRED)

AC_SUBST(LIBXKLAVIER_CFLAGS)
AC_SUBST(LIBXKLAVIER_LIBS)

AC_PATH_XTRA
XLIB_CFLAGS="$X_CFLAGS"
XLIB_LIBS="$X_LIBS -lX11 $X_EXTRA_LIBS"
AC_SUBST(XLIB_CFLAGS)
AC_SUBST(XLIB_LIBS)

GLIB_GSETTINGS

AC_ARG_ENABLE([tests],
              AC_HELP_STRING([--disable-tests], [Dont build the test apps]),,
              [enable_tests=yes])

AM_CONDITIONAL(BUILD_TESTS, test "$enable_tests" = "yes")

AC_SEARCH_LIBS([sqrt], [m])

GOBJECT_INTROSPECTION_CHECK([0.6.7])

AC_OUTPUT([
Makefile
libmatekbd.pc
libmatekbdui.pc
po/Makefile.in
libmatekbd/Makefile
libmatekbd/org.mate.peripherals-keyboard-xkb.gschema.xml
test/Makefile
])

AC_OUTPUT

dnl ---------------------------------------------------------------------------
dnl - Show summary
dnl ---------------------------------------------------------------------------

echo "
Configuration:

    prefix:                       ${prefix}
    exec_prefix:                  ${exec_prefix}
    libdir:                       ${libdir}
    bindir:                       ${bindir}
    sbindir:                      ${sbindir}
    sysconfdir:                   ${sysconfdir}
    datadir:                      ${datadir}
    source code location:         ${srcdir}
    compiler:                     ${CC}
    cflags:                       ${CFLAGS}
    Warning flags:                ${WARN_CFLAGS}
"

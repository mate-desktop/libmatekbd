dnl ***************************************************************************
dnl *** configure.in for libmatekbd                                        ***
dnl ***************************************************************************
m4_define([libmatekbd_major_version], [1])
m4_define([libmatekbd_minor_version], [7])
m4_define([libmatekbd_micro_version], [1])
m4_define([libmatekbd_version],[libmatekbd_major_version.libmatekbd_minor_version.libmatekbd_micro_version])

AC_INIT([libmatekbd], [libmatekbd_version], [https://github.com/mate-desktop/libmatekbd])
AC_PREREQ(2.59)

VERSION_INFO=6:0:2
AC_SUBST(VERSION_INFO)

AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.9 no-dist-gzip dist-xz])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR(m4)

dnl ----------------------------------------------------------------------------
dnl GTK library version
dnl ----------------------------------------------------------------------------
GTK_API_VERSION=2.0
GTK_REQUIRED=2.18

AC_MSG_CHECKING([which gtk+ version to compile against])
AC_ARG_WITH([gtk],
  [AS_HELP_STRING([--with-gtk=2.0|3.0],[which gtk+ version to compile against (default: 2.0)])],
  [case "$with_gtk" in
     2.0|3.0) ;;
     *) AC_MSG_ERROR([invalid gtk version specified]) ;;
   esac],
  [with_gtk=2.0])
AC_MSG_RESULT([$with_gtk])

case "$with_gtk" in
  2.0) GTK_API_VERSION=2.0
       GTK_REQUIRED=2.18
       GDK_REQUIRED=2.18
       ;;
  3.0) GTK_API_VERSION=3.0
       GTK_REQUIRED=3.0.0
       GDK_REQUIRED=3.0
       ;;
esac

dnl ***************************************************************************
dnl *** Minimum library versions for libmatekbd                            ***
dnl ***************************************************************************
GLIB_REQUIRED=2.18
GIO_REQUIRED=2.25.0

LIBXKLAVIER_REQUIRED=5.0

AC_SUBST([GIO_REQUIRED])
AC_SUBST([GTK_API_VERSION])
AC_SUBST([GTK_REQUIRED])
AC_SUBST([GDK_REQUIRED])
AC_SUBST([LIBXKLAVIER_REQUIRED])

dnl ***************************************************************************

MATE_COMMON_INIT
MATE_COMPILE_WARNINGS

AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AM_PROG_LIBTOOL

# ****
# i18n
# ****

GETTEXT_PACKAGE=libmatekbd
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])

AM_GLIB_GNU_GETTEXT

IT_PROG_INTLTOOL([0.35.0])

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

PKG_CHECK_MODULES(GDK, gdk-$GTK_API_VERSION >= $GDK_REQUIRED)

AC_SUBST(GDK_CFLAGS)
AC_SUBST(GDK_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-$GTK_API_VERSION >= $GTK_REQUIRED gdk-x11-$GTK_API_VERSION >= $GDK_REQUIRED)

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

AC_OUTPUT([
Makefile
libmatekbd.pc
libmatekbdui.pc
po/Makefile.in
libmatekbd/Makefile
test/Makefile
])

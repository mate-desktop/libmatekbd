if BUILD_TESTS
TEST = test
endif

SUBDIRS = po libmatekbd $(TEST)

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

DISTCHECK_CONFIGURE_FLAGS = \
	--enable-introspection \
	--enable-compile-warnings=no


EXTRA_DIST = \
        autogen.sh \
        libmatekbd.pc.in \
        libmatekbdui.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmatekbd.pc libmatekbdui.pc

# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

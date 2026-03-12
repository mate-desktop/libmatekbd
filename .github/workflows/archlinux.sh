#!/usr/bin/bash

set -eo pipefail

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Archlinux
requires=(
	ccache # Use ccache to speed up build
	clang  # Build with clang on Archlinux
	meson  # Used for meson build
)

# https://gitlab.archlinux.org/archlinux/packaging/packages/libmatekbd
requires+=(
	cairo
	dconf
	gcc
	gdk-pixbuf2
	git
	glib2
	glib2-devel
	glibc
	gobject-introspection
	gtk3
	libx11
	libxklavier
	make
	mate-common
	pango
	which
)

infobegin "Update system"
pacman --noconfirm -Syu
infoend

infobegin "Install dependency packages"
pacman --noconfirm -S ${requires[@]}
infoend

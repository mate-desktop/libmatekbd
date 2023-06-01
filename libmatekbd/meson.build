pkg_data_dir = get_option('prefix') / get_option('datadir') / 'libmatekbd'
ui_dir = pkg_data_dir / 'ui'
pkg = import('pkgconfig')

install_data(
  'show-layout.ui',
  install_dir: ui_dir
)

gsettings_cfg = configuration_data()
gsettings_cfg.set('GETTEXT_PACKAGE', gettext_domain)

configure_file(
  input: 'org.mate.peripherals-keyboard-xkb.gschema.xml.in',
  output: 'org.mate.peripherals-keyboard-xkb.gschema.xml',
  configuration: gsettings_cfg,
  install: true,
  install_dir: get_option('prefix') / get_option('datadir') / 'glib-2.0' / 'schemas'
)

gresource_files = gnome.compile_resources('matekbd-keyboard-drawing-resources', 'org.mate.peripherals.keyboard.gresource.xml', c_name : 'matekbd')

libmatekbd_deps = [
  glib_dep,
  gobject_dep,
  gio_dep,
  gdk_dep,
  gtk_dep,
  x11_dep,
  libxklavier_dep,
]

libmatekbd_sources = files(
  'matekbd-desktop-config.c',
  'matekbd-keyboard-config.c',
  'matekbd-util.c',
)

libmatekbd_headers = files(
  'matekbd-desktop-config.h',
  'matekbd-keyboard-config.h',
  'matekbd-util.h',
)

libmatekbd_lib = library(
  'matekbd',
  libmatekbd_sources,
  config_h,
  dependencies: libmatekbd_deps,
  include_directories: root_include,
  version: library_version,
  install: true,
)

pkg.generate(
  libmatekbd_lib,
  name: 'libmatekbd',
  description: 'MATE keyboard shared library',
  version: meson.project_version(),
)

libmatekbd_dep = declare_dependency(
  link_with: libmatekbd_lib,
  include_directories: root_include,
  dependencies: libmatekbd_deps,
  sources: config_h,
)

libmatekbdui_sources = files(
  'matekbd-indicator-config.c',
  'matekbd-indicator.c',
  'matekbd-status.c',
  'matekbd-keyboard-drawing.c',
)

libmatekbdui_headers = files(
  'matekbd-indicator.h',
  'matekbd-status.h',
  'matekbd-indicator-config.h',
  'matekbd-keyboard-drawing.h',
)
matekbd_indicator_marshal = gnome.genmarshal(
  'matekbd-indicator-marshal',
  prefix: 'matekbd_indicator',
  sources: 'matekbd-indicator-marshal.list',
)

matekbd_keyboard_drawing_marshal = gnome.genmarshal(
  'matekbd-keyboard-drawing-marshal',
  prefix: 'matekbd_keyboard_drawing',
  sources: 'matekbd-keyboard-drawing-marshal.list',
)


libmatekbdui_deps = [
  libmatekbd_dep,
  m_dep,
]

libmatekbdui_lib = library(
  'matekbdui',
  libmatekbdui_sources,
  gresource_files,
  config_h,
  matekbd_indicator_marshal,
  matekbd_keyboard_drawing_marshal,
  dependencies: libmatekbdui_deps,
  include_directories: root_include,
  version: library_version,
  c_args: [
    '-DUIDIR="@0@"'.format(ui_dir),
    '-DDATADIR="@0@"'.format(pkg_data_dir),
  ],
  install: true,
)

pkg.generate(
  libmatekbdui_lib,
  name: 'libmatekbdui',
  description: 'MATE keyboard shared library',
  version: meson.project_version(),
)

libmatekbdui_dep = declare_dependency(
  link_with: libmatekbdui_lib,
  include_directories: root_include,
  dependencies: libmatekbdui_deps,
  sources: [config_h, matekbd_indicator_marshal, matekbd_keyboard_drawing_marshal],
)

install_headers(
  libmatekbd_headers,
  libmatekbdui_headers,
  subdir: 'libmatekbd',
)

if get_option('gir')
  gir_dir = join_paths(get_option('prefix') / get_option('datadir'), '@0@-@1@'.format('gir', matekbd_gir_version))
  typelib_dir = join_paths(get_option('prefix') / get_option('libdir'), '@0@-@1@'.format('girepository', matekbd_gir_version))

  gnome.generate_gir(
    libmatekbd_lib, libmatekbdui_lib,
    sources: [
      libmatekbd_sources,
      libmatekbdui_sources,
      libmatekbd_headers,
      libmatekbdui_headers,
      matekbd_indicator_marshal,
      matekbd_keyboard_drawing_marshal,
    ],
    dependencies: [ libmatekbd_deps, m_dep ],
    include_directories: root_include,
    namespace: 'Matekbd',
    nsversion: '3.0',
    includes: ['GObject-2.0', 'Gtk-3.0', 'xlib-2.0', 'Xkl-1.0'],
    install: true,
    install_dir_gir: gir_dir,
    install_dir_typelib: typelib_dir,
  )
endif

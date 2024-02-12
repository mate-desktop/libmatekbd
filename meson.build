project('libmatekbd', 'c',
        version: '1.28.0',
        default_options: [
          'buildtype=debugoptimized',
          'warning_level=1',
          # We only need c99, but glib needs GNU-specific features
          # https://github.com/mesonbuild/meson/issues/2289
          'c_std=gnu99',
        ],
        meson_version : '>= 0.55',
        license: 'LGPL-2.1-or-later')

library_version = '6.0.2'
matekbd_gir_version = '1.0'
gettext_domain = 'libmatekbd'
prefix = get_option('prefix')

gnome = import('gnome')
i18n = import('i18n')

cc = meson.get_compiler('c')

glib_req        = '>= 2.45.0'
gio_req         = '>= 2.25.0'
gdk_req         = '>= 3.22.0'
gtk_req         = '>= 3.22.0'
libxklavier_req = '>= 5.2'

glib_dep = dependency('glib-2.0', version: glib_req)
gobject_dep = dependency('gobject-2.0', version: glib_req)
gio_dep = dependency('gio-2.0', version: gio_req)
gdk_dep = dependency('gdk-3.0', version: gdk_req)
gtk_dep = dependency('gtk+-3.0', version: gtk_req)
x11_dep = dependency('x11')
libxklavier_dep = dependency('libxklavier', version: libxklavier_req)
m_dep = cc.find_library('m', required : false)

add_project_arguments('-DGETTEXT_PACKAGE="@0@"'.format(gettext_domain), language: 'c')
add_project_arguments('-DHAVE_CONFIG_H=1', language: 'c')


config_cfg = configuration_data()
config_cfg.set_quoted('VERSION', meson.project_version())
config_cfg.set_quoted('MATELOCALEDIR', join_paths(prefix, get_option('localedir')))

config_h = configure_file(
  output: 'config.h',
  configuration: config_cfg,
)

root_include = include_directories('.')

subdir('libmatekbd')

subdir('po')

if get_option('tests')
  subdir('test')
endif

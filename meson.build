project(
  'Sickle Editor',
  'c',
  default_options: [
    'c_std=c23',
    'warning_level=2',
  ],
  version: '0.13.0',
  meson_version: '>=0.60.0',
  license: 'GPL-3.0-or-later',
)

add_project_arguments(
  '-DGDK_DISABLE_DEPRECATED',
  '-DGTK_DISABLE_DEPRECATED',
  language: 'c',
)

cc = meson.get_compiler('c')
libm = cc.find_library('m', required: false)

# Module imports #######################

gnome = import('gnome')

# Dependencies #########################

gtk_dep = dependency('gtk4', version: '>=4.10')
gio_dep = dependency('gio-2.0', version: '>=2.0')
rmf_dep = dependency('rmf', fallback: ['rmf', 'librmf_dep'])
epoxy_dep = dependency('epoxy')

# Configuration ########################

conf = configuration_data()
conf.set_quoted('SE_APPLICATION_ID', 'com.github.treecase.SickleEditor')
conf.set_quoted('SE_GRESOURCE_PREFIX', '/com/github/treecase/SickleEditor/')
conf.set_quoted('SE_VERSION', meson.project_version())

# Build ################################

sew_inc = include_directories('.')
serg_inc = include_directories('.')

sickle_sources = []

subdir('data')
subdir('serg')
subdir('sew')
subdir('src')

# Etc. #################################

gnome.post_install(glib_compile_schemas: true)
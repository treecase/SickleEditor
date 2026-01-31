project(
  'sew',
  'c',
  version: '0.1.0',
  default_options: [
    'c_std=c23',
    'warning_level=2',
  ],
  meson_version: '>=1.1.0',
  license: 'GPL-3.0-or-later',
)

# Imports ##############################

gnome = import('gnome')

# Config Data ##########################

conf = configuration_data()
conf.set_quoted('SEW_GRESOURCE_PREFIX', '/com/github/treecase/sew/')
conf.set_quoted('SEW_VERSION', meson.project_version())

# Include Dirs #########################

sew_inc = include_directories('.')

# Dependencies #########################

gtk_dep = dependency('gtk4', version: '>=4.0')

# Build ################################

subdir('sew')

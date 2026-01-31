project(
  'rmf',
  'c',
  version: '0.1.0',
  default_options: [
    'c_std=c23',
    'warning_level=2',
  ],
  meson_version: '>=1.1.0',
  license: 'GPL-3.0-or-later',
)

rmf_api_version = '0'
rmf_api_name = '@0@-@1@'.format(meson.project_name(), rmf_api_version)


rmf_prefix = get_option('prefix')
rmf_datadir = join_paths(rmf_prefix, get_option('datadir'))

# Imports
gnome = import('gnome')

# Include Dirs
rmf_inc = include_directories('.')

# Dependencies
gio_dep = dependency('gio-2.0', version: '>=2.0')

# Build
gir = find_program('g-ir-scanner', required: get_option('introspection'))
if not gir.found() and get_option('introspection').enabled()
  error('Introspection enabled, but g-ir-scanner not found.')
endif
build_gir = gir.found() and (
  get_option('introspection').enabled()
  or (get_option('introspection').allowed() and get_option('documentation'))
)

subdir('rmf')
subdir('docs')

# Summary

summary('Introspection', build_gir, section: 'Build')
summary('Documentation', get_option('documentation'), section: 'Build')

summary('Prefix', rmf_prefix, section: 'Directories')
summary('Datadir', rmf_datadir, section: 'Directories')

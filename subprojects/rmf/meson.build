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

rmf_inc = include_directories('.')

gio_dep = dependency('gio-2.0', version: '>=2.0')

subdir('rmf')

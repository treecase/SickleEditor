sew_cargs = [
  '-DSEW_COMPILATION',
  '-DG_LOG_DOMAIN="Sew"',
  '-DGDK_DISABLE_DEPRECATED',
  '-DGTK_DISABLE_DEPRECATED',
]

# Sources ##############################

sew_private_sources = files()

sew_public_sources = files(
  'sew-preferencesrow.c',
  'sew-viewport2d.c',
  'sew-viewport3d.c',
  'sew.c',
)

# sew_private_type_headers = files()

sew_public_headers = files(
  'sew-preferencesrow.h',
  'sew-viewport2d.h',
  'sew-viewport3d.h',
  'sew.h',
)

install_headers(sew_public_headers, subdir: 'sew')

sew_sources = sew_public_sources + sew_private_sources

# Resources ############################

sew_resources = gnome.compile_resources(
  'sew-resources',
  'resources/sew.gresource.xml',
  source_dir: 'resources',
  c_name: 'sew',
  extra_args: ['--manual-register'],
)

# Config Header ########################

sew_cdata = configuration_data()
sew_cdata.set_quoted('SEW_GRESOURCE_PREFIX', '/com/github/treecase/sew/')
sew_cdata.set_quoted('SEW_VERSION', meson.project_version())

sew_config_header = configure_file(
  configuration: sew_cdata,
  output: 'sew-config.h',
  install: true,
  install_dir: join_paths(get_option('prefix'), get_option('includedir')) / 'sew'
)

sew_sources += [
  sew_config_header,
  sew_resources,
]

# Library ##############################

sew_deps = [
  glew_dep,
  gtk_dep,
  rmf_dep,
  libserg_dep,
  libm,
]

libsew = library(
  'sew',
  sources: sew_sources,
  c_args: sew_cargs,
  include_directories: sew_inc,
  dependencies: sew_deps,
  install: true,
)

# TODO: Build gir

# Dependency ###########################

libsew_dep = declare_dependency(
  include_directories: sew_inc,
  dependencies: sew_deps,
  link_with: libsew,
)

serg_cargs = [
  '-DSERG_COMPILATION',
  '-DG_LOG_DOMAIN="Serg"',
]

# Sources ##############################

serg_private_sources = files()

serg_public_sources = files(
  'serg-program.c',
  'serg-rendergraph.c',
  'serg.c',
)

serg_private_type_headers = files()

serg_public_headers = files(
  'serg-program.h',
  'serg-rendergraph.h',
  'serg.h',
)

install_headers(serg_public_headers, subdir: 'serg')

serg_sources = serg_public_sources + serg_private_sources

# Resources ############################

serg_resources = gnome.compile_resources(
  'serg-resources',
  'resources/serg.gresource.xml',
  source_dir: 'resources',
  c_name: 'serg',
  extra_args: ['--manual-register'],
)

serg_sources += serg_resources

# Library ##############################

serg_deps = [
  epoxy_dep,
  gio_dep,
  graphene_dep,
]

libserg = library(
  'serg',
  sources: serg_sources,
  c_args: serg_cargs,
  include_directories: serg_inc,
  dependencies: serg_deps,
  install: true,
)

# TODO: Build gir

# Dependency ###########################

libserg_dep = declare_dependency(
  include_directories: serg_inc,
  dependencies: serg_deps,
  link_with: libserg,
)

rmf_cargs = [
  '-DRMF_COMPILATION',
  '-DG_LOG_DOMAIN="Rmf"',
]

# Sources

rmf_sources = files(
  'rmf-entity.c',
  'rmf-entitydata.c',
  'rmf-group.c',
  'rmf-iterator.c',
  'rmf-loader.c',
  'rmf-mapobject.c',
  'rmf-root.c',
  'rmf-solid.c',
  'rmf-structs.c',
  'rmf-types.c',
  'rmf-worldspawn.c',
)

# Library

rmf_deps = [
  gio_dep,
]

librmf = library(
  'rmf',
  sources: rmf_sources,
  c_args: rmf_cargs,
  include_directories: rmf_inc,
  dependencies: rmf_deps,
  install: true,
)

librmf_dep = declare_dependency(
  include_directories: rmf_inc,
  dependencies: rmf_deps,
  link_with: librmf,
)

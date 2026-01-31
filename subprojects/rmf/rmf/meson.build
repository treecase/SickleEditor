rmf_cargs = [
  '-DRMF_COMPILATION',
  '-DG_LOG_DOMAIN="Rmf"',
]

# Sources

# rmf_private_sources = files()

rmf_public_sources = files(
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

rmf_private_type_headers = files(
  'rmf-private.h',
)

rmf_public_headers = files(
  'rmf-entity.h',
  'rmf-entitydata.h',
  'rmf-group.h',
  'rmf-iterator.h',
  'rmf-loader.h',
  'rmf-mapobject.h',
  'rmf-root.h',
  'rmf-solid.h',
  'rmf-structs.h',
  'rmf-types.h',
  'rmf-worldspawn.h',
  'rmf.h',
)

install_headers(rmf_public_headers, subdir: 'rmf')

rmf_sources = rmf_public_sources

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

# Introspection
if build_gir
  gir_args = [
    '--quiet',
    '--doc-format=gi-docgen',
    '-DRMF_COMPILATION',
  ]
  rmf_gir = gnome.generate_gir(
    librmf,
    sources: [rmf_public_headers, rmf_public_sources],
    namespace: 'Rmf',
    nsversion: rmf_api_version,
    identifier_prefix: 'Rmf',
    symbol_prefix: 'rmf',
    # export_packages: rmf_api_name,
    includes: ['GObject-2.0', 'Gio-2.0'],
    header: 'rmf/rmf.h',
    install: true,
    extra_args: gir_args,
    fatal_warnings: false,
  )
endif

librmf_dep = declare_dependency(
  include_directories: rmf_inc,
  dependencies: rmf_deps,
  link_with: librmf,
)

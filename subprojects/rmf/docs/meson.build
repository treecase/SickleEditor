toml_conf = configuration_data()
toml_conf.set('version', meson.project_version())

gidocgen = find_program('gi-docgen', required: get_option('documentation'), native: true)
gidocgen_common_args = [
  '--quiet',
  '--no-namespace-dir',
]

docs_dir = rmf_datadir / 'doc'

if get_option('documentation') and not build_gir
  error('API reference requires introspection.')
endif

#

expand_content_md_files = []

if get_option('documentation')
  rmf_toml = configure_file(
    input: 'rmf.toml.in',
    output: 'rmf.toml',
    configuration: toml_conf,
    install: true,
    install_dir: docs_dir / 'rmf',
  )

  custom_target(
    'rmf-doc',
    input: rmf_gir[0],
    output: 'rmf',
    command: [
      gidocgen,
      'generate',
      gidocgen_common_args,
      '--config', rmf_toml,
      '--content-dir=@0@'.format(meson.current_source_dir()),
      '--add-include-path=@0@'.format(meson.current_build_dir() / '../rmf'),
      '--output-dir=@OUTPUT@',
      '@INPUT@',
    ],
    depend_files: [expand_content_md_files],
    build_by_default: true,
    install: true,
    install_dir: docs_dir,
    install_tag: 'doc',
  )
endif

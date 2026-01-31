sickle_cargs = [
  '-DG_LOG_DOMAIN="Sickle"',
]

sickle_resources = gnome.compile_resources(
  'sickle-resources',
  'resources/SickleEditor.gresource.xml',
  c_name: 'sickle',
  extra_args: ['--manual-register'],
  source_dir: 'resources',
)

config_header = configure_file(
  configuration: conf,
  output: 'config.h',
)

sickle_sources = [
  config_header,
  sickle_resources,
  files(
    'sickle-application.c',
    'sickle-applicationwindow.c',
    'sickle-main.c',
    'sickle-preferenceswindow.c',
  ),
]

sickle_deps = [
  gtk_dep,
  sew_dep,
  rmf_dep,
]

executable(
  'sickle',
  sickle_sources,
  c_args: sickle_cargs,
  dependencies: sickle_deps,
  install: true,
)

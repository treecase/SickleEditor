install_subdir(
  'icons',
  install_dir: get_option('datadir'),
)

install_subdir(
  'sickle',
  install_dir: get_option('datadir'),
)

install_data(
  'com.github.treecase.SickleEditor.desktop',
  install_dir: join_paths(get_option('datadir'), 'applications'),
)

install_data(
  'com.github.treecase.SickleEditor.gschema.xml',
  install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas'),
)
gnome.compile_schemas(
  build_by_default: true,
  depend_files: 'com.github.treecase.SickleEditor.gschema.xml',
)

if host_machine.system() == 'windows'
  subdir('windows')
endif

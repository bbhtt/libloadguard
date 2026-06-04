py = import('python').find_installation(
  'python3',
  modules: ['jsonschema', 'yaml'],
  required: false,
)

if py.found() and py.language_version().version_compare('>=3.11')
  install_data(
    'libloadguard_config_converter.py',
    install_dir: get_option('bindir'),
    rename: 'libloadguard-config-converter',
    install_mode: 'rwxr-xr-x',
  )
else
  warning(
    'libloadguard-config-converter will not be installed: '
    + 'Python >= 3.11 with jsonschema and PyYAML is required'
  )
endif

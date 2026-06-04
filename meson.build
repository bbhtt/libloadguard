project(
  'libloadguard',
  'c',
  license: 'LGPL-2.1-or-later',
  meson_version: '>=0.56.0',
  version: '25.03.1',
)

conf_data = configuration_data()
conf_data.set_quoted('LIBLOADGUARD_CONFIG', get_option('libloadguard_config'))
configure_file(output: 'config.h', configuration: conf_data)


libloadguard = shared_library('loadguard', 'libloadguard.c', install: true)

install_data(
  'libloadguard_config_converter.py',
  install_dir: get_option('bindir'),
  rename: 'libloadguard-config-converter',
  install_mode: 'rwxr-xr-x',
)


if get_option('tests')
  libloadguard_test = executable('libloadguard_test', 'libloadguard_test.c', link_with: libloadguard)
  test('libloadguard_test', libloadguard_test)
endif

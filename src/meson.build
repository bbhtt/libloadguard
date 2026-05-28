conf_data = configuration_data()
conf_data.set_quoted('LIBLOADGUARD_CONFIG', get_option('libloadguard_config'))
configure_file(output: 'config.h', configuration: conf_data)


shared_library(
  'loadguard',
  'libloadguard.c',
  install: true
)

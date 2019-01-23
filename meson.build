project('shared-library-guard', 'c', license : 'LGPL')

conf_data = configuration_data()
conf_data.set('SHARED_LIBRARY_GUARD_CONFIG', get_option('shared_library_guard_config'))
configure_file(output : 'config.h', configuration : conf_data)


shared_library('shared-library-guard', 'shared-library-guard.c',
    install : true)
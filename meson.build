project('shared-library-guard', 'c', license : 'LGPL')

conf_data = configuration_data()
conf_data.set_quoted('SHARED_LIBRARY_GUARD_CONFIG', get_option('shared_library_guard_config'))
configure_file(output : 'config.h', configuration : conf_data)


shared_library('shared-library-guard', 'shared-library-guard.c',
    install : true)
tester = executable('tester', 'tester.c')
test('empty_file', tester, args : ['tester', 'test_data/empty_file.txt'])
test('single_match', tester, args : ['tester', 'test_data/single_library.txt', '/foo/bar/baz.so'])
test('wrong_process', tester, args : ['tester2', 'test_data/single_library.txt'])
test('complex', tester, args : ['tester', 'test_data/complex.txt', '/foo/bar/baz.so', '/meep.so'])
test('absolute_path', tester, args : ['/foo/bar/tester', 'test_data/single_library.txt', '/foo/bar/baz.so'])
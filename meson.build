project(
  'shared-library-guard',
  'c',
  license : 'LGPL-2.1-or-later',
  meson_version : '>=0.56.0',
  version : '25.03.1',
)

conf_data = configuration_data()
conf_data.set_quoted('SHARED_LIBRARY_GUARD_CONFIG', get_option('shared_library_guard_config'))
configure_file(output : 'config.h', configuration : conf_data)


shared_library('shared-library-guard', 'shared-library-guard.c',
    install : true)

install_data(
  'shared-library-guard-config-converter.py',
  install_dir : get_option('bindir'),
  rename : 'shared-library-guard-config-converter',
  install_mode : 'rwxr-xr-x',
)


if get_option('tests')
  tester = executable('tester', 'tester.c')
  test('empty_file', tester, args : ['tester', join_paths(meson.project_source_root(), 'test_data/empty_file.txt')])
  test('single_match', tester, args : ['tester', join_paths(meson.project_source_root(), 'test_data/single_library.txt'), '/foo/bar/baz.so'])
  test('wrong_process', tester, args : ['tester2', join_paths(meson.project_source_root(), 'test_data/single_library.txt')])
  test('complex', tester, args : ['tester', join_paths(meson.project_source_root(), 'test_data/complex.txt'), '/foo/bar/baz.so', '/meep.so'])
  test('absolute_path', tester, args : ['/foo/bar/tester', join_paths(meson.project_source_root(), 'test_data/single_library.txt'), '/foo/bar/baz.so'])

  test_match = executable('test_match', 'test_match.c')

  test('absolute_match', test_match, args : ['true', '/some/absolute/path', '/some/absolute/path'])
  test('absolute_non_match', test_match, args : ['false', '/some/absolute/path', '/root/some/absolute/path'])

  test('relative_match', test_match, args : ['true', '*(/*)/relative/path', '/root/some/relative/path'])
  test('relative_match', test_match, args : ['false', 'relative/path', '/root/some/relative/path/not/finished'])

  test('relative_match_hidden', test_match, args : ['false', '*(/*)/relative/path', '/root/.some/relative/path'])
  test('relative_match_hidden', test_match, args : ['true', '*(/*|/.*)/relative/path', '/root/.some/relative/path'])
endif

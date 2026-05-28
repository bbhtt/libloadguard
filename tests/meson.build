inc = include_directories('..', '../src')

libloadguard_static = static_library(
  'loadguard_static',
  '../src/libloadguard.c',
  include_directories: inc,
)

test_exe = executable(
  'libloadguard_test',
  'test.c',
  link_with: libloadguard_static,
  include_directories: inc,
)

test('libloadguard', test_exe)

no_san = ['-fno-sanitize=all']

loadguard_so = shared_library(
  'loadguard',
  '../src/libloadguard.c',
  include_directories: inc,
  c_args: no_san,
  link_args: no_san,
)

dummy_lib = shared_library(
  'dummy',
  'dummy.c',
  c_args: no_san,
  link_args: no_san,
)

harness = executable(
  'harness',
  'harness.c',
  dependencies: [dependency('dl')],
  c_args: no_san,
  link_args: no_san,
)

audit_test = configure_file(
  input: 'audit_test.sh.in',
  output: 'audit_test.sh',
  configuration: {
    'LOADGUARD_SO': loadguard_so.full_path(),
    'HARNESS': harness.full_path(),
    'DUMMY_LIB': dummy_lib.full_path(),
  },
)

test(
  'audit',
  find_program('sh'),
  args: [audit_test],
  depends: [loadguard_so, dummy_lib, harness],
)

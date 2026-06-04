inc = include_directories('..', '../src')

libloadguard_static = static_library(
  'loadguard_static',
  '../src/libloadguard.c',
  include_directories: inc,
)

libloadguard_test = executable(
  'libloadguard_test',
  'libloadguard_test.c',
  link_with: libloadguard_static,
  include_directories: inc,
)

test('libloadguard_test', libloadguard_test)

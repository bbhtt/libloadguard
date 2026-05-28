project(
  'libloadguard',
  'c',
  license: 'LGPL-2.1-or-later',
  meson_version: '>=0.62.0',
  version: '26.05.0',
  default_options: 'c_std=gnu99',
)

cc = meson.get_compiler('c')

project_c_args = [
  '-Werror=empty-body',
  '-Werror=strict-prototypes',
  '-Werror=missing-prototypes',
  '-Werror=implicit-function-declaration',
  '-Werror=format=2',
  '-Werror=format-security',
  '-Werror=format-nonliteral',
  '-Werror=pointer-arith',
  '-Werror=init-self',
  '-Werror=missing-declarations',
  '-Werror=return-type',
  '-Werror=overflow',
  '-Werror=int-conversion',
  '-Werror=parentheses',
  '-Werror=incompatible-pointer-types',
  '-Werror=misleading-indentation',
  '-Werror=missing-include-dirs',
  '-D_GNU_SOURCE',
]

foreach arg: project_c_args
  if cc.has_argument(arg)
    add_project_arguments(arg, language: 'c')
  endif
endforeach

if cc.get_id() == 'clang'
  clang_args = [
    '-Wno-typedef-redefinition',
  ]
  foreach arg: clang_args
    if cc.has_argument(arg)
      add_project_arguments(arg, language: 'c')
    endif
  endforeach
endif

subdir('src')
subdir('utils')
if get_option('tests')
  subdir('tests')
endif

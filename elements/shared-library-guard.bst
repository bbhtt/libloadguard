kind: meson

build-depends:
- filename: freedesktop-sdk.bst:public-stacks/buildsystem-meson.bst

runtime-depends:
- filename: freedesktop-sdk.bst:bootstrap-import.bst
- filename: pyyaml.bst
- filename: jsonschema.bst

variables:
  meson-local: |
    -Dshared_library_guard_config=/app/etc/freedesktop.ld.so.blockedlist

config:
  build-commands:
  - "%{ninja}"
  - meson test -C _builddir
    
sources:
- kind: local
  path: shared-library-guard

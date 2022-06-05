kind: meson

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-meson.bst

runtime-depends:
- freedesktop-sdk.bst:bootstrap-import.bst
- freedesktop-sdk.bst:components/python3-pyyaml.bst
- jsonschema.bst

variables:
  meson-local: |
    -Dshared_library_guard_config=/app/etc/freedesktop.ld.so.blockedlist

config:
  build-commands:
    (>):
    - meson test -C _builddir
    
sources:
- kind: local
  path: shared-library-guard

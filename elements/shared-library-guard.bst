kind: meson

depends:
- filename: public-stacks/buildsystems.bst
  junction: freedesktop-sdk.bst

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

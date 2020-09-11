kind: pip

depends:
- freedesktop-sdk.bst:components/python3.bst
- freedesktop-sdk.bst:components/libyaml.bst

build-depends:
- freedesktop-sdk.bst:components/cython.bst
- freedesktop-sdk.bst:components/python3-pip.bst

sources:
- kind: git_tag
  url: github:yaml/pyyaml.git
  track: master
  track-tags: True
  ref: 5.1.2-0-g0f64cbfa54b0b22dc7b776b7b98a7cd657e84d78

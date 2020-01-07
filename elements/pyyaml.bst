kind: pip

depends:
- freedesktop-sdk.bst:components/python3.bst
- libyaml.bst

build-depends:
- freedesktop-sdk.bst:components/cython.bst
- freedesktop-sdk.bst:components/python3-pip.bst

sources:
- kind: git_tag
  url: github:yaml/pyyaml.git
  track: master
  track-tags: True

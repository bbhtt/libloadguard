kind: pip

depends:
- filename: freedesktop-sdk.bst:components/python3.bst
- filename: libyaml.bst

build-depends:
- filename: freedesktop-sdk.bst:components/python3-pip.bst
- filename: cython.bst

sources:
- kind: git_tag
  url: github:yaml/pyyaml.git
  track: master
  track-tags: True

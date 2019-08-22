kind: pip

depends:
- filename: freedesktop-sdk.bst:components/python3.bst

build-depends:
- filename: freedesktop-sdk.bst:components/python3-pip.bst

sources:
- kind: git_tag
  url: github:cython/cython.git
  track: release
  track-tags: True

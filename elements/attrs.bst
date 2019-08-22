kind: pip

depends:
- filename: freedesktop-sdk.bst:components/python3.bst

build-depends:
- filename: freedesktop-sdk.bst:components/python3-pip.bst
- filename: wheel.bst

sources:
- kind: git_tag
  url: github:python-attrs/attrs.git
  track: master
  track-tags: True

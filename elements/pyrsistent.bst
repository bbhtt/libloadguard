kind: pip

depends:
- filename: freedesktop-sdk.bst:components/python3.bst

build-depends:
- filename: freedesktop-sdk.bst:components/python3-pip.bst

sources:
- kind: git_tag
  url: github:tobgu/pyrsistent.git
  track: master
  track-tags: True
  ref: v0.15.4-0-gbc4a122eb16f90c95c3b80679954cec473f3afb0

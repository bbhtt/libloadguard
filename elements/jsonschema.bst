kind: pip

depends:
- filename: freedesktop-sdk.bst:components/python3.bst

build-depends:
- filename: freedesktop-sdk.bst:components/python3-pip.bst
- filename: setuptools-scm.bst
- filename: wheel.bst

runtime-depends:
- filename: attrs.bst
- filename: freedesktop-sdk.bst:components/python3-six.bst
- filename: pyrsistent.bst

sources:
- kind: git_tag
  url: github:Julian/jsonschema.git
  track: master
  track-tags: True

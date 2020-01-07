kind: pip

depends:
- freedesktop-sdk.bst:components/python3.bst

build-depends:
- freedesktop-sdk.bst:components/python3-pip.bst
- freedesktop-sdk.bst:components/python3-setuptools-scm.bst
- freedesktop-sdk.bst:components/python3-wheel.bst

runtime-depends:
- freedesktop-sdk.bst:components/python3-attrs.bst
- freedesktop-sdk.bst:components/python3-six.bst
- pyrsistent.bst

sources:
- kind: git_tag
  url: github:Julian/jsonschema.git
  track: master
  track-tags: True

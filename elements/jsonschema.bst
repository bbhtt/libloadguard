kind: pip

depends:
- freedesktop-sdk.bst:components/python3.bst

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-python.bst
- freedesktop-sdk.bst:components/python3-setuptools-scm.bst

runtime-depends:
- freedesktop-sdk.bst:components/python3-attrs.bst
- freedesktop-sdk.bst:components/python3-six.bst
- pyrsistent.bst

sources:
- kind: git_tag
  url: github:Julian/jsonschema.git
  track: master
  track-tags: True
  ref: v3.0.2-0-gd5946354b2277961f1886002e835d40ee183f7b9

kind: pip

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-python.bst

depends:
- freedesktop-sdk.bst:components/python3.bst

sources:
- kind: pypi
  name: pathspec
  ref:
    sha256sum: e564499435a2673d586f6b2130bb5b95f04a3ba06f81b8f895b651a3c76aabb1
    suffix: f6/33/436c5cb94e9f8902e59d1d544eb298b83c84b9ec37b5b769c5a0ad6edb19/pathspec-0.9.0.tar.gz

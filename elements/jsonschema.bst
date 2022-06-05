kind: pip

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-python.bst
- hatchling.bst
- hatch-vcs.bst

depends:
- freedesktop-sdk.bst:components/python3.bst

runtime-depends:
- freedesktop-sdk.bst:components/python3-attrs.bst
- freedesktop-sdk.bst:components/python3-six.bst
- pyrsistent.bst

sources:
- kind: pypi
  name: jsonschema
  ref:
    sha256sum: 9d6397ba4a6c0bf0300736057f649e3e12ecbc07d3e81a0dacb72de4e9801957
    suffix: b5/a0/dd13abb5f371f980037d271fd09461df18c85188216008a1e3a9c3f8bd0c/jsonschema-4.6.0.tar.gz

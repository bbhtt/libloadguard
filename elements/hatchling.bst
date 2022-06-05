kind: pip

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-python.bst

depends:
- freedesktop-sdk.bst:components/python3.bst
- freedesktop-sdk.bst:components/python3-packaging.bst
- freedesktop-sdk.bst:components/python3-tomli.bst
- pathspec.bst
- pluggy.bst

sources:
- kind: pypi
  name: hatchling
  ref:
    sha256sum: aaa63019c9d666053d9ff318097335e766123844547c52940155c0401c3c142f
    suffix: b0/49/caada91404326c3d4c610afc243e81dd4abc6b8d2caaa39d6954b42d8947/hatchling-1.3.1.tar.gz

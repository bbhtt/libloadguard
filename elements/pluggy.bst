kind: pip

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-python.bst

depends:
- freedesktop-sdk.bst:components/python3.bst

sources:
- kind: pypi
  name: pluggy
  ref:
    sha256sum: 4224373bacce55f955a878bf9cfa763c1e360858e330072059e10bad68531159
    suffix: a1/16/db2d7de3474b6e37cbb9c008965ee63835bba517e22cdb8c35b5116b5ce1/pluggy-1.0.0.tar.gz

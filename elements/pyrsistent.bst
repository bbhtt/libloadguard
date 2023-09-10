kind: pyproject

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-python-setuptools.bst

depends:
- freedesktop-sdk.bst:components/python3.bst

sources:
- kind: pypi
  name: pyrsistent
  ref:
    sha256sum: bfa0351be89c9fcbcb8c9879b826f4353be10f58f8a677efab0c017bf7137ec2
    suffix: b8/ef/325da441a385a8a931b3eeb70db23cb52da42799691988d8d943c5237f10/pyrsistent-0.19.2.tar.gz

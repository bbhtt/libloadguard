kind: pip

depends:
- freedesktop-sdk.bst:components/python3.bst

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-python.bst

sources:
- kind: pypi
  name: pyrsistent
  ref:
    sha256sum: d4d61f8b993a7255ba714df3aca52700f8125289f84f704cf80916517c46eb96
    suffix: 42/ac/455fdc7294acc4d4154b904e80d964cc9aae75b087bbf486be04df9f2abd/pyrsistent-0.18.1.tar.gz

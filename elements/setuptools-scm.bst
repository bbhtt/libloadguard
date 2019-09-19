kind: pip

depends:
- filename: freedesktop-sdk.bst:components/python3.bst
- filename: freedesktop-sdk.bst:components/python3-setuptools.bst
- filename: freedesktop-sdk.bst:components/git-minimal.bst

build-depends:
- filename: freedesktop-sdk.bst:components/python3-pip.bst
  
sources:
- kind: git_tag
  url: github:pypa/setuptools_scm.git
  track: master
  track-tags: True

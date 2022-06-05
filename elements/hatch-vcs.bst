kind: pip

build-depends:
- freedesktop-sdk.bst:public-stacks/buildsystem-python.bst

depends:
- freedesktop-sdk.bst:components/python3.bst
- hatchling.bst

runtime-depends:
- freedesktop-sdk.bst:components/python3-setuptools-scm.bst


sources:
- kind: pypi
  name: hatch-vcs
  ref:
    sha256sum: 9913d733b34eec9bb0345d0626ca32165a4ad2de15d1ce643c36d09ca908abff
    suffix: ac/13/b3d83564c0cc0f1d45b4ea2a78b87b383a8165797613fedf11f1a7e49b48/hatch_vcs-0.2.0.tar.gz

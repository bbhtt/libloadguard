kind: autotools

depends:
- filename: freedesktop-sdk.bst:bootstrap-import.bst

build-depends:
- filename: freedesktop-sdk.bst:public-stacks/buildsystem-autotools.bst

sources:
- kind: git_tag
  url: github:yaml/libyaml.git
  track: master
  track-tags: True


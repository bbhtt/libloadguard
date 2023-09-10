kind: junction

config:
  options:
    target_arch: "%{arch}"

sources:
- kind: git_repo
  url: https://gitlab.com/freedesktop-sdk/freedesktop-sdk.git
  track: freedesktop-sdk-23.08.*
  ref-format: git-describe
  ref: freedesktop-sdk-23.08.0-0-g1533f8d0393984f5684985c8b710e9fbaf9c8734

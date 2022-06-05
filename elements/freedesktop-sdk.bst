kind: junction

config:
  options:
    target_arch: "%{arch}"

sources:
- kind: git_tag
  url: https://gitlab.com/freedesktop-sdk/freedesktop-sdk.git
  track: release/21.08
  track-tags: true
  ref: freedesktop-sdk-21.08.14-0-g5def401ccfd2863c3aa38b5cd1af3aa8b991c031

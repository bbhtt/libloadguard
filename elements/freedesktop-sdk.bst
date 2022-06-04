kind: junction

config:
  options:
    target_arch: "%{arch}"

sources:
- kind: git_tag
  url: https://gitlab.com/freedesktop-sdk/freedesktop-sdk.git
  ref: release/21.08

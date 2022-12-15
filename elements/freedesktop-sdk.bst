kind: junction

config:
  options:
    target_arch: "%{arch}"

sources:
- kind: git_tag
  url: https://gitlab.com/freedesktop-sdk/freedesktop-sdk.git
  track: release/22.08
  track-tags: true
  ref: freedesktop-sdk-22.08.4-0-gf1224db8495fcdcbc3869962a6f763c8f751d34d

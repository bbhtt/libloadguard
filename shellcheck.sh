#!/bin/sh

git ls-files '*.sh' '*.bash' | xargs shellcheck -S warning

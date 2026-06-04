#!/bin/sh

git ls-files '*.c' '*.h' | xargs uncrustify -c uncrustify.cfg --no-backup

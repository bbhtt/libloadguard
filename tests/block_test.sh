#!/usr/bin/env bash

set -euo pipefail

AUDIT_LIB="/app/lib/libloadguard.so"
CONFIG="/tmp/test.conf"
ARCH_TRIPLET="$(gcc --print-multiarch)"

printf "/usr/bin/ls /usr/lib/$ARCH_TRIPLET/libselinux*\n" > "$CONFIG"

output=$(LD_AUDIT="$AUDIT_LIB" LIBLOADGUARD_CONFIG="$CONFIG" LIBLOADGUARD_DEBUG=1 /bin/ls 2>&1 || true)

printf "\n\n"

echo "$output"

if echo "$output" | grep -q "Blocked library"; then
    printf "\nPASS: library was blocked\n"
    exit 0
else
    printf "\nFAIL: library was not blocked\n"
    exit 1
fi

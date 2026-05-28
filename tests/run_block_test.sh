#!/usr/bin/env bash

set -euo pipefail

for cmd in git flatpak flatpak-builder; do
    if ! command -v "$cmd" &>/dev/null; then
        echo "Error: '$cmd' not found in PATH" >&2
        exit 1
    fi
done

TOPLEVEL="$(git rev-parse --show-toplevel 2>/dev/null)" || {
    echo "Error: failed to find git toplevel" >&2
    exit 1
}

cd "$TOPLEVEL"

export FLATPAK_USER_DIR="$TOPLEVEL/.flatpak-test"
mkdir -p "$FLATPAK_USER_DIR"

APPID="io.github.bbhtt.libloadguard"
MANIFEST_FILE="$APPID.json"

if [[ -n "${GITHUB_WORKSPACE:-}" ]]; then
    STATE_DIR_ARGS="--state-dir=$GITHUB_WORKSPACE/cache/.flatpak-builder"
else
    STATE_DIR_ARGS="--state-dir=$TOPLEVEL/.flatpak-builder-state"
fi

flatpak remote-add --user --if-not-exists flathub \
    https://dl.flathub.org/repo/flathub.flatpakrepo

flatpak uninstall --force-remove --user --no-related --app \
    --delete-data --assumeyes --noninteractive "$APPID" || true

flatpak-builder build \
    --user \
    --force-clean \
    --install-deps-from=flathub \
    --ccache \
    --assumeyes \
    --delete-build-dirs \
    --install \
    "${STATE_DIR_ARGS}" \
    data/"$MANIFEST_FILE"

flatpak run -d "$APPID"

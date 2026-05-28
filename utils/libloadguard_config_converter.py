#!/usr/bin/env python3

import argparse
import itertools
import json
import logging
import sys
from collections.abc import Generator
from pathlib import Path
from typing import Required, TypedDict

import jsonschema
import yaml

logger = logging.getLogger(__name__)


class BinaryEntry(TypedDict, total=False):
    path: Required[str]
    libdir: str
    no_prefix: bool


class LibraryEntry(TypedDict, total=False):
    path: Required[str]
    no_prefix: bool


class Blocklist(TypedDict, total=False):
    binaries: Required[list[BinaryEntry]]
    libraries: Required[list[LibraryEntry]]
    subdir: str


class Entry(TypedDict):
    basedir: str
    blocklists: list[Blocklist]


class Manifest(TypedDict):
    entries: list[Entry]


SCHEMA = {
    "type": "object",
    "required": ["entries"],
    "properties": {
        "entries": {
            "type": "array",
            "items": {
                "type": "object",
                "required": ["basedir", "blocklists"],
                "properties": {
                    "basedir": {"type": "string"},
                    "blocklists": {
                        "type": "array",
                        "items": {
                            "type": "object",
                            "required": ["binaries", "libraries"],
                            "properties": {
                                "binaries": {
                                    "type": "array",
                                    "items": {
                                        "type": "object",
                                        "required": ["path"],
                                        "properties": {
                                            "path": {"type": "string"},
                                            "libdir": {
                                                "type": "string"
                                            },
                                            "no-prefix": {
                                                "type": "boolean"
                                            },
                                        },
                                    },
                                },
                                "libraries": {
                                    "type": "array",
                                    "items": {
                                        "type": "object",
                                        "required": ["path"],
                                        "properties": {
                                            "path": {"type": "string"},
                                            "no-prefix": {
                                                "type": "boolean"
                                            },
                                        },
                                    },
                                },
                                "subdir": {"type": "string"},
                            },
                        },
                    },
                },
            },
        }
    },
}

ESCAPE_CHARS = "\\ \n"


def escape(string: str) -> str:
    s = str(string)
    for c in ESCAPE_CHARS:
        s = s.replace(c, "\\" + c)
    return s


def blocklist_line(binary: Path, library: Path) -> str:
    return f"{escape(str(binary))} {escape(str(library))}"


def get_item_path(
    item: BinaryEntry | LibraryEntry, prefix: Path | None = None
) -> Path:
    item_path = Path(item["path"])
    if (
        prefix is None
        or item.get("no-prefix", False)
        or item_path.is_absolute()
    ):
        return item_path
    return Path(prefix) / item_path


def generate_blocklist(
    manifest: Manifest,
) -> Generator[str, None, None]:
    for entry in manifest["entries"]:
        basedir = Path(entry["basedir"])
        for blocklist in entry["blocklists"]:
            subdir = basedir / blocklist.get("subdir", "")
            for binary in blocklist["binaries"]:
                bin_path = get_item_path(binary, subdir)
                for library in blocklist["libraries"]:
                    lib_path = get_item_path(
                        library, subdir / binary.get("libdir", "")
                    )
                    yield blocklist_line(bin_path, lib_path)


def load_json_manifest(manifest_path: str) -> Manifest | None:
    manifest: Manifest | None = None
    try:
        with open(manifest_path, encoding="utf-8") as mf:
            manifest = json.load(mf)
    except json.JSONDecodeError as err:
        logger.error(
            "Failed to parse JSON manifest %s: %s", manifest_path, err
        )
    return manifest


def load_yaml_manifest(manifest_path: str) -> Manifest | None:
    manifest: Manifest | None = None
    try:
        with open(manifest_path, encoding="utf-8") as mf:
            manifest = yaml.safe_load(mf)
    except yaml.YAMLError as err:
        logger.error(
            "Failed to parse YAML manifest %s: %s", manifest_path, err
        )
    return manifest


def validate_manifest(manifest: Manifest) -> bool:
    try:
        jsonschema.validate(instance=manifest, schema=SCHEMA)
        return True
    except jsonschema.ValidationError as err:
        logger.error("Manifest validation failed: %s", err)
        return False


def load_manifest(manifest_path: str) -> Manifest | None:
    manifest: Manifest | None = None

    if not Path(manifest_path).exists():
        return None

    if manifest_path.lower().endswith(".json"):
        loader = load_json_manifest
    elif manifest_path.lower().endswith((".yml", ".yaml")):
        loader = load_yaml_manifest
    else:
        logger.error("Unknown manifest format: %s", manifest_path)
        return None

    manifest = loader(manifest_path)
    if manifest is None:
        return None

    if not validate_manifest(manifest):
        return None

    return manifest


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate ld.so blocklist",
        formatter_class=argparse.RawTextHelpFormatter,
        usage=argparse.SUPPRESS,
        add_help=False,
    )
    parser.add_argument(
        "-h",
        "--help",
        action="help",
        help="Show this help message and exit",
    )
    parser.add_argument(
        "--manifest",
        required=True,
        nargs="+",
        metavar="",
        help="Path(s) to manifest files",
    )
    parser.add_argument(
        "-o",
        "--outfile",
        metavar="",
        required=False,
        help="Write blocklist to this file",
    )
    args = parser.parse_args()

    loaded: list[Manifest] = []

    for m in args.manifest:
        manifest = load_manifest(m)
        if manifest is None:
            return 1
        loaded.append(manifest)

    blocklist_lines = itertools.chain.from_iterable(
        generate_blocklist(m) for m in loaded
    )
    blocklist = "\n".join(blocklist_lines) + "\n"

    if args.outfile is not None:
        try:
            with open(args.outfile, "w", encoding="utf-8") as f:
                f.write(blocklist)
        except OSError as err:
            logger.error(
                "Failed to write output file %r: %s", args.outfile, err
            )
            return 1
    else:
        sys.stdout.write(blocklist)

    return 0


if __name__ == "__main__":
    logging.basicConfig(
        format="%(levelname)s: %(message)s", stream=sys.stderr
    )
    raise SystemExit(main())

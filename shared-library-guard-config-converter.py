#!/usr/bin/env python3

import argparse
import itertools
import json
import sys
from pathlib import Path

import jsonschema
import yaml

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
                                            "libdir": {"type": "string"},
                                            "no-prefix": {"type": "boolean"},
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
                                            "no-prefix": {"type": "boolean"},
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


def escape(string: str):
    s = str(string)
    for c in ESCAPE_CHARS:
        s = s.replace(c, "\\" + c)
    return s


def blocklist_line(binary: Path, library: Path):
    return f"{escape(binary)} {escape(library)}"


def get_item_path(item: dict, prefix: Path | None = None):
    item_path = Path(item["path"])
    if prefix is None or item.get("no-prefix", False) or item_path.is_absolute():
        return item_path
    return Path(prefix) / item_path


def generate_blocklist(manifest: dict):
    for entry in manifest["entries"]:
        basedir = Path(entry["basedir"])
        for blocklist in entry["blocklists"]:
            subdir = basedir / blocklist.get("subdir", "")
            for binary in blocklist["binaries"]:
                bin_path = get_item_path(binary, subdir)
                for library in blocklist["libraries"]:
                    lib_path = get_item_path(library, subdir / binary.get("libdir", ""))
                    yield blocklist_line(bin_path, lib_path)


def load_manifest(manifest_path):
    if manifest_path.lower().endswith(".json"):
        loader = json.load
    elif manifest_path.lower().endswith((".yml", ".yaml")):
        loader = yaml.safe_load
    else:
        raise ValueError("Manifest format not known.")
    with open(manifest_path, "r", encoding="utf-8") as mf:
        manifest = loader(mf)
    jsonschema.validate(instance=manifest, schema=SCHEMA)
    return manifest


def main():
    parser = argparse.ArgumentParser("Generate ld.so blocklist for Freedesktop.org SDK")
    parser.add_argument("manifest", nargs="+", help="Path to manifest file")
    parser.add_argument(
        "-o", "--outfile", required=False, help="Write blocklist to this file"
    )
    args = parser.parse_args()

    blocklist_lines = itertools.chain(
        *[generate_blocklist(load_manifest(m)) for m in args.manifest]
    )
    blocklist = "\n".join(blocklist_lines) + "\n"

    if args.outfile is not None:
        with open(args.outfile, "w", encoding="utf-8") as f:
            f.write(blocklist)
    else:
        sys.stdout.write(blocklist)


if __name__ == "__main__":
    main()

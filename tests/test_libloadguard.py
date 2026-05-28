import json
import sys
from pathlib import Path

import pytest
import yaml

sys.path.insert(0, str(Path(__file__).parent.parent / "utils"))

from libloadguard_config_converter import (
    LibraryEntry,
    Manifest,
    blocklist_line,
    escape,
    generate_blocklist,
    get_item_path,
    load_manifest,
    main,
    validate_manifest,
)


class TestEscape:
    def test_no_special_chars(self) -> None:
        assert escape("simple") == "simple"

    def test_space_escaped(self) -> None:
        assert escape("with space") == "with\\ space"

    def test_backslash_escaped(self) -> None:
        assert escape("back\\slash") == "back\\\\slash"

    def test_newline_escaped(self) -> None:
        assert escape("line\nbreak") == "line\\\nbreak"

    def test_multiple_specials(self) -> None:
        assert escape("a b\\c") == "a\\ b\\\\c"

    def test_empty_string(self) -> None:
        assert escape("") == ""


class TestBlocklistLine:
    def test_simple_paths(self) -> None:
        result = blocklist_line(
            Path("/usr/bin/foo"), Path("/usr/lib/libfoo.so")
        )
        assert result == "/usr/bin/foo /usr/lib/libfoo.so"

    def test_paths_with_spaces(self) -> None:
        result = blocklist_line(
            Path("/usr/bin/my tool"), Path("/usr/lib/my lib.so")
        )
        assert result == "/usr/bin/my\\ tool /usr/lib/my\\ lib.so"


class TestGetItemPath:
    def test_no_prefix(self) -> None:
        item: LibraryEntry = {"path": "libfoo.so"}
        assert get_item_path(item) == Path("libfoo.so")

    def test_with_prefix(self) -> None:
        item: LibraryEntry = {"path": "libfoo.so"}
        assert get_item_path(item, Path("/usr/lib")) == Path(
            "/usr/lib/libfoo.so"
        )

    def test_no_prefix_flag_skips_prefix(self) -> None:
        item: LibraryEntry = {"path": "libfoo.so", "no-prefix": True}  # type: ignore[typeddict-unknown-key]
        assert get_item_path(item, Path("/usr/lib")) == Path(
            "libfoo.so"
        )

    def test_absolute_path_skips_prefix(self) -> None:
        item: LibraryEntry = {"path": "/absolute/libfoo.so"}
        assert get_item_path(item, Path("/usr/lib")) == Path(
            "/absolute/libfoo.so"
        )

    def test_prefix_none_with_relative_path(self) -> None:
        item: LibraryEntry = {"path": "sub/libfoo.so"}
        assert get_item_path(item, None) == Path("sub/libfoo.so")


class TestGenerateBlocklist:
    def _simple_manifest(self) -> Manifest:
        return {
            "entries": [
                {
                    "basedir": "/opt/app",
                    "blocklists": [
                        {
                            "binaries": [{"path": "bin/foo"}],
                            "libraries": [{"path": "lib/libfoo.so"}],
                        }
                    ],
                }
            ]
        }

    def test_basic_output(self) -> None:
        lines = list(generate_blocklist(self._simple_manifest()))
        assert len(lines) == 1
        assert lines[0] == "/opt/app/bin/foo /opt/app/lib/libfoo.so"

    def test_subdir_applied(self) -> None:
        manifest: Manifest = {
            "entries": [
                {
                    "basedir": "/opt/app",
                    "blocklists": [
                        {
                            "subdir": "v2",
                            "binaries": [{"path": "bin/foo"}],
                            "libraries": [{"path": "lib/libfoo.so"}],
                        }
                    ],
                }
            ]
        }
        lines = list(generate_blocklist(manifest))
        assert (
            lines[0] == "/opt/app/v2/bin/foo /opt/app/v2/lib/libfoo.so"
        )

    def test_libdir_on_binary(self) -> None:
        manifest: Manifest = {
            "entries": [
                {
                    "basedir": "/opt",
                    "blocklists": [
                        {
                            "binaries": [
                                {"path": "bin/foo", "libdir": "lib64"}
                            ],
                            "libraries": [{"path": "libfoo.so"}],
                        }
                    ],
                }
            ]
        }
        lines = list(generate_blocklist(manifest))
        assert lines[0] == "/opt/bin/foo /opt/lib64/libfoo.so"

    def test_cartesian_product(self) -> None:
        manifest: Manifest = {
            "entries": [
                {
                    "basedir": "/opt",
                    "blocklists": [
                        {
                            "binaries": [
                                {"path": "bin/a"},
                                {"path": "bin/b"},
                            ],
                            "libraries": [
                                {"path": "lib/x.so"},
                                {"path": "lib/y.so"},
                            ],
                        }
                    ],
                }
            ]
        }
        lines = list(generate_blocklist(manifest))
        assert len(lines) == 4

    def test_no_prefix_on_library(self) -> None:
        manifest: Manifest = {
            "entries": [
                {
                    "basedir": "/opt",
                    "blocklists": [
                        {
                            "binaries": [{"path": "bin/foo"}],
                            "libraries": [
                                {
                                    "path": "/abs/lib/libbar.so",
                                    "no-prefix": True,
                                }  # type: ignore[typeddict-unknown-key]
                            ],
                        }
                    ],
                }
            ]
        }
        lines = list(generate_blocklist(manifest))
        assert lines[0] == "/opt/bin/foo /abs/lib/libbar.so"

    def test_multiple_entries(self) -> None:
        manifest: Manifest = {
            "entries": [
                {
                    "basedir": "/a",
                    "blocklists": [
                        {
                            "binaries": [{"path": "bin/foo"}],
                            "libraries": [{"path": "lib/liba.so"}],
                        }
                    ],
                },
                {
                    "basedir": "/b",
                    "blocklists": [
                        {
                            "binaries": [{"path": "bin/bar"}],
                            "libraries": [{"path": "lib/libb.so"}],
                        }
                    ],
                },
            ]
        }
        lines = list(generate_blocklist(manifest))
        assert len(lines) == 2
        assert lines[0] == "/a/bin/foo /a/lib/liba.so"
        assert lines[1] == "/b/bin/bar /b/lib/libb.so"


class TestValidateManifest:
    def test_valid_manifest(self) -> None:
        manifest: Manifest = {
            "entries": [
                {
                    "basedir": "/opt",
                    "blocklists": [
                        {
                            "binaries": [{"path": "bin/foo"}],
                            "libraries": [{"path": "lib/libfoo.so"}],
                        }
                    ],
                }
            ]
        }
        assert validate_manifest(manifest) is True

    def test_missing_entries_key(self) -> None:
        raw: object = {"not_entries": []}
        assert validate_manifest(raw) is False  # type: ignore[arg-type]

    def test_missing_basedir(self) -> None:
        manifest: Manifest = {
            "entries": [
                {
                    "basedir": "",
                    "blocklists": [
                        {
                            "binaries": [{"path": "bin/foo"}],
                            "libraries": [],
                        }
                    ],
                }
            ]
        }
        assert validate_manifest(manifest) is True

    def test_missing_binary_path(self) -> None:
        raw: object = {
            "entries": [
                {
                    "basedir": "/opt",
                    "blocklists": [
                        {
                            "binaries": [{"libdir": "lib"}],
                            "libraries": [{"path": "lib/libfoo.so"}],
                        }
                    ],
                }
            ]
        }
        assert validate_manifest(raw) is False  # type: ignore[arg-type]

    def test_optional_subdir_accepted(self) -> None:
        manifest: Manifest = {
            "entries": [
                {
                    "basedir": "/opt",
                    "blocklists": [
                        {
                            "subdir": "extra",
                            "binaries": [{"path": "bin/foo"}],
                            "libraries": [{"path": "lib/libfoo.so"}],
                        }
                    ],
                }
            ]
        }
        assert validate_manifest(manifest) is True


_VALID_MANIFEST: Manifest = {
    "entries": [
        {
            "basedir": "/opt",
            "blocklists": [
                {
                    "binaries": [{"path": "bin/foo"}],
                    "libraries": [{"path": "lib/libfoo.so"}],
                }
            ],
        }
    ]
}


class TestLoadManifest:
    def test_load_valid_json(self, tmp_path: Path) -> None:
        p = tmp_path / "manifest.json"
        p.write_text(json.dumps(_VALID_MANIFEST), encoding="utf-8")
        result = load_manifest(str(p))
        assert result is not None
        assert result["entries"][0]["basedir"] == "/opt"

    def test_load_valid_yaml(self, tmp_path: Path) -> None:
        p = tmp_path / "manifest.yaml"
        p.write_text(yaml.dump(_VALID_MANIFEST), encoding="utf-8")
        assert load_manifest(str(p)) is not None

    def test_load_valid_yml_extension(self, tmp_path: Path) -> None:
        p = tmp_path / "manifest.yml"
        p.write_text(yaml.dump(_VALID_MANIFEST), encoding="utf-8")
        assert load_manifest(str(p)) is not None

    def test_unknown_extension_returns_none(
        self, tmp_path: Path
    ) -> None:
        p = tmp_path / "manifest.toml"
        p.write_text("", encoding="utf-8")
        assert load_manifest(str(p)) is None

    def test_invalid_json_returns_none(self, tmp_path: Path) -> None:
        p = tmp_path / "bad.json"
        p.write_text("{not valid json", encoding="utf-8")
        assert load_manifest(str(p)) is None

    def test_invalid_yaml_returns_none(self, tmp_path: Path) -> None:
        p = tmp_path / "bad.yaml"
        p.write_text("entries:\n\t- bad", encoding="utf-8")
        assert load_manifest(str(p)) is None

    def test_schema_invalid_json_returns_none(
        self, tmp_path: Path
    ) -> None:
        p = tmp_path / "bad_schema.json"
        p.write_text(
            json.dumps({"entries": "not-a-list"}), encoding="utf-8"
        )
        assert load_manifest(str(p)) is None


class TestMain:
    def test_writes_to_stdout(
        self,
        tmp_path: Path,
        capsys: pytest.CaptureFixture[str],
        monkeypatch: pytest.MonkeyPatch,
    ) -> None:
        p = tmp_path / "m.json"
        p.write_text(json.dumps(_VALID_MANIFEST), encoding="utf-8")
        monkeypatch.setattr(sys, "argv", ["prog", "--manifest", str(p)])
        assert main() == 0
        assert (
            "/opt/bin/foo /opt/lib/libfoo.so" in capsys.readouterr().out
        )

    def test_writes_to_outfile(
        self, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
    ) -> None:
        p = tmp_path / "m.json"
        p.write_text(json.dumps(_VALID_MANIFEST), encoding="utf-8")
        out_file = tmp_path / "blocklist.txt"
        monkeypatch.setattr(
            sys,
            "argv",
            ["prog", "--manifest", str(p), "-o", str(out_file)],
        )
        assert main() == 0
        content = out_file.read_text(encoding="utf-8")
        assert "/opt/bin/foo /opt/lib/libfoo.so" in content
        assert content.endswith("\n")

    def test_invalid_manifest_returns_1(
        self, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
    ) -> None:
        p = tmp_path / "bad.json"
        p.write_text("{}", encoding="utf-8")
        monkeypatch.setattr(sys, "argv", ["prog", "--manifest", str(p)])
        assert main() == 1

    def test_multiple_manifests(
        self,
        tmp_path: Path,
        capsys: pytest.CaptureFixture[str],
        monkeypatch: pytest.MonkeyPatch,
    ) -> None:
        data_a: Manifest = {
            "entries": [
                {
                    "basedir": "/a",
                    "blocklists": [
                        {
                            "binaries": [{"path": "bin/a"}],
                            "libraries": [{"path": "lib/la.so"}],
                        }
                    ],
                }
            ]
        }
        data_b: Manifest = {
            "entries": [
                {
                    "basedir": "/b",
                    "blocklists": [
                        {
                            "binaries": [{"path": "bin/b"}],
                            "libraries": [{"path": "lib/lb.so"}],
                        }
                    ],
                }
            ]
        }
        pa = tmp_path / "a.json"
        pb = tmp_path / "b.json"
        pa.write_text(json.dumps(data_a), encoding="utf-8")
        pb.write_text(json.dumps(data_b), encoding="utf-8")
        monkeypatch.setattr(
            sys, "argv", ["prog", "--manifest", str(pa), str(pb)]
        )
        assert main() == 0
        out = capsys.readouterr().out
        assert "/a/bin/a /a/lib/la.so" in out
        assert "/b/bin/b /b/lib/lb.so" in out

    def test_nonexistent_manifest_returns_none(
        self, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
    ) -> None:
        missing = tmp_path / "missing.json"
        monkeypatch.setattr(
            sys, "argv", ["prog", "--manifest", str(missing)]
        )

        assert main() == 1

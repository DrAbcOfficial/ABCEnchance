#!/usr/bin/env python3
"""Build the single ABC gamedata catalog shared by AbcUi and AbcEssential.

The two plugin manifests select a subset of the upstream symbol catalog. The
AbcUi layout catalog adds AbcUi-specific scalar/structMember records for the
same game versions. This script first synchronizes the union of the plugin
symbols, then merges the layout binaries/records into one snapshot per game
version and writes one index.json.
"""

import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path


def parse_arguments():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--core-dir", required=True, type=Path)
    parser.add_argument("--manifest", required=True, type=Path)
    parser.add_argument("--layout-dir", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--temp-root", required=True, type=Path)
    return parser.parse_args()


def load_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path, document):
    text = json.dumps(document, indent=2, ensure_ascii=False) + "\n"
    payload = text.encode("utf-8")
    path.write_bytes(payload)
    return payload


def merge_binaries(base, extra, game_version):
    merged = json.loads(json.dumps(base))
    for module, platforms in extra.items():
        if module not in merged:
            merged[module] = platforms
            continue
        for platform, identity in platforms.items():
            current = merged[module].get(platform)
            if current is None:
                merged[module][platform] = identity
                continue
            current_crc = current.get("crc64")
            extra_crc = identity.get("crc64")
            if current_crc and extra_crc and current_crc.lower() != extra_crc.lower():
                raise SystemExit(
                    f"gamedata binary identity mismatch for {game_version}/{module}/{platform}: "
                    f"{current_crc} != {extra_crc}"
                )
            for key, value in identity.items():
                current.setdefault(key, value)
    return merged


def merge_records(base, extra, game_version):
    merged = []
    seen = {}
    for record in list(base) + list(extra):
        key = (record.get("module"), record.get("symbolName"))
        if not key[0] or not key[1]:
            raise SystemExit(f"gamedata record without module/name in {game_version}")
        previous = seen.get(key)
        if previous is not None:
            if previous != record:
                raise SystemExit(
                    f"conflicting gamedata record {key} in {game_version}"
                )
            continue
        seen[key] = record
        merged.append(record)
    return merged


def run_checked(command):
    print("[ABC gamedata] " + " ".join(str(part) for part in command), flush=True)
    subprocess.run(command, check=True)


def main():
    args = parse_arguments()
    core_dir = args.core_dir.resolve()
    manifest = args.manifest.resolve()
    layout_dir = args.layout_dir.resolve()
    output = args.output.resolve()
    temp_root = args.temp_root.resolve()

    sync_script = core_dir / "cmake" / "gamedata" / "sync-gamedata.py"
    validate_script = core_dir / "cmake" / "gamedata" / "validate-gamedata.py"
    if not sync_script.is_file() or not validate_script.is_file():
        raise SystemExit(f"AbcCore gamedata tools not found under {core_dir}")
    if not layout_dir.is_dir():
        raise SystemExit(f"AbcUi layout catalog not found at {layout_dir}")

    output.mkdir(parents=True, exist_ok=True)
    temp_root.mkdir(parents=True, exist_ok=True)

    # Validate the hand-pinned layout data before it is merged.
    run_checked([sys.executable, str(validate_script), str(layout_dir)])

    # Build the union of the plugin symbol manifests.
    run_checked(
        [
            sys.executable,
            str(sync_script),
            "--manifest",
            str(manifest),
            "--target-dir",
            str(output),
            "--temp-root",
            str(temp_root),
        ]
    )
    run_checked(
        [
            sys.executable,
            str(validate_script),
            str(output),
            "--manifest",
            str(manifest),
        ]
    )

    base_index = load_json(output / "index.json")
    layout_index = load_json(layout_dir / "index.json")
    base_versions = {item["gameVersion"]: item for item in base_index["versions"]}
    layout_versions = {item["gameVersion"]: item for item in layout_index["versions"]}
    if set(base_versions) != set(layout_versions):
        raise SystemExit(
            "the symbol and layout catalogs cover different game versions: "
            f"{sorted(base_versions)} != {sorted(layout_versions)}"
        )

    merged_versions = []
    for game_version in sorted(base_versions):
        base_entry = base_versions[game_version]
        layout_entry = layout_versions[game_version]
        base_snapshot = load_json(output / base_entry["url"])
        layout_snapshot = load_json(layout_dir / layout_entry["url"])

        merged = dict(base_snapshot)
        merged["binaries"] = merge_binaries(
            base_snapshot.get("binaries", {}),
            layout_snapshot.get("binaries", {}),
            game_version,
        )
        merged["records"] = merge_records(
            base_snapshot.get("records", []),
            layout_snapshot.get("records", []),
            game_version,
        )

        file_name = f"{game_version}.json"
        payload = write_json(output / file_name, merged)
        merged_versions.append(
            {
                "gameVersion": game_version,
                "url": file_name,
                "sha256": hashlib.sha256(payload).hexdigest(),
                "size": len(payload),
                "snapshotSchemaVersion": base_entry.get("snapshotSchemaVersion", 8),
                "fileCount": len(merged["records"]),
                "lastPublishTime": base_entry.get("lastPublishTime", ""),
            }
        )

    write_json(
        output / "index.json",
        {"schemaVersion": 4, "versions": merged_versions},
    )
    run_checked([sys.executable, str(validate_script), str(output)])
    print(
        f"[ABC gamedata] merged {len(merged_versions)} game version(s) into {output}",
        flush=True,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())

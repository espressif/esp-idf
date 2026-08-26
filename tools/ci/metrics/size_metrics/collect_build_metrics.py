#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
ESP-IDF Size Metrics Extractor (Build-Time)

For each selected app this writes one build_size_metrics.json raw artifact into
the build dir. It does NOT upload anything: the artifacts are collected from
build jobs and a single downstream job (generate_metrics.py + esp-metrics-cli)
extracts, aggregates, and uploads them once.

Entry point:
- build_raw_metrics_for_build_dir(): called per-app from IdfCMakeApp._post_build,
  while the bootloader binary still exists (it is deleted right after).

Design:
- size_metrics_config.yml is the build-side gate: only apps listed there produce
  raw artifacts, every other built app is ignored
- For a listed app: the artifact carries esp-idf-size --archives JSON generated
  here from the app map file, plus best-effort bootloader esp-idf-size JSON and
  binary size when explicitly enabled and present. The size_*.json written by
  idf-build-apps is left untouched (other CI consumers depend on its default format).
  The downstream aggregator decides which schema entries to emit.
- No upload / esp-metrics-cli / DB credentials needed here
"""

import json
import subprocess
import sys
from collections.abc import Iterator
from pathlib import Path
from typing import Any
from typing import TypedDict

try:
    import yaml
except ImportError:
    yaml = None  # type: ignore[assignment]


class SizeMetricsConfig(TypedDict):
    libs: list[str] | bool
    bootloader: bool
    configs: list[str] | None
    targets: list[str] | None


def _opt_str_list(value: Any) -> list[str] | None:
    """Normalize an optional YAML list into list[str], or None when absent/not a list."""
    return [str(v) for v in value] if isinstance(value, list) else None


def load_size_metrics_config(config_file: Path) -> dict[str, SizeMetricsConfig]:
    """
    Load the size metrics config YAML.

    The config is the single source of truth for which apps publish size metrics.
    Returns a dict mapping app_path -> metric selection:
        libs=False        -> app_size only (no per-library breakdown)
        libs=True         -> app_size + every library
        libs=list[str]    -> app_size + only the named archives
        bootloader=True   -> include bootloader_size when available
        configs=None      -> every built config of the app
        configs=list[str] -> only builds whose config matches one of these
        targets=None      -> every built target of the app
        targets=list[str] -> only builds whose target matches one of these
    Returns an empty dict if the config file is missing or yaml is unavailable
    (in which case no metrics are published at all).
    """
    if not config_file.exists():
        print(f'Note: {config_file} not found - no size metrics will be posted')
        return {}

    if yaml is None:
        print('Warning: PyYAML not available - skipping size metrics config')
        return {}

    try:
        with open(config_file) as f:
            data = yaml.safe_load(f) or {}
    except Exception as e:
        print(f'Warning: Failed to load {config_file}: {e}')
        return {}

    result: dict[str, SizeMetricsConfig] = {}
    for app_path, entry in data.items():
        libs: list[str] | bool = False
        bootloader = False
        configs: list[str] | None = None
        targets: list[str] | None = None
        if isinstance(entry, dict):
            if 'libs' in entry:
                val = entry['libs']
                if isinstance(val, list):
                    libs = val
                else:
                    # null, the literal `all`, or any truthy scalar -> all libraries
                    libs = True
            bootloader = bool(entry.get('bootloader', False))
            configs = _opt_str_list(entry.get('configs'))
            targets = _opt_str_list(entry.get('targets'))
        result[str(app_path)] = {
            'libs': libs,
            'bootloader': bootloader,
            'configs': configs,
            'targets': targets,
        }

    return result


def is_build_selected(
    app_path: str,
    target: str,
    config: str,
    size_config: dict[str, SizeMetricsConfig],
) -> bool:
    """
    Return True if metrics should be published for this app build.

    A build is selected when the app is listed in the config and, when the
    entry restricts `configs`/`targets`, the build's config/target is in that
    list. An absent `configs`/`targets` filter matches every config/target.
    """
    entry = size_config.get(app_path)
    if entry is None:
        return False
    if entry['configs'] is not None and config not in entry['configs']:
        return False
    if entry['targets'] is not None and target not in entry['targets']:
        return False
    return True


def run_size_json(map_file: Path) -> dict[str, Any] | None:
    """Run esp-idf-size for a map file and return json2 archive output."""
    output_file = map_file.with_suffix('.size_metrics.json')
    try:
        result = subprocess.run(
            [
                sys.executable,
                '-m',
                'esp_idf_size',
                '--archives',
                '--format',
                'json2',
                '--output-file',
                str(output_file),
                str(map_file),
            ],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )
    except OSError as e:
        print(f'Warning: Failed to run esp-idf-size for {map_file}: {e}')
        return None

    if result.returncode != 0:
        stderr = result.stderr.strip()
        print(f'Warning: esp-idf-size failed for {map_file}: {stderr}')
        return None

    try:
        with open(output_file) as f:
            data: dict[str, Any] = json.load(f, strict=False)
        return data
    except (OSError, json.JSONDecodeError) as e:
        print(f'Warning: Failed to parse esp-idf-size output for {map_file}: {e}')
        return None
    finally:
        output_file.unlink(missing_ok=True)


def _iter_archive_sections(size_data: dict[str, Any]) -> Iterator[tuple[str | None, str, int]]:
    """
    Walk esp_idf_size --archives --format json2 output once.

    The json2 archives format is keyed by full archive path, e.g.:
        {"esp-idf/esp_timer/libesp_timer.a": {"abbrev_name": "libesp_timer.a", "memory_types": {...}}}

    Yields (abbrev_name, section_name, size) for every linker section of every
    archive, so the per-archive and per-total accumulators can share one traversal.
    """
    for entry in size_data.values():
        if not isinstance(entry, dict) or 'memory_types' not in entry:
            continue
        abbrev_name = entry.get('abbrev_name')
        for memory_type in entry.get('memory_types', {}).values():
            for section_name, section_data in memory_type.get('sections', {}).items():
                yield abbrev_name, section_name, section_data.get('size', 0)


def _memory_total_category(memory_type_name: str) -> str | None:
    """Map esp-idf-size memory type names to size metric total buckets."""
    lower_name = memory_type_name.lower()
    if 'flash' in lower_name:
        return 'flash'
    if lower_name == 'dram':
        return 'dram'
    if lower_name == 'iram':
        return 'iram'
    return None


def _section_total_category(section_name: str) -> str | None:
    """Map linker section prefixes to size metric total buckets."""
    if section_name.startswith('.flash.'):
        return 'flash'
    if section_name.startswith('.dram'):
        return 'dram'
    if section_name.startswith('.iram'):
        return 'iram'
    return None


def extract_all_archive_sizes(size_data: dict[str, Any]) -> dict[str, dict[str, int]]:
    """
    Extract per-archive (text/rodata/data/bss/total) size breakdown.

    Returns a dict keyed by abbreviated archive name (e.g. "libesp_timer.a").
    Archives with all-zero sizes are excluded.
    """
    section_map = {
        '.flash.text': 'text',
        '.iram0.text': 'text',
        '.flash.rodata': 'rodata',
        '.dram0.rodata': 'rodata',
        '.dram0.data': 'data',
        '.dram0.bss': 'bss',
    }

    libraries: dict[str, dict[str, int]] = {}

    for abbrev_name, section_name, size in _iter_archive_sections(size_data):
        category = section_map.get(section_name)
        if not abbrev_name or not category:
            continue
        sizes = libraries.setdefault(abbrev_name, {'text': 0, 'rodata': 0, 'data': 0, 'bss': 0})
        sizes[category] += size

    for name in list(libraries):
        sizes = libraries[name]
        if any(v > 0 for v in sizes.values()):
            sizes['total'] = sum(sizes.values())
        else:
            del libraries[name]

    return libraries


def extract_total_sizes(size_data: dict[str, Any]) -> dict[str, int]:
    """
    Extract total flash/DRAM/IRAM sizes.

    Returns a dict with keys: flash_total, dram_total, iram_total.
    """
    totals = {'flash': 0, 'dram': 0, 'iram': 0}

    for archive in size_data.values():
        if not isinstance(archive, dict):
            continue

        for memory_type_name, memory_type in archive.get('memory_types', {}).items():
            if not isinstance(memory_type, dict):
                continue

            category = _memory_total_category(str(memory_type_name))
            if category:
                totals[category] += memory_type.get('size', 0)
                continue

            # Split memory types such as DIRAM contain both DRAM and IRAM
            # sections, so classify those by section prefix.
            for section_name, section_data in memory_type.get('sections', {}).items():
                category = _section_total_category(str(section_name))
                if category:
                    totals[category] += section_data.get('size', 0)

    return {
        'flash_total': totals['flash'],
        'dram_total': totals['dram'],
        'iram_total': totals['iram'],
    }


def build_raw_metrics_for_build_dir(
    build_dir: Path,
    app_path: str,
    target: str,
    config: str,
    size_config: dict[str, SizeMetricsConfig],
) -> dict[str, Any] | None:
    """
    Build a raw size metrics artifact for one configured app build.

    The app/target/config identity is supplied by the caller (the ``App`` object
    already knows it), so there is no need to reverse-engineer it from the build
    dir name. The artifact intentionally keeps the build job dumb: it runs
    esp-idf-size --archives on the app map file (the size_*.json written by
    idf-build-apps keeps its default format for other CI consumers) and adds
    best-effort bootloader esp-idf-size JSON when explicitly enabled. The
    downstream aggregator applies the schema-specific extraction and library
    filtering.
    """
    if not is_build_selected(app_path, target, config, size_config):
        return None

    # The app map file sits at the top of the build dir; bootloader/TEE maps
    # live in subdirectories, so a non-recursive glob only matches the app map.
    app_maps = sorted(build_dir.glob('*.map'))
    if not app_maps:
        print(f'Warning: No app map file found in {build_dir}')
        return None

    app_size_data = run_size_json(app_maps[0])
    if app_size_data is None:
        return None

    identity = {
        'app': app_path,
        'target': target,
        'config': config,
    }
    artifact: dict[str, Any] = {
        'identity': identity,
        'binaries': [
            {
                'kind': 'app',
                'name': app_path,
                'size_json': app_size_data,
            }
        ],
    }

    if size_config[app_path]['bootloader']:
        boot_map = build_dir / 'bootloader' / 'bootloader.map'
    else:
        boot_map = None

    if boot_map and boot_map.is_file():
        boot_entry: dict[str, Any] = {
            'kind': 'bootloader',
            'name': 'bootloader',
            'size_json': run_size_json(boot_map),
        }
        boot_bin = boot_map.with_suffix('.bin')
        if boot_bin.is_file():
            boot_entry['bin_size'] = boot_bin.stat().st_size
        artifact['binaries'].append(boot_entry)

    return artifact


def write_raw_metrics_artifact(build_dir: Path, artifact: dict[str, Any]) -> Path:
    """Write a raw metrics artifact into the build dir, where it is collected
    as a build artifact (**/build*/build_size_metrics.json) and aggregated downstream."""
    artifact_path = build_dir / 'build_size_metrics.json'
    with open(artifact_path, 'w') as f:
        json.dump(artifact, f)
    return artifact_path

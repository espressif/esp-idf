#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
ESP-IDF Size Metrics Generator (aggregator)

Runs once in a single downstream job after all build jobs have finished. It
globs the per-app build*/build_size_metrics.json artifacts produced by
collect_build_metrics.py (collected as build artifacts), extracts the
configured metric families, and writes metrics.json next to schema.yaml ready
for `esp-metrics-cli upload`.

This step does not need a build environment, esp-idf-size, or DB credentials.
"""

import json
import os
from pathlib import Path
from typing import Any

from collect_build_metrics import SizeMetricsConfig
from collect_build_metrics import extract_all_archive_sizes
from collect_build_metrics import extract_total_sizes
from collect_build_metrics import load_size_metrics_config

# Top-level metric families; each becomes one document in the DB (see schema.yaml).
METRIC_FAMILIES = ('app_size', 'library_size', 'bootloader_size')


def find_fragments(root_dir: Path) -> list[Path]:
    """Find all metrics artifact files written into build* directories."""
    return sorted(root_dir.rglob('build*/build_size_metrics.json'))


def add_raw_artifact(
    data: dict[str, list[Any]], payload: dict[str, Any], size_config: dict[str, SizeMetricsConfig]
) -> None:
    """Extract schema entries from a raw per-app size metrics artifact."""
    identity = payload.get('identity', {})
    app = identity.get('app')
    # A fragment only exists because the collector already gated it against the
    # config, so a membership check is enough here (no need to re-check configs/targets).
    if not app or app not in size_config:
        return

    for binary in payload.get('binaries', []):
        kind = binary.get('kind')
        size_json = binary.get('size_json')

        if kind == 'app' and isinstance(size_json, dict):
            data['app_size'].append(
                {
                    **identity,
                    **extract_total_sizes(size_json),
                }
            )

            libs_spec = size_config[app]['libs']
            if libs_spec:
                all_libraries = extract_all_archive_sizes(size_json)
                if isinstance(libs_spec, list):
                    libraries = {k: v for k, v in all_libraries.items() if k in libs_spec}
                else:
                    libraries = all_libraries

                for lib_name, lib_sizes in libraries.items():
                    data['library_size'].append(
                        {
                            **identity,
                            'library': lib_name,
                            'text': lib_sizes['text'],
                            'rodata': lib_sizes['rodata'],
                            'data': lib_sizes['data'],
                            'bss': lib_sizes['bss'],
                            'total': lib_sizes['total'],
                        }
                    )

        elif kind == 'bootloader' and 'bin_size' in binary:
            data['bootloader_size'].append(
                {
                    **identity,
                    'bin_size': binary['bin_size'],
                }
            )


def aggregate_fragments(fragments: list[Path], size_config: dict[str, SizeMetricsConfig]) -> dict[str, Any]:
    """Extract artifact files into the esp-metrics document structure."""
    data: dict[str, list[Any]] = {family: [] for family in METRIC_FAMILIES}

    for frag in fragments:
        try:
            with open(frag) as f:
                payload = json.load(f)
        except (OSError, json.JSONDecodeError) as e:
            print(f'Warning: skipping unreadable fragment {frag}: {e}')
            continue

        if 'identity' in payload and 'binaries' in payload:
            add_raw_artifact(data, payload, size_config)
        else:
            print(f'Warning: skipping fragment with unexpected shape: {frag}')

    return {family: {'data': data[family]} for family in METRIC_FAMILIES}


def main() -> None:
    """CLI to aggregate size metric artifacts into metrics.json."""
    import argparse

    parser = argparse.ArgumentParser(description='Aggregate size metric artifacts into metrics.json')
    parser.add_argument(
        '--root',
        default=os.environ.get('IDF_PATH', os.getcwd()),
        help='Root directory to search for build*/build_size_metrics.json artifacts (default: IDF_PATH or cwd)',
    )
    parser.add_argument(
        '--output',
        default='metrics.json',
        help='Output metrics file (default: metrics.json, next to schema.yaml)',
    )
    parser.add_argument(
        '--config',
        default=None,
        help='Path to size_metrics_config.yml (default: next to this script)',
    )
    args = parser.parse_args()

    root_dir = Path(args.root)
    print(f'Searching for metric artifacts under {root_dir}...')

    script_dir = Path(__file__).parent
    config_file = Path(args.config) if args.config else script_dir / 'size_metrics_config.yml'
    size_config = load_size_metrics_config(config_file)

    fragments = find_fragments(root_dir)
    print(f'Found {len(fragments)} metric artifact(s)')

    metrics = aggregate_fragments(fragments, size_config)

    output_file = Path(args.output)
    output_file.write_text(json.dumps(metrics, indent=2))

    counts = {family: len(metrics[family]['data']) for family in METRIC_FAMILIES}
    summary = ', '.join(f'{n} {family}' for family, n in counts.items())
    print(f'\n✓ Aggregated [{summary}] into {output_file}')

    if not any(counts.values()):
        # Nothing to upload; keep the (empty) file so the upload step is a no-op.
        print('Note: no metrics collected - none of the built apps are listed in size_metrics_config.yml')


if __name__ == '__main__':
    main()

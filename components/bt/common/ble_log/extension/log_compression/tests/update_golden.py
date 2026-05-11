#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Regenerate golden expected output files from fixture C sources.

Usage:
    python3 update_golden.py

This runs the compression pipeline on each fixture C source file and copies
the generated header to fixtures/expected/. The output should be reviewed
for correctness before committing.
"""

import shutil
import sys
import tempfile
from pathlib import Path

TESTS_DIR = Path(__file__).resolve().parent
SCRIPTS_DIR = TESTS_DIR.parent / 'scripts'
FIXTURE_DIR = TESTS_DIR / 'fixtures'
EXPECTED_DIR = FIXTURE_DIR / 'expected'

sys.path.insert(0, str(SCRIPTS_DIR))

import yaml  # noqa: E402
from ble_log_compress import LogCompressor  # noqa: E402
from LogDBManager import LogDBManager  # noqa: E402

BLUEDROID_SCRIPT = SCRIPTS_DIR / 'module_scripts' / 'bluedroid' / 'make_bluedroid_log_macro.py'
BLUEDROID_TAGS = [
    'APPL_TRACE_ERROR',
    'APPL_TRACE_WARNING',
    'APPL_TRACE_API',
    'APPL_TRACE_DEBUG',
    'APPL_TRACE_EVENT',
    'APPL_TRACE_VERBOSE',
]


def run_pipeline(fixture_files: list[str], output_name: str, tags_with_preserve: list[str] | None = None) -> None:
    """Run compression on fixture files and save the generated header as a golden file."""
    tags_with_preserve = tags_with_preserve or ['APPL_TRACE_ERROR', 'APPL_TRACE_WARNING']

    with tempfile.TemporaryDirectory() as tmp_str:
        tmp = Path(tmp_str)
        code_base = tmp / 'code_base'
        build_dir = tmp / 'build'
        compressed_srcs = build_dir / 'ble_log' / '.compressed_srcs'
        (build_dir / 'ble_log' / 'include').mkdir(parents=True)
        compressed_srcs.mkdir(parents=True)

        # Write config
        config = {
            'log_config': {
                'db_path': 'ble_log/ble_log_database',
                'modules': {
                    'BLE_HOST': {
                        'description': 'BLE Host Test',
                        'code_path': ['test_src'],
                        'log_index_file': 'test_log_index.h',
                        'script': str(BLUEDROID_SCRIPT),
                        'tags': BLUEDROID_TAGS,
                        'tags_with_preserve': tags_with_preserve,
                    }
                },
            }
        }
        config_path = build_dir / 'ble_log' / 'module_info.yml'
        with open(config_path, 'w') as f:
            yaml.safe_dump(config, f)

        # Copy fixtures
        rel_paths = []
        for fname in fixture_files:
            src = FIXTURE_DIR / 'c_sources' / fname
            rel = Path('test_src') / fname
            dst = code_base / rel
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src, dst)
            rel_paths.append(str(rel))

        # Init compressor
        comp = LogCompressor()
        comp.code_base_path = code_base
        comp.build_dir = build_dir
        comp.bt_compressed_srcs_path = compressed_srcs
        comp.load_config(str(config_path), ['BLE_HOST'])

        db_path = build_dir / comp.config.get('db_path', 'ble_log/ble_log_database')
        db = LogDBManager(
            data_dir=str(db_path),
            sources={s: str(c) for s, c in comp.module_info.items()},
        )
        comp.db_manager = db

        # Run compression
        comp.prepare_source_files(rel_paths)
        files_to_process = []
        for module, info in comp.module_info.items():
            files_to_process.extend([(module, p) for p in info['files_to_process']])
        files_to_process.sort(key=lambda x: x[1])

        all_macros: dict[str, list[tuple[int, str]]] = {}
        for fi in files_to_process:
            for module, log_id, macro in comp.compress_file(fi):
                all_macros.setdefault(module, []).append((log_id, macro))

        for module, macros in all_macros.items():
            comp.generate_log_index_header(module, macros)

        db.close()

        # Copy generated header to expected
        header = build_dir / 'ble_log' / 'include' / 'test_log_index.h'
        if header.exists():
            dest = EXPECTED_DIR / output_name
            shutil.copy2(header, dest)
            print(f'Generated: {dest}')
        else:
            print(f'No header generated for {fixture_files}')


def main() -> None:
    EXPECTED_DIR.mkdir(parents=True, exist_ok=True)

    scenarios = [
        (['simple_logs.c'], 'simple_logs_index.h'),
        (['format_specifiers.c'], 'format_specifiers_index.h'),
        (['special_tokens.c'], 'special_tokens_index.h'),
        (['multi_level.c'], 'multi_level_index.h'),
        (['simple_logs.c', 'multi_level.c'], 'multi_file_index.h'),
    ]

    for files, output in scenarios:
        run_pipeline(files, output)

    print(f'\nAll golden files generated in {EXPECTED_DIR}')
    print('Review the output for correctness before committing.')


if __name__ == '__main__':
    main()

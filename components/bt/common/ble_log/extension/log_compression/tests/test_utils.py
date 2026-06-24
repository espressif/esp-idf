# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Shared utilities for BLE log compression tests (unittest-based, no third-party deps)."""

from __future__ import annotations

import difflib
import os
import re
import shutil
import sys
import unittest
from pathlib import Path
from typing import Any

# Add scripts/ to sys.path so test modules can import the production code
SCRIPTS_DIR = Path(__file__).resolve().parent.parent / 'scripts'
if str(SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_DIR))

FIXTURE_DIR = Path(__file__).resolve().parent / 'fixtures'
BLUEDROID_SCRIPT = SCRIPTS_DIR / 'module_scripts' / 'bluedroid' / 'make_bluedroid_log_macro.py'
MESH_SCRIPT = SCRIPTS_DIR / 'module_scripts' / 'ble_mesh' / 'make_mesh_log_macro.py'

BLUEDROID_TAGS = [
    'APPL_TRACE_ERROR',
    'APPL_TRACE_WARNING',
    'APPL_TRACE_API',
    'APPL_TRACE_DEBUG',
    'APPL_TRACE_EVENT',
    'APPL_TRACE_VERBOSE',
]

MESH_TAGS = [
    'BT_ERR',
    'BT_WARN',
    'BT_INFO',
    'BT_DBG',
    'NET_BUF_ERR',
    'NET_BUF_WARN',
    'NET_BUF_DBG',
]


def make_db_manager(tmp_dir: str) -> Any:
    """Create a fresh LogDBManager in tmp_dir."""
    from LogDBManager import LogDBManager

    db_path = os.path.join(tmp_dir, 'db')
    return LogDBManager(data_dir=db_path, sources={'BLE_HOST': 'test_config_v1'})


def make_compressor() -> Any:
    """Create a LogCompressor with parser initialized."""
    from ble_log_compress import LogCompressor

    c = LogCompressor()
    c.init_parser()
    return c


def write_yaml_config(
    tmp_dir: str,
    tags: list[str],
    script_path: str | Path,
    module_name: str = 'BLE_HOST',
    log_index_file: str = 'test_log_index.h',
    tags_with_preserve: list[str] | None = None,
) -> Path:
    """Write a module_info.yml and return its path."""
    import yaml

    tags_with_preserve = tags_with_preserve or []
    config = {
        'log_config': {
            'db_path': 'ble_log/ble_log_database',
            'modules': {
                module_name: {
                    'description': f'{module_name} Test',
                    'code_path': ['test_src'],
                    'log_index_file': log_index_file,
                    'script': str(script_path),
                    'tags': tags,
                    'tags_with_preserve': tags_with_preserve,
                }
            },
        }
    }
    config_dir = Path(tmp_dir) / 'build' / 'ble_log'
    config_dir.mkdir(parents=True, exist_ok=True)
    config_path = config_dir / 'module_info.yml'
    with open(config_path, 'w') as f:
        yaml.safe_dump(config, f)
    return config_path


class PipelineContext:
    """Full pipeline context: directory structure + config + DB + compressor."""

    def __init__(self, tmp_dir: str) -> None:
        from ble_log_compress import LogCompressor
        from LogDBManager import LogDBManager

        self.tmp_dir = Path(tmp_dir)
        self.code_base = self.tmp_dir / 'code_base'
        self.build_dir = self.tmp_dir / 'build'
        self.compressed_srcs = self.build_dir / 'ble_log' / '.compressed_srcs'

        (self.build_dir / 'ble_log' / 'include').mkdir(parents=True, exist_ok=True)
        self.compressed_srcs.mkdir(parents=True, exist_ok=True)
        self.code_base.mkdir(parents=True, exist_ok=True)

        self.config_path = write_yaml_config(
            tmp_dir,
            tags=BLUEDROID_TAGS,
            script_path=BLUEDROID_SCRIPT,
            tags_with_preserve=['APPL_TRACE_ERROR', 'APPL_TRACE_WARNING'],
        )

        self.compressor = LogCompressor()
        self.compressor.code_base_path = self.code_base
        self.compressor.build_dir = self.build_dir
        self.compressor.bt_compressed_srcs_path = self.compressed_srcs
        self.compressor.load_config(str(self.config_path), ['BLE_HOST'])

        db_path = self.build_dir / self.compressor.config.get('db_path', 'ble_log/ble_log_database')
        self.db_manager = LogDBManager(
            data_dir=str(db_path),
            sources={s: str(c) for s, c in self.compressor.module_info.items()},
        )
        self.compressor.db_manager = self.db_manager
        self.module = 'BLE_HOST'

    def copy_fixture(self, filename: str) -> str:
        """Copy a fixture C file into the code base and return its relative path."""
        src = FIXTURE_DIR / 'c_sources' / filename
        rel_path = Path('test_src') / filename
        dst = self.code_base / rel_path
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)
        return str(rel_path)

    def run_compression(self, src_list: list[str]) -> dict[str, list[tuple[int, str]]]:
        """Run prepare + compress + header generation. Returns generated macros."""
        self.compressor.prepare_source_files(src_list)

        files_to_process = []
        for module, info in self.compressor.module_info.items():
            files_to_process.extend([(module, path) for path in info['files_to_process']])
        files_to_process.sort(key=lambda x: x[1])

        all_macros: dict[str, list[tuple[int, str]]] = {}
        for file_info in files_to_process:
            file_macros = self.compressor.compress_file(file_info)
            for module, log_id, macro in file_macros:
                all_macros.setdefault(module, []).append((log_id, macro))

        for module, macros in all_macros.items():
            self.compressor.generate_log_index_header(module, macros)

        # Mark files as processed
        for module, info in self.compressor.module_info.items():
            for temp_path in info['files_to_process']:
                src_path = self.code_base / os.path.relpath(temp_path[:-4], self.compressed_srcs)
                self.db_manager.mark_file_processed(module, src_path, temp_path)

        # Copy .tmp to final
        for root, _, files in os.walk(self.compressed_srcs):
            for name in files:
                if name.endswith('.tmp'):
                    file_src = os.path.join(root, name)
                    dst_path = os.path.join(root, name[:-4])
                    shutil.copy2(file_src, dst_path)

        self.db_manager.save_all()
        return all_macros

    @property
    def header_path(self) -> Path:
        return self.build_dir / 'ble_log' / 'include' / 'test_log_index.h'

    def close(self) -> None:
        self.db_manager.close()


def assert_header_matches_golden(
    test_case: unittest.TestCase,
    generated_path: str | Path,
    golden_path: str | Path,
) -> None:
    """Compare generated header to golden file, normalizing copyright year."""
    gen_text = Path(generated_path).read_text()
    gold_text = Path(golden_path).read_text()
    gen_norm = re.sub(r'20\d{2}', 'YYYY', gen_text)
    gold_norm = re.sub(r'20\d{2}', 'YYYY', gold_text)
    if gen_norm != gold_norm:
        diff = difflib.unified_diff(
            gold_norm.splitlines(keepends=True),
            gen_norm.splitlines(keepends=True),
            fromfile='expected (golden)',
            tofile='generated',
        )
        test_case.fail(f'Header mismatch:\n{"".join(diff)}')

# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Tests for incremental compression: caching, re-runs, config changes."""

import shutil
import tempfile
import unittest

import test_utils  # noqa: F401 — must be first to set up sys.path
import yaml
from LogDBManager import LogDBManager
from test_utils import PipelineContext


class _IncrementalTestCase(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.mkdtemp()
        self.ctx = PipelineContext(self.tmp)

    def tearDown(self) -> None:
        self.ctx.close()
        shutil.rmtree(self.tmp, ignore_errors=True)


class TestFirstRun(_IncrementalTestCase):
    def test_all_files_compressed(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        macros = self.ctx.run_compression([rel])
        self.assertIn('BLE_HOST', macros)
        self.assertGreater(len(macros['BLE_HOST']), 0)

    def test_header_generated(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        self.ctx.run_compression([rel])
        self.assertTrue(self.ctx.header_path.exists())


class TestRerunUnchanged(_IncrementalTestCase):
    def test_file_hashes_tracked(self) -> None:
        """After first run, is_file_processed returns True for unchanged files."""
        rel = self.ctx.copy_fixture('simple_logs.c')
        self.ctx.run_compression([rel])

        # Simulate new build: close and reopen DB
        self.ctx.db_manager.close()
        db_path = self.ctx.build_dir / self.ctx.compressor.config.get('db_path', 'ble_log/ble_log_database')
        self.ctx.db_manager = LogDBManager(
            data_dir=str(db_path),
            sources={s: str(c) for s, c in self.ctx.compressor.module_info.items()},
        )
        self.ctx.compressor.db_manager = self.ctx.db_manager

        src_path = self.ctx.code_base / rel
        temp_path = str(self.ctx.compressed_srcs / rel) + '.tmp'
        self.assertTrue(self.ctx.db_manager.is_file_processed('BLE_HOST', src_path, temp_path))

    def test_log_ids_unchanged_on_rerun(self) -> None:
        """Re-running compression on the same file produces same IDs."""
        rel = self.ctx.copy_fixture('simple_logs.c')
        macros1 = self.ctx.run_compression([rel])
        ids1 = sorted(log_id for log_id, _ in macros1['BLE_HOST'])

        # Re-run: same file, same content — macros for already-existing logs
        macros2 = self.ctx.run_compression([rel])
        ids2 = sorted(log_id for log_id, _ in macros2['BLE_HOST'])
        self.assertEqual(ids1, ids2)


class TestRerunModifiedFile(_IncrementalTestCase):
    def test_modified_file_reprocessed(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        self.ctx.run_compression([rel])

        # Modify the source file
        src_path = self.ctx.code_base / rel
        content = src_path.read_text()
        content += '\nvoid new_func(int v) { APPL_TRACE_DEBUG("new %d", v); }\n'
        src_path.write_text(content)

        macros = self.ctx.run_compression([rel])
        self.assertIn('BLE_HOST', macros)
        self.assertGreater(len(macros['BLE_HOST']), 0)

    def test_existing_ids_stable(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        macros1 = self.ctx.run_compression([rel])
        ids1 = sorted(log_id for log_id, _ in macros1['BLE_HOST'])

        # Modify source: add a new log
        src_path = self.ctx.code_base / rel
        content = src_path.read_text()
        content += '\nvoid extra(int v) { APPL_TRACE_DEBUG("extra %d", v); }\n'
        src_path.write_text(content)

        # Re-run — original IDs should still appear
        macros2 = self.ctx.run_compression([rel])
        ids2 = sorted(log_id for log_id, _ in macros2['BLE_HOST'])
        for old_id in ids1:
            self.assertIn(old_id, ids2)


class TestAddNewFile(_IncrementalTestCase):
    def test_new_file_ids_continue(self) -> None:
        rel1 = self.ctx.copy_fixture('simple_logs.c')
        macros1 = self.ctx.run_compression([rel1])
        ids1 = set(log_id for log_id, _ in macros1['BLE_HOST'])
        max_id_1 = max(ids1)

        # Simulate new build
        self.ctx.db_manager.close()
        db_path = self.ctx.build_dir / self.ctx.compressor.config.get('db_path', 'ble_log/ble_log_database')
        self.ctx.db_manager = LogDBManager(
            data_dir=str(db_path),
            sources={s: str(c) for s, c in self.ctx.compressor.module_info.items()},
        )
        self.ctx.compressor.db_manager = self.ctx.db_manager

        rel2 = self.ctx.copy_fixture('multi_level.c')
        macros2 = self.ctx.run_compression([rel1, rel2])

        if 'BLE_HOST' in macros2 and macros2['BLE_HOST']:
            all_ids_2 = set(log_id for log_id, _ in macros2['BLE_HOST'])
            new_ids = all_ids_2 - ids1
            self.assertGreater(len(new_ids), 0)
            for new_id in new_ids:
                self.assertGreater(new_id, max_id_1)


class TestConfigChange(_IncrementalTestCase):
    def test_config_change_triggers_full_regen(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        self.ctx.run_compression([rel])

        # Change config
        with open(self.ctx.config_path) as f:
            config = yaml.safe_load(f)
        config['log_config']['modules']['BLE_HOST']['tags'].append('NEW_TAG_ADDED')
        with open(self.ctx.config_path, 'w') as f:
            yaml.safe_dump(config, f)

        # Reload
        self.ctx.compressor.module_info.clear()
        self.ctx.compressor.module_mod.clear()
        self.ctx.compressor.load_config(str(self.ctx.config_path), ['BLE_HOST'])

        self.ctx.db_manager.close()
        db_path = self.ctx.build_dir / self.ctx.compressor.config.get('db_path', 'ble_log/ble_log_database')
        self.ctx.db_manager = LogDBManager(
            data_dir=str(db_path),
            sources={s: str(c) for s, c in self.ctx.compressor.module_info.items()},
        )
        self.ctx.compressor.db_manager = self.ctx.db_manager

        self.assertTrue(self.ctx.db_manager.is_config_updated('BLE_HOST'))
        self.assertEqual(
            self.ctx.db_manager.source_update_state('BLE_HOST'),
            LogDBManager.SOURCE_LOG_UPDATE_FULL,
        )


if __name__ == '__main__':
    unittest.main()

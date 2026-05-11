# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Tests for LogDBManager: state persistence, incremental logic, file tracking."""

import shutil
import tempfile
import unittest

import test_utils  # noqa: F401 — must be first to set up sys.path
from LogDBManager import LogDBManager


class TestAddLog(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.mkdtemp()
        self.db = LogDBManager(data_dir=f'{self.tmp}/db', sources={'BLE_HOST': 'test_config_v1'})

    def tearDown(self) -> None:
        self.db.close()
        shutil.rmtree(self.tmp, ignore_errors=True)

    def test_add_new_returns_success(self) -> None:
        result, log_id = self.db.add_log(
            source='BLE_HOST',
            log_tag='APPL_TRACE_DEBUG',
            log_format='"message %d"',
            log_line_number=10,
            hexify=True,
            caller_func='foo',
            caller_line=5,
            file_name='test.c',
        )
        self.assertEqual(result, LogDBManager.SUCCESS)
        self.assertEqual(log_id, 1)

    def test_add_duplicate_returns_exists(self) -> None:
        kw = dict(
            source='BLE_HOST',
            log_tag='APPL_TRACE_DEBUG',
            log_format='"message %d"',
            log_line_number=10,
            hexify=True,
            caller_func='foo',
            caller_line=5,
            file_name='test.c',
        )
        self.db.add_log(**kw)
        result, log_id = self.db.add_log(**kw)
        self.assertEqual(result, LogDBManager.LOG_EXISTS)
        self.assertEqual(log_id, 1)

    def test_sequential_ids(self) -> None:
        _, id1 = self.db.add_log(
            source='BLE_HOST',
            log_tag='T',
            log_format='"m1"',
            log_line_number=10,
            hexify=True,
            caller_func='foo',
            caller_line=5,
            file_name='test.c',
        )
        _, id2 = self.db.add_log(
            source='BLE_HOST',
            log_tag='T',
            log_format='"m2"',
            log_line_number=20,
            hexify=True,
            caller_func='bar',
            caller_line=15,
            file_name='test.c',
        )
        self.assertEqual(id1, 1)
        self.assertEqual(id2, 2)

    def test_unique_key_distinguishes_all_fields(self) -> None:
        _, id1 = self.db.add_log(
            source='BLE_HOST',
            log_tag='T',
            log_format='"msg %d"',
            log_line_number=10,
            hexify=True,
            caller_func='foo',
            caller_line=5,
            file_name='a.c',
        )
        _, id2 = self.db.add_log(
            source='BLE_HOST',
            log_tag='T',
            log_format='"msg %d"',
            log_line_number=20,
            hexify=True,
            caller_func='bar',
            caller_line=15,
            file_name='b.c',
        )
        self.assertNotEqual(id1, id2)


class TestPersistence(unittest.TestCase):
    def test_save_and_reload(self) -> None:
        tmp = tempfile.mkdtemp()
        try:
            db_path = f'{tmp}/db'
            sources = {'BLE_HOST': 'cfg_v1'}

            mgr1 = LogDBManager(data_dir=db_path, sources=sources)
            mgr1.add_log(
                source='BLE_HOST',
                log_tag='APPL_TRACE_ERROR',
                log_format='"error %d"',
                log_line_number=10,
                hexify=True,
                caller_func='func1',
                caller_line=5,
                file_name='f.c',
            )
            mgr1.save_all()
            mgr1.close()

            mgr2 = LogDBManager(data_dir=db_path, sources=sources)
            result, log_id = mgr2.add_log(
                source='BLE_HOST',
                log_tag='APPL_TRACE_ERROR',
                log_format='"error %d"',
                log_line_number=10,
                hexify=True,
                caller_func='func1',
                caller_line=5,
                file_name='f.c',
            )
            self.assertEqual(result, LogDBManager.LOG_EXISTS)
            self.assertEqual(log_id, 1)
            mgr2.close()
        finally:
            shutil.rmtree(tmp, ignore_errors=True)


class TestFileProcessing(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.mkdtemp()
        self.db = LogDBManager(data_dir=f'{self.tmp}/db', sources={'BLE_HOST': 'cfg_v1'})

    def tearDown(self) -> None:
        self.db.close()
        shutil.rmtree(self.tmp, ignore_errors=True)

    def _write(self, name: str, content: str) -> str:
        import os

        p = os.path.join(self.tmp, name)
        with open(p, 'w') as f:
            f.write(content)
        return p

    def test_new_file_not_processed(self) -> None:
        src = self._write('src.c', 'void foo(void) {}')
        comp = self._write('src.c.tmp', 'void foo(void) {}')
        self.assertFalse(self.db.is_file_processed('BLE_HOST', src, comp))

    def test_marked_file_is_processed(self) -> None:
        src = self._write('src.c', 'void foo(void) {}')
        comp = self._write('src.c.tmp', 'void foo_compressed(void) {}')
        self.db.mark_file_processed('BLE_HOST', src, comp)
        self.db.save_all()
        self.assertTrue(self.db.is_file_processed('BLE_HOST', src, comp))

    def test_modified_file_not_processed(self) -> None:
        src = self._write('src.c', 'void foo(void) {}')
        comp = self._write('src.c.tmp', 'void foo_compressed(void) {}')
        self.db.mark_file_processed('BLE_HOST', src, comp)
        self.db.save_all()
        # Modify source
        with open(src, 'w') as f:
            f.write('void foo(void) { /* changed */ }')
        self.assertFalse(self.db.is_file_processed('BLE_HOST', src, comp))


class TestConfigUpdate(unittest.TestCase):
    def test_fresh_db_config_is_updated(self) -> None:
        tmp = tempfile.mkdtemp()
        try:
            db = LogDBManager(data_dir=f'{tmp}/db', sources={'BLE_HOST': 'cfg_v1'})
            self.assertTrue(db.is_config_updated('BLE_HOST'))
            db.close()
        finally:
            shutil.rmtree(tmp, ignore_errors=True)

    def test_same_config_not_updated_after_save_reload(self) -> None:
        tmp = tempfile.mkdtemp()
        try:
            sources = {'BLE_HOST': 'cfg_v1'}
            mgr1 = LogDBManager(data_dir=f'{tmp}/db', sources=sources)
            mgr1.add_log(
                source='BLE_HOST',
                log_tag='T',
                log_format='"m"',
                log_line_number=1,
                hexify=True,
                caller_func='f',
                caller_line=1,
                file_name='x.c',
            )
            mgr1.save_all()
            mgr1.close()

            mgr2 = LogDBManager(data_dir=f'{tmp}/db', sources=sources)
            self.assertFalse(mgr2.is_config_updated('BLE_HOST'))
            mgr2.close()
        finally:
            shutil.rmtree(tmp, ignore_errors=True)

    def test_different_config_is_updated(self) -> None:
        tmp = tempfile.mkdtemp()
        try:
            mgr1 = LogDBManager(data_dir=f'{tmp}/db', sources={'BLE_HOST': 'cfg_v1'})
            mgr1.add_log(
                source='BLE_HOST',
                log_tag='T',
                log_format='"m"',
                log_line_number=1,
                hexify=True,
                caller_func='f',
                caller_line=1,
                file_name='x.c',
            )
            mgr1.save_all()
            mgr1.close()

            mgr2 = LogDBManager(data_dir=f'{tmp}/db', sources={'BLE_HOST': 'cfg_v2_changed'})
            self.assertTrue(mgr2.is_config_updated('BLE_HOST'))
            mgr2.close()
        finally:
            shutil.rmtree(tmp, ignore_errors=True)


class TestSourceUpdateState(unittest.TestCase):
    def test_fresh_db_returns_full(self) -> None:
        tmp = tempfile.mkdtemp()
        try:
            db = LogDBManager(data_dir=f'{tmp}/db', sources={'BLE_HOST': 'cfg_v1'})
            self.assertEqual(db.source_update_state('BLE_HOST'), LogDBManager.SOURCE_LOG_UPDATE_FULL)
            db.close()
        finally:
            shutil.rmtree(tmp, ignore_errors=True)

    def test_loaded_with_logs_no_new_returns_none(self) -> None:
        tmp = tempfile.mkdtemp()
        try:
            sources = {'BLE_HOST': 'cfg_v1'}
            mgr1 = LogDBManager(data_dir=f'{tmp}/db', sources=sources)
            mgr1.add_log(
                source='BLE_HOST',
                log_tag='T',
                log_format='"m"',
                log_line_number=1,
                hexify=True,
                caller_func='f',
                caller_line=1,
                file_name='x.c',
            )
            mgr1.save_all()
            mgr1.close()

            mgr2 = LogDBManager(data_dir=f'{tmp}/db', sources=sources)
            self.assertEqual(mgr2.source_update_state('BLE_HOST'), LogDBManager.SOURCE_LOG_UPDATE_NONE)
            mgr2.close()
        finally:
            shutil.rmtree(tmp, ignore_errors=True)

    def test_new_log_added_returns_partial(self) -> None:
        tmp = tempfile.mkdtemp()
        try:
            sources = {'BLE_HOST': 'cfg_v1'}
            mgr1 = LogDBManager(data_dir=f'{tmp}/db', sources=sources)
            mgr1.add_log(
                source='BLE_HOST',
                log_tag='T',
                log_format='"m1"',
                log_line_number=1,
                hexify=True,
                caller_func='f',
                caller_line=1,
                file_name='x.c',
            )
            mgr1.save_all()
            mgr1.close()

            mgr2 = LogDBManager(data_dir=f'{tmp}/db', sources=sources)
            mgr2.add_log(
                source='BLE_HOST',
                log_tag='T',
                log_format='"m2"',
                log_line_number=2,
                hexify=True,
                caller_func='g',
                caller_line=2,
                file_name='x.c',
            )
            self.assertEqual(mgr2.source_update_state('BLE_HOST'), LogDBManager.SOURCE_LOG_UPDATE_PARTIAL)
            mgr2.close()
        finally:
            shutil.rmtree(tmp, ignore_errors=True)

    def test_config_change_returns_full(self) -> None:
        tmp = tempfile.mkdtemp()
        try:
            mgr1 = LogDBManager(data_dir=f'{tmp}/db', sources={'BLE_HOST': 'cfg_v1'})
            mgr1.add_log(
                source='BLE_HOST',
                log_tag='T',
                log_format='"m"',
                log_line_number=1,
                hexify=True,
                caller_func='f',
                caller_line=1,
                file_name='x.c',
            )
            mgr1.save_all()
            mgr1.close()

            mgr2 = LogDBManager(data_dir=f'{tmp}/db', sources={'BLE_HOST': 'cfg_v2'})
            self.assertEqual(mgr2.source_update_state('BLE_HOST'), LogDBManager.SOURCE_LOG_UPDATE_FULL)
            mgr2.close()
        finally:
            shutil.rmtree(tmp, ignore_errors=True)


if __name__ == '__main__':
    unittest.main()

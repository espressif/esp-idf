# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Tests for compressed macro generation (bluedroid and mesh module scripts)."""

import sys
import types
import unittest

from test_utils import SCRIPTS_DIR


class TestBluedroidMacroGen(unittest.TestCase):
    mod: types.ModuleType

    @classmethod
    def setUpClass(cls) -> None:
        p = str(SCRIPTS_DIR / 'module_scripts' / 'bluedroid')
        if p not in sys.path:
            sys.path.insert(0, p)
        import make_bluedroid_log_macro

        cls.mod = make_bluedroid_log_macro

    def test_header_head_has_declarations(self) -> None:
        head = self.mod.gen_header_head()
        self.assertIn('ble_log_compressed_hex_print', head)
        self.assertIn('ble_log_compressed_hex_print_buf', head)
        self.assertIn('extern', head)

    def test_zero_args_macro(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=1,
            module_id=0,
            func_name='APPL_TRACE_DEBUG',
            fmt=None,
            args=[],
            buffer_args=[],
        )
        self.assertIn('ble_log_compressed_hex_print(0,1, 0)', stmt)

    def test_single_arg_macro(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=2,
            module_id=0,
            func_name='APPL_TRACE_ERROR',
            fmt=None,
            args=[{'name': 'val', 'size_type': '0'}],
            buffer_args=[],
        )
        self.assertIn('ble_log_compressed_hex_print(0,2, 1, 0, val)', stmt)

    def test_multi_arg_with_sizes(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=3,
            module_id=0,
            func_name='APPL_TRACE_DEBUG',
            fmt=None,
            args=[
                {'name': 'a', 'size_type': '0'},
                {'name': 'b', 'size_type': '1'},
                {'name': 'c', 'size_type': '2'},
            ],
            buffer_args=[],
        )
        self.assertIn('3, 0, 1, 2, a, b, c', stmt)

    def test_appl_error_level_check(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=1,
            module_id=0,
            func_name='APPL_TRACE_ERROR',
            fmt=None,
            args=[],
            buffer_args=[],
        )
        self.assertIn('appl_trace_level >= BT_TRACE_LEVEL_ERROR', stmt)
        self.assertIn('BT_LOG_LEVEL_CHECK(APPL, ERROR)', stmt)

    def test_appl_debug_level_check(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=1,
            module_id=0,
            func_name='APPL_TRACE_DEBUG',
            fmt=None,
            args=[],
            buffer_args=[],
        )
        self.assertIn('appl_trace_level >= BT_TRACE_LEVEL_DEBUG', stmt)
        self.assertIn('BT_LOG_LEVEL_CHECK(APPL, DEBUG)', stmt)

    def test_buffer_args_generate_buf_print(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=5,
            module_id=0,
            func_name='APPL_TRACE_DEBUG',
            fmt=None,
            args=[],
            buffer_args=[{'buffer': 'bd_addr', 'length': '6'}],
        )
        self.assertIn('ble_log_compressed_hex_print_buf', stmt)
        self.assertIn('(const uint8_t *)bd_addr, 6', stmt)

    def test_multiple_buffer_args(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=6,
            module_id=0,
            func_name='APPL_TRACE_DEBUG',
            fmt=None,
            args=[],
            buffer_args=[
                {'buffer': 'buf1', 'length': '10'},
                {'buffer': 'buf2', 'length': '20'},
            ],
        )
        self.assertIn('(const uint8_t *)buf1', stmt)
        self.assertIn('(const uint8_t *)buf2', stmt)


class TestMeshMacroGen(unittest.TestCase):
    mod: types.ModuleType

    @classmethod
    def setUpClass(cls) -> None:
        p = str(SCRIPTS_DIR / 'module_scripts' / 'ble_mesh')
        if p not in sys.path:
            sys.path.insert(0, p)
        import make_mesh_log_macro

        cls.mod = make_mesh_log_macro

    def test_header_head_has_declarations(self) -> None:
        head = self.mod.gen_header_head()
        self.assertIn('ble_log_compressed_hex_print', head)

    def test_bt_err_level_check(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=1,
            module_id=1,
            func_name='BT_ERR',
            fmt=None,
            args=[],
            buffer_args=[],
        )
        self.assertIn('ERROR', stmt)

    def test_bt_dbg_level_check(self) -> None:
        stmt = self.mod.gen_compressed_stmt(
            log_index=2,
            module_id=1,
            func_name='BT_DBG',
            fmt=None,
            args=[],
            buffer_args=[],
        )
        self.assertIn('DEBUG', stmt)


if __name__ == '__main__':
    unittest.main()

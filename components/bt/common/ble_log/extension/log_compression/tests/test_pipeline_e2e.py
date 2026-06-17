# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""End-to-end pipeline tests: .c input -> .h output with golden file comparison."""

import re
import shutil
import tempfile
import unittest

from test_utils import FIXTURE_DIR
from test_utils import PipelineContext
from test_utils import assert_header_matches_golden


class _E2ETestCase(unittest.TestCase):
    """Base class with pipeline setUp/tearDown."""

    def setUp(self) -> None:
        self.tmp = tempfile.mkdtemp()
        self.ctx = PipelineContext(self.tmp)

    def tearDown(self) -> None:
        self.ctx.close()
        shutil.rmtree(self.tmp, ignore_errors=True)


class TestSimpleLogsE2E(_E2ETestCase):
    def test_compresses_and_generates_header(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        macros = self.ctx.run_compression([rel])
        self.assertIn('BLE_HOST', macros)
        self.assertEqual(len(macros['BLE_HOST']), 4)
        self.assertTrue(self.ctx.header_path.exists())

    def test_tag_replacement_in_source(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        self.ctx.run_compression([rel])
        tmp_file = self.ctx.compressed_srcs / 'test_src' / 'simple_logs.c.tmp'
        content = tmp_file.read_text()
        self.assertRegex(content, r'APPL_TRACE_DEBUG_\d+')
        self.assertRegex(content, r'APPL_TRACE_ERROR_\d+')

    def test_header_structure(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        self.ctx.run_compression([rel])
        header = self.ctx.header_path.read_text()
        self.assertIn('#ifndef __BLE_HOST_INTERNAL_LOG_INDEX_H', header)
        self.assertIn('#define __BLE_HOST_INTERNAL_LOG_INDEX_H', header)
        self.assertIn('#endif', header)
        self.assertIn('ble_log_compressed_hex_print', header)

    def test_macros_sorted_by_id(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        self.ctx.run_compression([rel])
        header = self.ctx.header_path.read_text()
        ids = [int(m) for m in re.findall(r'#define \w+_(\d+)\(fmt', header)]
        self.assertEqual(ids, sorted(ids))

    def test_preserve_tag_appends_original(self) -> None:
        rel = self.ctx.copy_fixture('simple_logs.c')
        self.ctx.run_compression([rel])
        header = self.ctx.header_path.read_text()
        self.assertIn('APPL_TRACE_ERROR(fmt, ##__VA_ARGS__)', header)
        self.assertIn('APPL_TRACE_WARNING(fmt, ##__VA_ARGS__)', header)
        self.assertNotIn('APPL_TRACE_DEBUG(fmt, ##__VA_ARGS__)', header)
        self.assertNotIn('APPL_TRACE_API(fmt, ##__VA_ARGS__)', header)


class TestFormatSpecifiersE2E(_E2ETestCase):
    def test_all_size_types_in_header(self) -> None:
        rel = self.ctx.copy_fixture('format_specifiers.c')
        self.ctx.run_compression([rel])
        header = self.ctx.header_path.read_text()
        self.assertTrue(', 0,' in header or ', 0, ' in header)  # U32
        self.assertTrue(', 1,' in header or ', 1, ' in header)  # STR
        self.assertTrue(', 2,' in header or ', 2, ' in header)  # U64


class TestSpecialTokensE2E(_E2ETestCase):
    def test_func_and_line_in_header(self) -> None:
        rel = self.ctx.copy_fixture('special_tokens.c')
        self.ctx.run_compression([rel])
        header = self.ctx.header_path.read_text()
        self.assertIn('ble_log_compressed_hex_print_buf', header)

    def test_func_macro_not_in_args(self) -> None:
        rel = self.ctx.copy_fixture('special_tokens.c')
        self.ctx.run_compression([rel])
        header = self.ctx.header_path.read_text()
        self.assertNotIn('__func__', header)


class TestNoLogsE2E(_E2ETestCase):
    def test_no_macros_generated(self) -> None:
        rel = self.ctx.copy_fixture('no_logs.c')
        macros = self.ctx.run_compression([rel])
        self.assertEqual(len(macros), 0)


class TestMultiLevelE2E(_E2ETestCase):
    def test_all_levels_get_unique_ids(self) -> None:
        rel = self.ctx.copy_fixture('multi_level.c')
        macros = self.ctx.run_compression([rel])
        ids = [log_id for log_id, _ in macros['BLE_HOST']]
        self.assertEqual(len(ids), len(set(ids)))

    def test_different_level_checks(self) -> None:
        rel = self.ctx.copy_fixture('multi_level.c')
        self.ctx.run_compression([rel])
        header = self.ctx.header_path.read_text()
        self.assertIn('BT_TRACE_LEVEL_ERROR', header)
        self.assertIn('BT_TRACE_LEVEL_WARNING', header)
        self.assertIn('BT_TRACE_LEVEL_API', header)
        self.assertIn('BT_TRACE_LEVEL_DEBUG', header)


class TestMultiFileE2E(_E2ETestCase):
    def test_ids_globally_unique(self) -> None:
        rel1 = self.ctx.copy_fixture('simple_logs.c')
        rel2 = self.ctx.copy_fixture('multi_level.c')
        macros = self.ctx.run_compression([rel1, rel2])
        all_ids = [log_id for log_id, _ in macros['BLE_HOST']]
        self.assertEqual(len(all_ids), len(set(all_ids)))
        self.assertEqual(len(all_ids), 10)


class TestConcatenatedStringsE2E(_E2ETestCase):
    def test_pri_macros_produce_u64(self) -> None:
        rel = self.ctx.copy_fixture('concatenated_strings.c')
        self.ctx.run_compression([rel])
        header = self.ctx.header_path.read_text()
        self.assertTrue(', 2,' in header or ', 2, ' in header)


class TestGoldenFileComparison(_E2ETestCase):
    def _run_and_compare(self, fixture_files: list, golden_name: str) -> None:
        rels = [self.ctx.copy_fixture(f) for f in fixture_files]
        self.ctx.run_compression(rels)
        golden = FIXTURE_DIR / 'expected' / golden_name
        if not golden.exists():
            self.skipTest(f'Golden file {golden_name} not found. Run update_golden.py first.')
        assert_header_matches_golden(self, self.ctx.header_path, golden)

    def test_simple_logs_golden(self) -> None:
        self._run_and_compare(['simple_logs.c'], 'simple_logs_index.h')

    def test_format_specifiers_golden(self) -> None:
        self._run_and_compare(['format_specifiers.c'], 'format_specifiers_index.h')

    def test_special_tokens_golden(self) -> None:
        self._run_and_compare(['special_tokens.c'], 'special_tokens_index.h')

    def test_multi_level_golden(self) -> None:
        self._run_and_compare(['multi_level.c'], 'multi_level_index.h')

    def test_multi_file_golden(self) -> None:
        self._run_and_compare(['simple_logs.c', 'multi_level.c'], 'multi_file_index.h')


if __name__ == '__main__':
    unittest.main()

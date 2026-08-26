# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Tests for argument size type determination in generate_compressed_macro.

Verifies that FormatToken.conv_char and FormatToken.length are used correctly
to determine ARG_SIZE_TYPE (the fix for the off-by-one bug).
"""

import re
import shutil
import tempfile
import unittest

import test_utils  # noqa: F401 — must be first to set up sys.path
from ble_log_compress import ARG_SIZE_TYPE
from test_utils import PipelineContext


def _extract_size_types_from_macro(macro_str: str) -> list[int] | None:
    """Extract size type values from a generated macro string."""
    m = re.search(r'ble_log_compressed_hex_print\(\d+,\d+, (\d+)(?:, (.+?))?(?:,\s*[a-zA-Z_])', macro_str)
    if not m:
        m = re.search(r'ble_log_compressed_hex_print\(\d+,\d+, 0\)', macro_str)
        if m:
            return []
        return None
    arg_count = int(m.group(1))
    if arg_count == 0:
        return []
    size_str = m.group(2)
    return [int(x.strip()) for x in size_str.split(',')[:arg_count]]


def _compress_and_get_macro(ctx: 'PipelineContext', c_code: bytes) -> str:
    """Compress a C code snippet and return the generated macro string."""
    src_dir = ctx.code_base / 'test_src'
    src_dir.mkdir(parents=True, exist_ok=True)
    (src_dir / 'test_size.c').write_bytes(c_code)

    tmp_dir = ctx.compressed_srcs / 'test_src'
    tmp_dir.mkdir(parents=True, exist_ok=True)
    (tmp_dir / 'test_size.c.tmp').write_bytes(c_code)

    file_macros = ctx.compressor.compress_file(('BLE_HOST', str(tmp_dir / 'test_size.c.tmp')))
    assert len(file_macros) > 0, 'No macros generated'
    result: str = file_macros[0][2]
    return result


class _PipelineTestCase(unittest.TestCase):
    """Base class that sets up and tears down a full pipeline context."""

    def setUp(self) -> None:
        self.tmp = tempfile.mkdtemp()
        self.ctx = PipelineContext(self.tmp)

    def tearDown(self) -> None:
        self.ctx.close()
        shutil.rmtree(self.tmp, ignore_errors=True)


class TestU32SizeType(_PipelineTestCase):
    def test_int_d(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("v %d", a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.U32)])

    def test_unsigned_u(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("v %u", a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.U32)])

    def test_hex_x(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("v %x", a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.U32)])


class TestU64SizeType(_PipelineTestCase):
    def test_float_f(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("v %f", a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.U64)])

    def test_long_long_d(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("v %lld", a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.U64)])

    def test_long_long_x(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("v %llx", a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.U64)])


class TestSTRSizeType(_PipelineTestCase):
    def test_string_s(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("v %s", a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.STR)])


class TestMixedSizeTypes(_PipelineTestCase):
    def test_d_s_lld_f(self) -> None:
        macro = _compress_and_get_macro(
            self.ctx, b'void f(void) { APPL_TRACE_DEBUG("a %d b %s c %lld d %f", i, s, ll, fv); }'
        )
        self.assertEqual(
            _extract_size_types_from_macro(macro),
            [
                int(ARG_SIZE_TYPE.U32),
                int(ARG_SIZE_TYPE.STR),
                int(ARG_SIZE_TYPE.U64),
                int(ARG_SIZE_TYPE.U64),
            ],
        )


class TestSpecialTokensSkipped(_PipelineTestCase):
    def test_func_excluded_from_args(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("%s val %d", __func__, a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.U32)])

    def test_line_excluded_from_args(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("line %d val %d", __LINE__, a); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [int(ARG_SIZE_TYPE.U32)])

    def test_zero_args_after_func(self) -> None:
        macro = _compress_and_get_macro(self.ctx, b'void f(void) { APPL_TRACE_DEBUG("%s starting", __func__); }')
        self.assertEqual(_extract_size_types_from_macro(macro), [])


class TestHexFuncSkipped(_PipelineTestCase):
    def test_mac2str_generates_buf_print(self) -> None:
        macro = _compress_and_get_macro(
            self.ctx, b'void f(void) { APPL_TRACE_DEBUG("addr=" MACSTR, MAC2STR(bd_addr)); }'
        )
        self.assertIn('ble_log_compressed_hex_print_buf', macro)
        self.assertIn('bd_addr', macro)


if __name__ == '__main__':
    unittest.main()

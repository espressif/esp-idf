# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Tests for LogCompressor.extract_log_calls: tree-sitter AST extraction."""

import unittest

import test_utils  # noqa: F401 — must be first to set up sys.path
from c_format_parse import FormatToken
from test_utils import make_compressor

TAGS = [
    'APPL_TRACE_ERROR',
    'APPL_TRACE_WARNING',
    'APPL_TRACE_API',
    'APPL_TRACE_DEBUG',
    'APPL_TRACE_EVENT',
    'APPL_TRACE_VERBOSE',
]


class TestExtractNoLogs(unittest.TestCase):
    def setUp(self) -> None:
        self.comp = make_compressor()

    def test_no_matching_tags(self) -> None:
        code = b'void foo(void) { printf("hello"); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(logs, [])

    def test_file_with_no_functions(self) -> None:
        code = b'int global_var = 42;'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(logs, [])


class TestExtractBasicLogs(unittest.TestCase):
    def setUp(self) -> None:
        self.comp = make_compressor()

    def test_single_log(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("msg %d", val); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 1)
        self.assertEqual(logs[0]['tag'][0], 'APPL_TRACE_DEBUG')
        self.assertEqual(logs[0]['caller_name'], 'foo')

    def test_no_arg_log(self) -> None:
        code = b'void bar(void) { APPL_TRACE_DEBUG("simple message"); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 1)
        tokens = [t for t in logs[0]['argu_tokens'] if isinstance(t, FormatToken)]
        self.assertEqual(len(tokens), 0)

    def test_multiple_tags(self) -> None:
        code = b'void foo(void) {\n    APPL_TRACE_DEBUG("debug");\n    APPL_TRACE_ERROR("error %d", code);\n}\n'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 2)
        self.assertEqual(logs[0]['tag'][0], 'APPL_TRACE_DEBUG')
        self.assertEqual(logs[1]['tag'][0], 'APPL_TRACE_ERROR')


class TestExtractFormatTokens(unittest.TestCase):
    def setUp(self) -> None:
        self.comp = make_compressor()

    def test_format_tokens_populated(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("val %d str %s", a, b); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        tokens = [t for t in logs[0]['argu_tokens'] if isinstance(t, FormatToken)]
        self.assertEqual(len(tokens), 2)
        self.assertEqual(tokens[0].conv_char, 'd')
        self.assertEqual(tokens[1].conv_char, 's')

    def test_hexify_true_for_normal_formats(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("val %d", a); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertTrue(logs[0]['hexify'])

    def test_hexify_false_for_dynamic_width(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("%*d", w, a); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 0)

    def test_hexify_false_for_dynamic_precision(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("%.*s", p, s); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 0)


class TestExtractSpecialTokens(unittest.TestCase):
    def setUp(self) -> None:
        self.comp = make_compressor()

    def test_func_macro_replaced(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("%s starting", __func__); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 1)
        tokens = [t for t in logs[0]['argu_tokens'] if isinstance(t, FormatToken)]
        self.assertTrue(any(t.conv_char == '@func' for t in tokens))

    def test_line_macro_replaced(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("at line %d", __LINE__); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 1)
        tokens = [t for t in logs[0]['argu_tokens'] if isinstance(t, FormatToken)]
        self.assertTrue(any(t.conv_char == '@line' for t in tokens))

    def test_mac2str_replaced(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("addr=" MACSTR, MAC2STR(bd_addr)); }'
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 1)
        tokens = [t for t in logs[0]['argu_tokens'] if isinstance(t, FormatToken)]
        hex_tokens = [t for t in tokens if t.conv_char.startswith('@hex_func')]
        self.assertEqual(len(hex_tokens), 1)
        self.assertIn('bd_addr', hex_tokens[0].conv_char)
        self.assertIn('6', hex_tokens[0].conv_char)


class TestExtractCallerInfo(unittest.TestCase):
    def setUp(self) -> None:
        self.comp = make_compressor()

    def test_correct_caller(self) -> None:
        code = (
            b'void first_func(void) {\n'
            b'    APPL_TRACE_DEBUG("in first");\n'
            b'}\n'
            b'void second_func(void) {\n'
            b'    APPL_TRACE_DEBUG("in second");\n'
            b'}\n'
        )
        logs = self.comp.extract_log_calls(code, TAGS)
        self.assertEqual(len(logs), 2)
        self.assertEqual(logs[0]['caller_name'], 'first_func')
        self.assertEqual(logs[1]['caller_name'], 'second_func')


class TestExtractArgMismatch(unittest.TestCase):
    def setUp(self) -> None:
        self.comp = make_compressor()

    def test_arg_count_mismatch_raises(self) -> None:
        code = b'void foo(void) { APPL_TRACE_DEBUG("msg %d %d", val); }'
        with self.assertRaises(SyntaxError):
            self.comp.extract_log_calls(code, TAGS)


if __name__ == '__main__':
    unittest.main()

# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Tests for c_format_parse.py: FormatToken NamedTuple and parse_format_string."""

import unittest

import test_utils  # noqa: F401 — must be first to set up sys.path
from c_format_parse import FormatToken
from c_format_parse import parse_format_string


class TestFormatTokenStructure(unittest.TestCase):
    def test_is_tuple_subclass(self) -> None:
        tokens = parse_format_string('%d')
        self.assertIsInstance(tokens[0], tuple)
        self.assertIsInstance(tokens[0], FormatToken)

    def test_field_names(self) -> None:
        self.assertEqual(
            FormatToken._fields,
            ('start', 'end', 'full_spec', 'flags', 'width', 'precision', 'length', 'conv_char'),
        )

    def test_named_access_matches_index(self) -> None:
        ft = parse_format_string('%08llx')[0]
        self.assertEqual(ft.start, ft[0])
        self.assertEqual(ft.end, ft[1])
        self.assertEqual(ft.full_spec, ft[2])
        self.assertEqual(ft.flags, ft[3])
        self.assertEqual(ft.width, ft[4])
        self.assertEqual(ft.precision, ft[5])
        self.assertEqual(ft.length, ft[6])
        self.assertEqual(ft.conv_char, ft[7])

    def test_complex_token_fields(self) -> None:
        ft = parse_format_string('%08llx')[0]
        self.assertEqual(ft.flags, '0')
        self.assertEqual(ft.width, '8')
        self.assertEqual(ft.precision, '')
        self.assertEqual(ft.length, 'll')
        self.assertEqual(ft.conv_char, 'x')
        self.assertEqual(ft.full_spec, '%08llx')


class TestBasicSpecifiers(unittest.TestCase):
    def test_single_specifiers(self) -> None:
        cases = [
            ('%d', 'd'),
            ('%i', 'i'),
            ('%u', 'u'),
            ('%x', 'x'),
            ('%X', 'X'),
            ('%o', 'o'),
            ('%c', 'c'),
            ('%s', 's'),
            ('%p', 'p'),
            ('%f', 'f'),
            ('%F', 'F'),
            ('%e', 'e'),
            ('%E', 'E'),
            ('%g', 'g'),
            ('%G', 'G'),
            ('%a', 'a'),
            ('%A', 'A'),
            ('%n', 'n'),
        ]
        for fmt, conv in cases:
            with self.subTest(fmt=fmt):
                tokens = parse_format_string(fmt)
                self.assertEqual(len(tokens), 1)
                self.assertIsInstance(tokens[0], FormatToken)
                self.assertEqual(tokens[0].conv_char, conv)

    def test_escaped_percent(self) -> None:
        tokens = parse_format_string('100%%')
        self.assertTrue(all(isinstance(t, str) for t in tokens))

    def test_empty_string(self) -> None:
        self.assertEqual(parse_format_string(''), [])

    def test_no_specifiers(self) -> None:
        tokens = parse_format_string('hello world')
        self.assertEqual(len(tokens), 1)
        self.assertEqual(tokens[0], 'hello world')


class TestPositionTracking(unittest.TestCase):
    def test_start_end_single(self) -> None:
        ft = parse_format_string('%d')[0]
        self.assertEqual(ft.start, 0)
        self.assertEqual(ft.end, 2)

    def test_start_end_with_prefix(self) -> None:
        tokens = parse_format_string('abc%dxyz')
        ft = [t for t in tokens if isinstance(t, FormatToken)][0]
        self.assertEqual(ft.start, 3)
        self.assertEqual(ft.end, 5)

    def test_multiple_positions(self) -> None:
        tokens = parse_format_string('%d %s')
        fts = [t for t in tokens if isinstance(t, FormatToken)]
        self.assertEqual(fts[0].start, 0)
        self.assertEqual(fts[0].end, 2)
        self.assertEqual(fts[1].start, 3)
        self.assertEqual(fts[1].end, 5)


class TestLengthModifiers(unittest.TestCase):
    def test_length_modifiers(self) -> None:
        cases = [
            ('%hd', 'h', 'd'),
            ('%hhd', 'hh', 'd'),
            ('%ld', 'l', 'd'),
            ('%lld', 'll', 'd'),
            ('%zu', 'z', 'u'),
            ('%jd', 'j', 'd'),
            ('%td', 't', 'd'),
            ('%llx', 'll', 'x'),
            ('%lx', 'l', 'x'),
            ('%hu', 'h', 'u'),
        ]
        for fmt, length, conv in cases:
            with self.subTest(fmt=fmt):
                ft = parse_format_string(fmt)[0]
                self.assertEqual(ft.length, length)
                self.assertEqual(ft.conv_char, conv)


class TestWidthAndPrecision(unittest.TestCase):
    def test_fixed_width(self) -> None:
        ft = parse_format_string('%10d')[0]
        self.assertEqual(ft.width, '10')
        self.assertEqual(ft.precision, '')

    def test_fixed_precision(self) -> None:
        ft = parse_format_string('%.2f')[0]
        self.assertEqual(ft.width, '')
        self.assertEqual(ft.precision, '2')

    def test_width_and_precision(self) -> None:
        ft = parse_format_string('%10.2f')[0]
        self.assertEqual(ft.width, '10')
        self.assertEqual(ft.precision, '2')

    def test_dynamic_width(self) -> None:
        ft = parse_format_string('%*d')[0]
        self.assertEqual(ft.width, '*')

    def test_dynamic_precision(self) -> None:
        ft = parse_format_string('%.*s')[0]
        self.assertEqual(ft.precision, '*')

    def test_both_dynamic(self) -> None:
        ft = parse_format_string('%*.*f')[0]
        self.assertEqual(ft.width, '*')
        self.assertEqual(ft.precision, '*')


class TestFlags(unittest.TestCase):
    def test_flags(self) -> None:
        cases = [
            ('%-d', '-'),
            ('%+d', '+'),
            ('% d', ' '),
            ('%#x', '#'),
            ('%0d', '0'),
            ('%08x', '0'),
        ]
        for fmt, expected_flags in cases:
            with self.subTest(fmt=fmt):
                ft = parse_format_string(fmt)[0]
                self.assertEqual(ft.flags, expected_flags)


class TestMixedContent(unittest.TestCase):
    def test_text_and_specifiers(self) -> None:
        tokens = parse_format_string('val=%d name=%s')
        fts = [t for t in tokens if isinstance(t, FormatToken)]
        strs = [t for t in tokens if isinstance(t, str)]
        self.assertEqual(len(fts), 2)
        self.assertEqual(fts[0].conv_char, 'd')
        self.assertEqual(fts[1].conv_char, 's')
        self.assertTrue(any('val=' in s for s in strs))

    def test_many_specifiers(self) -> None:
        tokens = parse_format_string('%d %s %lld %f %x')
        fts = [t for t in tokens if isinstance(t, FormatToken)]
        self.assertEqual(len(fts), 5)
        self.assertEqual([ft.conv_char for ft in fts], ['d', 's', 'd', 'f', 'x'])
        self.assertEqual(fts[2].length, 'll')

    def test_quoted_format_string(self) -> None:
        tokens = parse_format_string('"hello %d world"')
        fts = [t for t in tokens if isinstance(t, FormatToken)]
        self.assertEqual(len(fts), 1)
        self.assertEqual(fts[0].conv_char, 'd')


if __name__ == '__main__':
    unittest.main()

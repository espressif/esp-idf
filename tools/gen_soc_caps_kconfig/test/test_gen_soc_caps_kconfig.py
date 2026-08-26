#!/usr/bin/env python
# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import sys
import unittest

try:
    import gen_soc_caps_kconfig
except ImportError:
    sys.path.append('..')
    import gen_soc_caps_kconfig


class ParserTests(unittest.TestCase):
    def test_parse_name(self):
        res = gen_soc_caps_kconfig.parse_define('#define TEST_NAME (2)')
        self.assertEqual(res.name, 'TEST_NAME')

    def test_parse_int(self):
        res = gen_soc_caps_kconfig.parse_define('#define TEST_INT (2)')
        self.assertEqual(res.int_value, '2')

    def test_parse_string(self):
        res = gen_soc_caps_kconfig.parse_define('#define TEST_STRING "this is a test string"')
        self.assertEqual(res.str_value, 'this is a test string')

    def test_parse_hex(self):
        res = gen_soc_caps_kconfig.parse_define('#define TEST_HEX 0xDEADBEEF')
        self.assertEqual(res.hex_value, '0xDEADBEEF')

        res = gen_soc_caps_kconfig.parse_define('#define TEST_HEX 0x0000000')
        self.assertEqual(res.hex_value, '0x0000000')

    def test_parse_literal_suffix(self):
        res = gen_soc_caps_kconfig.parse_define('#define TEST (0x200UL)')
        self.assertEqual(res.hex_value, '0x200')

        res = gen_soc_caps_kconfig.parse_define('#define TEST_U 20000u')
        self.assertEqual(res.int_value, '20000')

        res = gen_soc_caps_kconfig.parse_define('#define TEST_U 329000LL')
        self.assertEqual(res.int_value, '329000')

    def test_parse_float(self):
        # Kconfig doesn't support floats, should not be parsed as anything
        res = gen_soc_caps_kconfig.parse_define('#define TEST_FLOAT (3.14)')
        self.assertEqual(res.value, '')

    def test_parse_ignore_pragma(self):
        res = gen_soc_caps_kconfig.parse_define('#define TEST_IGNORE // [gen_soc_caps:ignore]')
        self.assertEqual(' '.join(res.ignore_pragma), '// [gen_soc_caps:ignore]')

        res = gen_soc_caps_kconfig.parse_define('#define TEST_NO_IGNORE')
        self.assertEqual(res.ignore_pragma, '')


if __name__ == '__main__':
    print('Running gen_soc_caps_config tests...')
    unittest.main()

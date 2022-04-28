#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import os
import sys
import unittest

try:
    from ldgen.entity import Entity
    from ldgen.output_commands import AlignAtAddress, InputSectionDesc, SymbolAtAddress
except ImportError:
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))
    from ldgen.entity import Entity
    from ldgen.output_commands import AlignAtAddress, InputSectionDesc, SymbolAtAddress


SECTIONS = ['.text', '.text.*', '.literal', '.literal.*']

FREERTOS = Entity('libfreertos.a')
CROUTINE = Entity('libfreertos.a', 'croutine')


class InputSectionDescTest(unittest.TestCase):

    def test_catch_all_placement(self):
        # Test default (catch-all) command
        expected = '*(.literal .literal.* .text .text.*)'

        desc = InputSectionDesc(Entity(), SECTIONS)
        self.assertEqual(expected, str(desc))

        desc = InputSectionDesc(Entity(Entity.ALL), SECTIONS)
        self.assertEqual(expected, str(desc))

    def test_lib_placement(self):
        # Test library placement command
        expected = '*libfreertos.a:(.literal .literal.* .text .text.*)'

        desc = InputSectionDesc(Entity('libfreertos.a'), SECTIONS)
        self.assertEqual(expected, str(desc))

        desc = InputSectionDesc(Entity('libfreertos.a', Entity.ALL), SECTIONS)
        self.assertEqual(expected, str(desc))

        desc = InputSectionDesc(Entity('libfreertos.a', None, Entity.ALL), SECTIONS)
        self.assertEqual(expected, str(desc))

        desc = InputSectionDesc(Entity('libfreertos.a', Entity.ALL, Entity.ALL), SECTIONS)
        self.assertEqual(expected, str(desc))

    def test_obj_placement(self):
        # Test object placement command
        expected = '*libfreertos.a:croutine.*(.literal .literal.* .text .text.*)'

        desc = InputSectionDesc(Entity('libfreertos.a', 'croutine'), SECTIONS)
        self.assertEqual(expected, str(desc))

        desc = InputSectionDesc(Entity('libfreertos.a', 'croutine'), SECTIONS)
        self.assertEqual(expected, str(desc))

        desc = InputSectionDesc(Entity('libfreertos.a', 'croutine', Entity.ALL), SECTIONS)
        self.assertEqual(expected, str(desc))

        # Disambugated placement
        expected = '*libfreertos.a:croutine.c.*(.literal .literal.* .text .text.*)'

        desc = InputSectionDesc(Entity('libfreertos.a', 'croutine.c'), SECTIONS)
        self.assertEqual(expected, str(desc))

    def test_invalid_entity(self):
        # Invalid entity specification
        with self.assertRaises(AssertionError):
            InputSectionDesc(Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList'), SECTIONS)

        with self.assertRaises(AssertionError):
            InputSectionDesc(Entity('libfreertos.a', 'croutine'), SECTIONS, [Entity()])

        with self.assertRaises(AssertionError):
            InputSectionDesc(Entity('libfreertos.a', 'croutine'), SECTIONS, [Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList')])

    def test_exclusions(self):
        # Test exclusions

        # Library
        expected = ('*libfreertos.a:croutine.*'
                    '(EXCLUDE_FILE(*libfreertos.a) '
                    '.literal EXCLUDE_FILE(*libfreertos.a) '
                    '.literal.* EXCLUDE_FILE(*libfreertos.a) '
                    '.text EXCLUDE_FILE(*libfreertos.a) .text.*)')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [FREERTOS])
        self.assertEqual(expected, str(desc))

        # Object
        expected = ('*libfreertos.a:croutine.*'
                    '(EXCLUDE_FILE(*libfreertos.a:croutine.*) '
                    '.literal EXCLUDE_FILE(*libfreertos.a:croutine.*) '
                    '.literal.* EXCLUDE_FILE(*libfreertos.a:croutine.*) '
                    '.text EXCLUDE_FILE(*libfreertos.a:croutine.*) .text.*)')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [CROUTINE])
        self.assertEqual(expected, str(desc))

        # Multiple exclusions
        expected = ('*libfreertos.a:croutine.*'
                    '(EXCLUDE_FILE(*libfreertos.a *libfreertos.a:croutine.*) '
                    '.literal EXCLUDE_FILE(*libfreertos.a *libfreertos.a:croutine.*) '
                    '.literal.* EXCLUDE_FILE(*libfreertos.a *libfreertos.a:croutine.*) '
                    '.text EXCLUDE_FILE(*libfreertos.a *libfreertos.a:croutine.*) .text.*)')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [FREERTOS, CROUTINE])
        self.assertEqual(expected, str(desc))

        # Disambugated exclusion
        expected = ('*libfreertos.a:croutine.*'
                    '(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) '
                    '.literal EXCLUDE_FILE(*libfreertos.a:croutine.c.*) '
                    '.literal.* EXCLUDE_FILE(*libfreertos.a:croutine.c.*) '
                    '.text EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*)')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')])
        self.assertEqual(expected, str(desc))

    def test_empty_sections(self):
        # Test empty sections
        expected = '*libfreertos.a:croutine.*( )'

        desc = InputSectionDesc(Entity('libfreertos.a', 'croutine'), [])
        self.assertEqual(expected, str(desc))

    def test_keep(self):
        # Test KEEP
        expected = 'KEEP(*libfreertos.a:croutine.*( ))'

        desc = InputSectionDesc(Entity('libfreertos.a', 'croutine'), [], keep=True)
        self.assertEqual(expected, str(desc))

    def test_sort(self):
        # Test sort
        expected = ('*libfreertos.a:croutine.*('
                    'SORT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal) '
                    'SORT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal.*) '
                    'SORT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text) '
                    'SORT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*)'
                    ')')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')], keep=False, sort=(None, None))
        self.assertEqual(expected, str(desc))

        expected = ('*libfreertos.a:croutine.*('
                    'SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal) '
                    'SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal.*) '
                    'SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text) '
                    'SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*)'
                    ')')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')], keep=False, sort=('name', None))
        self.assertEqual(expected, str(desc))

        expected = ('*libfreertos.a:croutine.*('
                    'SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal) '
                    'SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal.*) '
                    'SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text) '
                    'SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*)'
                    ')')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')], keep=False, sort=('alignment', None))
        self.assertEqual(expected, str(desc))

        expected = ('*libfreertos.a:croutine.*('
                    'SORT_BY_INIT_PRIORITY(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal) '
                    'SORT_BY_INIT_PRIORITY(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal.*) '
                    'SORT_BY_INIT_PRIORITY(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text) '
                    'SORT_BY_INIT_PRIORITY(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*)'
                    ')')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')], keep=False, sort=('init_priority', None))
        self.assertEqual(expected, str(desc))

        expected = ('*libfreertos.a:croutine.*('
                    'SORT_BY_NAME(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal)) '
                    'SORT_BY_NAME(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal.*)) '
                    'SORT_BY_NAME(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text)) '
                    'SORT_BY_NAME(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*))'
                    ')')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')], keep=False, sort=('name', 'alignment'))
        self.assertEqual(expected, str(desc))

        expected = ('*libfreertos.a:croutine.*('
                    'SORT_BY_NAME(SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal)) '
                    'SORT_BY_NAME(SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal.*)) '
                    'SORT_BY_NAME(SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text)) '
                    'SORT_BY_NAME(SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*))'
                    ')')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')], keep=False, sort=('name', 'name'))
        self.assertEqual(expected, str(desc))

        expected = ('*libfreertos.a:croutine.*('
                    'SORT_BY_ALIGNMENT(SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal)) '
                    'SORT_BY_ALIGNMENT(SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal.*)) '
                    'SORT_BY_ALIGNMENT(SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text)) '
                    'SORT_BY_ALIGNMENT(SORT_BY_NAME(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*))'
                    ')')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')], keep=False, sort=('alignment', 'name'))
        self.assertEqual(expected, str(desc))

        expected = ('*libfreertos.a:croutine.*('
                    'SORT_BY_ALIGNMENT(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal)) '
                    'SORT_BY_ALIGNMENT(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .literal.*)) '
                    'SORT_BY_ALIGNMENT(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text)) '
                    'SORT_BY_ALIGNMENT(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*libfreertos.a:croutine.c.*) .text.*))'
                    ')')
        desc = InputSectionDesc(CROUTINE, SECTIONS, [Entity('libfreertos.a', 'croutine.c')], keep=False, sort=('alignment', 'alignment'))
        self.assertEqual(expected, str(desc))


class SymbolAtAddressTest(unittest.TestCase):

    def test_symbol(self):
        symbol = 'test_symbol'
        expected = '%s = ABSOLUTE(.);' % symbol

        desc = SymbolAtAddress(symbol)
        self.assertEqual(expected, str(desc))


class AlignAtAddressTest(unittest.TestCase):

    def test_align(self):
        align = 8
        expected = '. = ALIGN(%d);' % 8

        desc = AlignAtAddress(align)
        self.assertEqual(expected, str(desc))


if __name__ == '__main__':
    unittest.main()

#!/usr/bin/env python
#
# Copyright 2021 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import sys
import unittest

try:
    from output_commands import InputSectionDesc
except ImportError:
    sys.path.append('../')
    from output_commands import InputSectionDesc

from entity import Entity

SECTIONS = ['.text', '.text.*', '.literal', '.literal.*']

FREERTOS = Entity('libfreertos.a')
CROUTINE = Entity('libfreertos.a', 'croutine')


class InputSectionDescTest(unittest.TestCase):

    def test_output_00(self):
        # Test default (catch-all) command
        expected = '*(.literal .literal.* .text .text.*)'

        desc = InputSectionDesc(Entity(), SECTIONS)
        self.assertEqual(expected, str(desc))

        desc = InputSectionDesc(Entity(Entity.ALL), SECTIONS)
        self.assertEqual(expected, str(desc))

    def test_output_01(self):
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

    def test_output_02(self):
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

    def test_output_03(self):
        # Invalid entity specification
        with self.assertRaises(AssertionError):
            InputSectionDesc(Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList'), SECTIONS)

        with self.assertRaises(AssertionError):
            InputSectionDesc(Entity('libfreertos.a', 'croutine'), SECTIONS, [Entity()])

        with self.assertRaises(AssertionError):
            InputSectionDesc(Entity('libfreertos.a', 'croutine'), SECTIONS, [Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList')])

    def test_output_04(self):
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

    def test_output_05(self):
        # Test empty sections
        expected = '*libfreertos.a:croutine.*( )'

        desc = InputSectionDesc(Entity('libfreertos.a', 'croutine'), [])
        self.assertEqual(expected, str(desc))


if __name__ == '__main__':
    unittest.main()

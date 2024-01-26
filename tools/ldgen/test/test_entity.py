#!/usr/bin/env python
# coding=utf-8
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import os
import sys
import unittest

try:
    from ldgen.entity import Entity, EntityDB
except ImportError:
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))
    from ldgen.entity import Entity, EntityDB


class EntityTest(unittest.TestCase):

    def test_create_none(self):
        entity = Entity(Entity.ALL)
        self.assertEqual(Entity.Specificity.NONE, entity.specificity)

        entity = Entity(None)
        self.assertEqual(Entity.Specificity.NONE, entity.specificity)

        entity = Entity()
        self.assertEqual(Entity.Specificity.NONE, entity.specificity)

    def test_create_archive(self):
        entity = Entity('libfreertos.a')
        self.assertEqual(Entity.Specificity.ARCHIVE, entity.specificity)

        entity = Entity('libfreertos.a', Entity.ALL, Entity.ALL)
        self.assertEqual(Entity.Specificity.ARCHIVE, entity.specificity)

        entity = Entity('libfreertos.a', None, None)
        self.assertEqual(Entity.Specificity.ARCHIVE, entity.specificity)

        entity = Entity('libfreertos.a', Entity.ALL, None)
        self.assertEqual(Entity.Specificity.ARCHIVE, entity.specificity)

        entity = Entity('libfreertos.a', None, Entity.ALL)
        self.assertEqual(Entity.Specificity.ARCHIVE, entity.specificity)

    def test_create_obj(self):
        entity = Entity('libfreertos.a', 'croutine')
        self.assertEqual(Entity.Specificity.OBJ, entity.specificity)

        entity = Entity('libfreertos.a', 'croutine', Entity.ALL)
        self.assertEqual(Entity.Specificity.OBJ, entity.specificity)

        entity = Entity('libfreertos.a', 'croutine', None)
        self.assertEqual(Entity.Specificity.OBJ, entity.specificity)

    def test_create_symbol(self):
        entity = Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList')
        self.assertEqual(Entity.Specificity.SYMBOL, entity.specificity)

    def test_create_invalid(self):
        with self.assertRaises(ValueError):
            Entity(None, 'croutine')

        with self.assertRaises(ValueError):
            Entity(Entity.ALL, 'croutine')

        with self.assertRaises(ValueError):
            Entity(None, None, 'prvCheckPendingReadyList')

        with self.assertRaises(ValueError):
            Entity(Entity.ALL, Entity.ALL, 'prvCheckPendingReadyList')

        with self.assertRaises(ValueError):
            Entity(None, Entity.ALL, 'prvCheckPendingReadyList')

        with self.assertRaises(ValueError):
            Entity(Entity.ALL, None, 'prvCheckPendingReadyList')

        with self.assertRaises(ValueError):
            Entity('libfreertos.a', None, 'prvCheckPendingReadyList')

        with self.assertRaises(ValueError):
            Entity('libfreertos.a', Entity.ALL, 'prvCheckPendingReadyList')

    def test_compare_different_specificity(self):
        # Different specificity: NONE < ARCHIVE < OBJ < SYMBOL
        entity_a = Entity()
        entity_b = Entity('libfreertos.a')
        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a')
        entity_b = Entity('libfreertos.a', 'croutine')
        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', 'croutine')
        entity_b = Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList')
        self.assertLess(entity_a, entity_b)

        entity_a = Entity(Entity.ALL)
        entity_b = Entity('libfreertos.a')
        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', Entity.ALL)
        entity_b = Entity('libfreertos.a', 'croutine')
        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', 'croutine', Entity.ALL)
        entity_b = Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList')
        self.assertLess(entity_a, entity_b)

    def test_compare_equal(self):
        # Compare equal specificities and members
        entity_a = Entity()
        entity_b = Entity()
        self.assertEqual(entity_a, entity_b)

        entity_a = Entity('libfreertos.a')
        entity_b = Entity('libfreertos.a')
        self.assertEqual(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', 'croutine')
        entity_b = Entity('libfreertos.a', 'croutine')
        self.assertEqual(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList')
        entity_b = Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList')
        self.assertEqual(entity_a, entity_b)

    def test_compare_none_vs_all(self):
        # Two entities might have the same specifity whether
        # Entity.ALL is used or not specified; the latter is
        # considered less than the former.
        entity_a = Entity()
        entity_b = Entity(Entity.ALL)
        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a')
        entity_b = Entity('libfreertos.a', Entity.ALL, Entity.ALL)
        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', 'croutine')
        entity_b = Entity('libfreertos.a', 'croutine', Entity.ALL)
        self.assertLess(entity_a, entity_b)

    def test_compare_same_specificity(self):
        # Test that entities will be compared alphabetically
        # when the specificities are the same.
        entity_a = Entity('libfreertos_a.a')
        entity_b = Entity('libfreertos_b.a')
        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos_b.a', 'croutine_a')
        entity_b = Entity('libfreertos_a.a', 'croutine_b')
        self.assertLess(entity_b, entity_a)

        entity_a = Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList_a')
        entity_b = Entity('libfreertos.a', 'croutine', 'prvCheckPendingReadyList_b')
        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', 'croutine_b', 'prvCheckPendingReadyList_a')
        entity_b = Entity('libfreertos.a', 'croutine_a', 'prvCheckPendingReadyList_b')
        self.assertLess(entity_b, entity_a)

        entity_a = Entity('libfreertos_a.a', 'croutine_b', 'prvCheckPendingReadyList_a')
        entity_b = Entity('libfreertos_b.a', 'croutine_a', 'prvCheckPendingReadyList_b')
        self.assertLess(entity_a, entity_b)

    def test_compare_all_non_character(self):
        # Test that Entity.ALL is not treated as an
        # ordinary character in comparisons.
        entity_a = Entity(Entity.ALL)
        entity_b = Entity(chr(ord(Entity.ALL[0]) - 1))

        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', Entity.ALL)
        entity_b = Entity('libfreertos.a', chr(ord(Entity.ALL[0]) - 1))

        self.assertLess(entity_a, entity_b)

        entity_a = Entity('libfreertos.a', 'croutine', '*')
        entity_b = Entity('libfreertos.a', 'croutine', chr(ord(Entity.ALL[0]) - 1))

        self.assertLess(entity_a, entity_b)


class EntityDBTest(unittest.TestCase):

    def setUp(self):
        self.entities = EntityDB()

        with open('data/test_entity/libfreertos.a.txt') as objdump:
            self.entities.add_sections_info(objdump)

    def test_get_archives(self):
        archives = self.entities.get_archives()
        self.assertEqual(set(archives), set(['libfreertos.a']))

    def test_get_objs(self):
        objs = self.entities.get_objects('libfreertos.a')
        self.assertEqual(set(objs), set(['croutine.S.obj', 'croutine.c.obj', 'croutine.cpp.obj', 'timers.o']))

    def test_get_sections(self):
        # Needs disambugation between possible matches: croutine.S, croutine.c, croutine.cpp
        with self.assertRaises(ValueError):
            self.entities.get_sections('libfreertos.a', 'croutine')

        # Test disambugation works
        sections = self.entities.get_sections('libfreertos.a', 'croutine.c')
        expected = set(['.literal.prvCheckPendingReadyList', '.literal.prvCheckDelayedList'])
        self.assertEqual(set(sections), expected)

        sections = self.entities.get_sections('libfreertos.a', 'croutine.S')
        expected = set(['.debug_frame', '.debug_info', '.debug_abbrev'])
        self.assertEqual(set(sections), expected)

        # Test .o extension works
        sections = self.entities.get_sections('libfreertos.a', 'timers')
        expected = set(['.literal.prvGetNextExpireTime', '.literal.prvInsertTimerInActiveList'])
        self.assertEqual(set(sections), expected)

    def test_parsing(self):
        # Tests parsing objdump with the following:
        #
        # - non-ascii characters
        # - different architecture string
        # - different column entries for each sections
        # - unexpected 'comments'
        with open('data/test_entity/parse_test.txt') as objdump:
            self.entities.add_sections_info(objdump)

        sections = self.entities.get_sections('ěščřžýáíé.a', 'croutine')
        self.assertEqual(set(sections), set(['.text', '.data', '.bss']))

        sections = self.entities.get_sections('ěščřžýáíé.a', 'FreeRTOS-ěščřžýáíé')
        self.assertEqual(set(sections), set(['.literal.ěščřžýáíé']))

        sections = self.entities.get_sections('ěščřžýáíé.a', 'o_suffix')
        self.assertEqual(set(sections), set(['.find_me']))


if __name__ == '__main__':
    unittest.main()

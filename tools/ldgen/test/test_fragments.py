#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import os
import sys
import tempfile
import unittest
from io import StringIO

from pyparsing import ParseException, ParseFatalException, Word, alphanums

try:
    from fragments import FRAGMENT_TYPES, Fragment, FragmentFile, Mapping
    from sdkconfig import SDKConfig
except ImportError:
    sys.path.append('../')
    from fragments import FRAGMENT_TYPES, Fragment, FragmentFile, Mapping
    from sdkconfig import SDKConfig


class SampleFragment(Fragment):

    grammars = {
        'key_1': Fragment.KeyValue(Word(alphanums + '_').setResultsName('value'), 0, None, True),
        'key_2': Fragment.KeyValue(Word(alphanums + '_').setResultsName('value'), 0, None, False),
        'key_3': Fragment.KeyValue(Word(alphanums + '_').setResultsName('value'), 3, 5, False)
    }

    def set_key_value(self, key, parse_results):
        if key == 'key_1':
            self.key_1 = list()
            for result in parse_results:
                self.key_1.append(result['value'])
        elif key == 'key_2':
            self.key_2 = list()
            for result in parse_results:
                self.key_2.append(result['value'])

    def get_key_grammars(self):
        return self.__class__.grammars


FRAGMENT_TYPES['test'] = SampleFragment


class FragmentTest(unittest.TestCase):

    def setUp(self):
        with tempfile.NamedTemporaryFile(delete=False) as f:
            self.kconfigs_source_file = os.path.join(tempfile.gettempdir(), f.name)
        with tempfile.NamedTemporaryFile(delete=False) as f:
            self.kconfig_projbuilds_source_file = os.path.join(tempfile.gettempdir(), f.name)

        os.environ['COMPONENT_KCONFIGS_SOURCE_FILE'] = self.kconfigs_source_file
        os.environ['COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE'] = self.kconfig_projbuilds_source_file
        os.environ['COMPONENT_KCONFIGS'] = ''
        os.environ['COMPONENT_KCONFIGS_PROJBUILD'] = ''

        # prepare_kconfig_files.py doesn't have to be called because COMPONENT_KCONFIGS and
        # COMPONENT_KCONFIGS_PROJBUILD are empty

        self.sdkconfig = SDKConfig('data/Kconfig', 'data/sdkconfig')

    def tearDown(self):
        try:
            os.remove(self.kconfigs_source_file)
            os.remove(self.kconfig_projbuilds_source_file)
        except Exception:
            pass

    @staticmethod
    def create_fragment_file(contents, name='test_fragment.lf'):
        f = StringIO(contents)
        f.name = name
        return f

    def test_basic(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_1
    value_2 # comments should be ignored
    value_3
# this is a comment as well
key_2: value_a

# this is the last comment
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)

        self.assertEqual(len(fragment_file.fragments[0].key_1), 3)
        self.assertEqual(fragment_file.fragments[0].key_1[0], 'value_1')
        self.assertEqual(fragment_file.fragments[0].key_1[1], 'value_2')
        self.assertEqual(fragment_file.fragments[0].key_1[2], 'value_3')
        self.assertEqual(len(fragment_file.fragments[0].key_2), 1)
        self.assertEqual(fragment_file.fragments[0].key_2[0], 'value_a')

    def test_duplicate_keys(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1: value_1
key_1: value_a
""")
        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_empty_key(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
""")
        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_conditional(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_1
    if A = y:
        value_2
    value_3
    if A = n:
        value_4
    if B = n:
        value_5
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].key_1[0], 'value_1')
        self.assertEqual(fragment_file.fragments[0].key_1[1], 'value_2')
        self.assertEqual(fragment_file.fragments[0].key_1[2], 'value_3')
        self.assertEqual(fragment_file.fragments[0].key_1[3], 'value_5')

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_1
    if B = y:
        value_2
    elif C = y:
        value_3
    elif A = y:
        value_4
    else:
        value_5
    value_6
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].key_1[0], 'value_1')
        self.assertEqual(fragment_file.fragments[0].key_1[1], 'value_3')
        self.assertEqual(fragment_file.fragments[0].key_1[2], 'value_6')

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_1
    if A = y:
        value_2
        if B = y:
            value_3
        else:
            value_4
            if C = y:
                value_5
            value_6
        value_7
key_2:
    value_a
    if B != y:
        value_b
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].key_1[0], 'value_1')
        self.assertEqual(fragment_file.fragments[0].key_1[1], 'value_2')
        self.assertEqual(fragment_file.fragments[0].key_1[2], 'value_4')
        self.assertEqual(fragment_file.fragments[0].key_1[3], 'value_5')
        self.assertEqual(fragment_file.fragments[0].key_1[4], 'value_6')
        self.assertEqual(fragment_file.fragments[0].key_1[5], 'value_7')
        self.assertEqual(fragment_file.fragments[0].key_2[0], 'value_a')
        self.assertEqual(fragment_file.fragments[0].key_2[1], 'value_b')

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    if A = n:
        value_2
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(len(fragment_file.fragments[0].key_1), 0)

    def test_empty_file(self):
        test_fragment = self.create_fragment_file(u"""




""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(len(fragment_file.fragments), 0)

    def test_setting_indent(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
 value_1
 value_2
 value_3
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)

        self.assertEqual(len(fragment_file.fragments[0].key_1), 3)
        self.assertEqual(fragment_file.fragments[0].key_1[0], 'value_1')
        self.assertEqual(fragment_file.fragments[0].key_1[1], 'value_2')
        self.assertEqual(fragment_file.fragments[0].key_1[2], 'value_3')

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
 value_1
  value_2 # first element dictates indent
  value_3
""")
        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_values_num_limit(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_a
key_3:
    value_1
    value_2
    value_3
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_a
key_3:
    value_1
    value_2
    value_3
    value_4
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(len(fragment_file.fragments), 1)

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_a
key_3:
    value_1
    value_2
    value_3
    value_4
    value_5
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(len(fragment_file.fragments), 1)

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_a
key_3:
    value_1
    value_2
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_a
key_3:
    value_1
    value_2
    value_3
    value_4
    value_5
    value_6
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_unsupported_key(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    value_a
key_4:
    value_1
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_empty_fragment(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_empty_conditional(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    if B = y:
    else:
        value_1
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    if B = y:
        value_1
    else B = y:
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    if B = y:
        value_1
    elif B = y:
    else:
        value_2
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_out_of_order_conditional(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    elif B = y:
        value_1
    else:
        value_2
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[test:test]
key_1:
    else:
        value_2
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_required_keys(self):
        test_fragment = self.create_fragment_file(u"""
[test:test]
key_2:
    value_1
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_multiple_fragments(self):
        test_fragment = self.create_fragment_file(u"""
[test:test1]
key_1:
    value_1

[test:test2]
key_1:
    value_2
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)

        self.assertEqual(len(fragment_file.fragments), 2)
        self.assertEqual(fragment_file.fragments[0].key_1[0], 'value_1')
        self.assertEqual(fragment_file.fragments[1].key_1[0], 'value_2')

    def test_whole_conditional_fragment(self):
        test_fragment = self.create_fragment_file(u"""
if B = y:
    [test:test1]
    key_1:
        value_1
else:
    [test:test2]
    key_1:
        value_2

    if A = y:
        [test:test3]
        key_1:
            value_3
            if C = y:
                value_6

    [test:test4]
    key_1:
        value_4

[test:test5]
key_1:
    value_5
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(len(fragment_file.fragments), 4)
        self.assertEqual(fragment_file.fragments[0].name, 'test2')
        self.assertEqual(fragment_file.fragments[1].name, 'test3')
        self.assertEqual(fragment_file.fragments[1].key_1[1], 'value_6')
        self.assertEqual(fragment_file.fragments[2].name, 'test4')
        self.assertEqual(fragment_file.fragments[3].name, 'test5')

    def test_equivalent_conditional_fragment(self):
        test_fragment1 = self.create_fragment_file(u"""
if A = y:
    [test:test1]
    key_1:
        value_1
else:
    [test:test2]
    key_1:
        value_2
""")

        fragment_file1 = FragmentFile(test_fragment1, self.sdkconfig)
        self.assertEqual(len(fragment_file1.fragments), 1)
        self.assertEqual(fragment_file1.fragments[0].key_1[0], 'value_1')

        test_fragment2 = self.create_fragment_file(u"""
[test:test1]
key_1:
    if A = y:
        value_1
    else:
        value_2
""")

        fragment_file2 = FragmentFile(test_fragment2, self.sdkconfig)
        self.assertEqual(len(fragment_file2.fragments), 1)
        self.assertEqual(fragment_file2.fragments[0].key_1[0], 'value_1')


class SectionsTest(FragmentTest):

    def test_basic(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    .section1
    .section2
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries, {'.section1', '.section2'})

    def test_duplicate_entries(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    .section1
    .section2
    .section3
    .section2
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries, {'.section1', '.section2', '.section3'})

    def test_empty_entries(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    if B = y:
        .section1
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_entries_grammar(self):

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    _valid1
    valid2.
    .valid3_-
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries,
                         {'_valid1', 'valid2.', '.valid3_-'})

        # invalid starting char
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    1invalid
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    -invalid
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        # + notation
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    valid+
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries,
                         {'valid+'})

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    inva+lid+
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)


class SchemeTest(FragmentTest):

    def test_basic(self):
        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
    sections1 -> target1
    sections2 -> target2
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries,
                         {('sections1', 'target1'),
                          ('sections2', 'target2')})

    def test_duplicate_entries(self):
        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
    sections1 -> target1
    sections2 -> target2
    sections2 -> target2
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries,
                         {('sections1', 'target1'),
                          ('sections2', 'target2')})

    def test_empty_entries(self):
        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
    if B = y:
        sections1 -> target1
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_improper_grammar(self):
        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
    sections1, target1 # improper separator
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)


class MappingTest(FragmentTest):

    def test_basic(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj:symbol (noflash)
    obj (noflash)
    obj:symbol_2 (noflash)
    obj_2 (noflash)
    * (noflash)
""")

        expected = {('obj', 'symbol', 'noflash'),
                    ('obj', None, 'noflash'),
                    ('obj', 'symbol_2', 'noflash'),
                    ('obj_2', None, 'noflash'),
                    ('*', None, 'noflash')}

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_archive(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
entries:
    * (default)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib1.a
    lib2.a
entries:
    * (default)
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_archive_allowed_names(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    libstdc++.a
entries:
    * (default)
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual('libstdc++.a', fragment_file.fragments[0].archive)

    def test_empty_entries(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib.a
entries:
    if B = y:
        * (noflash) # if condition is false, then no 'entries' key value
""")

        expected = set()

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(expected, fragment_file.fragments[0].entries)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib.a
entries:
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_duplicate_entries(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib.a
entries:
    obj:symbol (noflash)
    obj:symbol (noflash)
""")

        expected = {('obj', 'symbol', 'noflash')}

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_invalid_grammar(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib.a
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
entries:
    * (default)
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj: (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj: ()
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj:symbol
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj:* (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    :symbol (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    *:symbol (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_keep_flag(self):
        # Test parsing combinations and orders of flags
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default);
        text->flash_text KEEP(),
        rodata->flash_rodata KEEP() KEEP()
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)

        fragment = fragment_file.fragments[0]

        expected = [('text', 'flash_text', [Mapping.Keep()]),
                    ('rodata', 'flash_rodata', [Mapping.Keep(), Mapping.Keep()])]
        actual = fragment.flags[('obj1', None, 'default')]

        self.assertEqual(expected, actual)

    def test_align_flag(self):
        # Test parsing combinations and orders of flags
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default);
        text->flash_text ALIGN(8),
        rodata->flash_rodata ALIGN(8, pre),
        data->dram0_data ALIGN(8, pre, post),
        bss->dram0_bss ALIGN(8, post),
        common->dram0_bss ALIGN(8, pre, post) ALIGN(8)
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [('text', 'flash_text', [Mapping.Align(8, True, False)]),
                    ('rodata', 'flash_rodata', [Mapping.Align(8, True, False)]),
                    ('data', 'dram0_data', [Mapping.Align(8, True, True)]),
                    ('bss', 'dram0_bss', [Mapping.Align(8, False, True)]),
                    ('common', 'dram0_bss', [Mapping.Align(8, True, True), Mapping.Align(8, True, False)])]
        actual = fragment.flags[('obj1', None, 'default')]

        self.assertEqual(expected, actual)

        # Wrong post, pre order
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (noflash)
        text->iram0_text ALIGN(8, post, pre)
""")

        with self.assertRaises(ParseFatalException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_sort_flag(self):
        # Test parsing combinations and orders of flags
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default);
        text->flash_text SORT(name),
        rodata->flash_rodata SORT(alignment),
        data->dram0_data SORT(init_priority),
        bss->dram0_bss SORT(name, alignment),
        common->dram0_bss SORT(alignment, name),
        iram->iram0_text SORT(name, name),
        dram->dram0_data SORT(alignment, alignment)
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [('text', 'flash_text', [Mapping.Sort('name')]),
                    ('rodata', 'flash_rodata', [Mapping.Sort('alignment')]),
                    ('data', 'dram0_data', [Mapping.Sort('init_priority')]),
                    ('bss', 'dram0_bss', [Mapping.Sort('name', 'alignment')]),
                    ('common', 'dram0_bss', [Mapping.Sort('alignment', 'name')]),
                    ('iram', 'iram0_text', [Mapping.Sort('name', 'name')]),
                    ('dram', 'dram0_data', [Mapping.Sort('alignment', 'alignment')])]
        actual = fragment.flags[('obj1', None, 'default')]
        self.assertEqual(expected, actual)

        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default)
        text->iram0_text SORT(name) SORT(alignment)
""")

    def test_surround_flag(self):
        # Test parsing combinations and orders of flags
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default);
        text->flash_text SURROUND(sym1)
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [('text', 'flash_text', [Mapping.Surround('sym1')])]
        actual = fragment.flags[('obj1', None, 'default')]
        self.assertEqual(expected, actual)

    def test_flag_order(self):
        # Test that the order in which the flags are specified is retained
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default);
        text->flash_text ALIGN(4) KEEP() SURROUND(sym1) ALIGN(8) SORT(name),
        rodata->flash_rodata KEEP() ALIGN(4) KEEP() SURROUND(sym1) ALIGN(8) ALIGN(4) SORT(name)
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [('text', 'flash_text', [Mapping.Align(4, True, False),
                                            Mapping.Keep(),
                                            Mapping.Surround('sym1'),
                                            Mapping.Align(8, True, False),
                                            Mapping.Sort('name')]),
                    ('rodata', 'flash_rodata', [Mapping.Keep(),
                                                Mapping.Align(4, True, False),
                                                Mapping.Keep(),
                                                Mapping.Surround('sym1'),
                                                Mapping.Align(8, True, False),
                                                Mapping.Align(4, True, False),
                                                Mapping.Sort('name')])]
        actual = fragment.flags[('obj1', None, 'default')]
        self.assertEqual(expected, actual)

    def test_flags_entries_multiple_flags(self):
        # Not an error, generation step handles this, since
        # it that step has a more complete information
        # about all mappings.
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default);
        text->flash_text ALIGN(4) KEEP() SURROUND(sym1) SORT(name),
        text->flash_text ALIGN(4) KEEP() SURROUND(sym1) SORT(name)
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [('text', 'flash_text', [Mapping.Align(4, True, False),
                                            Mapping.Keep(),
                                            Mapping.Surround('sym1'),
                                            Mapping.Sort('name')]),
                    ('text', 'flash_text', [Mapping.Align(4, True, False),
                                            Mapping.Keep(),
                                            Mapping.Surround('sym1'),
                                            Mapping.Sort('name')])]
        actual = fragment.flags[('obj1', None, 'default')]
        self.assertEqual(expected, actual)

    def test_flags_entries_multiple_flags_and_entries(self):
        # Not an error, generation step handles this, since
        # it that step has a more complete information
        # about all mappings. This can happen across multiple
        # mapping fragments.
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default);
        text->flash_text ALIGN(4) KEEP() SURROUND(sym1) SORT(name)
    obj1 (default);
        text->flash_text ALIGN(4) KEEP() SURROUND(sym1) SORT(name)
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [('text', 'flash_text', [Mapping.Align(4, True, False),
                                            Mapping.Keep(),
                                            Mapping.Surround('sym1'),
                                            Mapping.Sort('name')]),
                    ('text', 'flash_text', [Mapping.Align(4, True, False),
                                            Mapping.Keep(),
                                            Mapping.Surround('sym1'),
                                            Mapping.Sort('name')])]
        actual = fragment.flags[('obj1', None, 'default')]
        self.assertEqual(expected, actual)


class DeprecatedMappingTest(FragmentTest):

    def test_valid_grammar(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    obj:symbol (noflash)
    # Comments should not matter
    obj (noflash)
    # Nor should whitespace
                    obj  :     symbol_2 (    noflash )
        obj_2  (    noflash )
    * (noflash)
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual('lib.a', fragment_file.fragments[0].archive)
        self.assertEqual('lib_a', fragment_file.fragments[0].name)

        expected = {('obj', 'symbol', 'noflash'),
                    ('obj', None, 'noflash'),
                    ('obj', 'symbol_2', 'noflash'),
                    ('obj_2', None, 'noflash'),
                    ('*', None, 'noflash')
                    }

        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_explicit_blank_default_w_others(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : A = n
    obj_a (noflash)
    : default
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        expected = {('*', None, 'default')}

        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_implicit_blank_default_w_others(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : A = n
    obj_a (noflash)
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        expected = {('*', None, 'default')}

        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_explicit_blank_default(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : default
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        expected = {('*', None, 'default')}

        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_implicit_blank_default(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : default
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        expected = {('*', None, 'default')}

        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_multiple_entries(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : A = n
    obj_a1 (noflash)
    obj_a2 (noflash)
    : B = n
    obj_b1 (noflash)
    obj_b2 (noflash)
    obj_b3 (noflash)
    : C = n
    obj_c1 (noflash)
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        expected = {('obj_b1', None, 'noflash'),
                    ('obj_b2', None, 'noflash'),
                    ('obj_b3', None, 'noflash')}
        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_blank_entries(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : A = n
    obj_a (noflash)
    : B = n
    : C = n
    obj_c (noflash)
    : default
    obj (noflash)
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        expected = {('*', None, 'default')}
        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_blank_first_condition(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    obj_a (noflash)
    : CONFIG_B = y
    obj_b (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_nonlast_default_1(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : default
    obj_a (noflash)
    : CONFIG_A = y
    obj_A (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_nonlast_default_2(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : A = y
    obj_A (noflash)
    : default
    obj_a (noflash)
    : B = y
    obj_B (noflash
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_nonlast_default_3(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : A = y
    obj_A (noflash)
    :
    obj_a (noflash)
    : B = y
    obj_B (noflash
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_duplicate_default_1(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : CONFIG_A = y
    obj_A (noflash)
    : default
    obj_a (noflash)
    : CONFIG_B = y
    obj_B (noflash)
    : default
    obj_a (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_duplicate_default_2(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : CONFIG_A = y
    obj_A (noflash)
    : CONFIG_B = y
    obj_a (noflash)
    : default
    obj_B (noflash)
    :
    obj_a (noflash)
""")

        with self.assertRaises(ParseException):
            FragmentFile(test_fragment, self.sdkconfig)

    def test_mixed_deprecated_mapping(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : A = n
    obj_A (noflash)
    : default
    obj_B (noflash)


[mapping:test]
archive: lib.a
entries:
    if A = n:
        obj_A (noflash)
    else:
        obj_B (noflash)
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(2, len(fragment_file.fragments))

        self.assertEqual(fragment_file.fragments[0].entries,
                         fragment_file.fragments[1].entries)


if __name__ == '__main__':
    unittest.main()

#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import os
import sys
import tempfile
import unittest
from io import StringIO

try:
    from ldgen.fragments import Align, Flag, Keep, Sort, Surround, parse_fragment_file
    from ldgen.sdkconfig import SDKConfig
except ImportError:
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))
    from ldgen.fragments import Align, Flag, Keep, Sort, Surround, parse_fragment_file
    from ldgen.sdkconfig import SDKConfig

from pyparsing import ParseException, ParseFatalException


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
[sections:test]
entries:
    value_1
    value_2 # comments should be ignored
    value_3
# this is a comment as well
    value_a
# this is the last comment
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)

        self.assertEqual(fragment_file.fragments[0].name, 'test')
        self.assertEqual(fragment_file.fragments[0].entries, {'value_1', 'value_2', 'value_3', 'value_a'})

    def test_conditional(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    value_1
    if PERFORMANCE_A = y:
        value_2
    value_3
    if PERFORMANCE_A = n:
        value_4
    if PERFORMANCE_B = n:
        value_5
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)

        self.assertEqual(fragment_file.fragments[0].name, 'test')
        self.assertEqual(fragment_file.fragments[0].entries, {'value_1', 'value_2', 'value_3', 'value_5'})

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    value_1
    if PERFORMANCE_B = y:
        value_2
    elif PERFORMANCE_C = y:
        value_3
    elif PERFORMANCE_A = y:
        value_4
    else:
        value_5
    value_6
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)

        self.assertEqual(fragment_file.fragments[0].name, 'test')
        self.assertEqual(fragment_file.fragments[0].entries, {'value_1', 'value_3', 'value_6'})

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    value_1
    if PERFORMANCE_A = y:
        value_2
        if PERFORMANCE_B = y:
            value_3
        else:
            value_4
            if PERFORMANCE_C = y:
                value_5
            value_6
        value_7
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)

        self.assertEqual(fragment_file.fragments[0].name, 'test')
        self.assertEqual(fragment_file.fragments[0].entries,
                         {'value_1', 'value_2', 'value_4', 'value_5', 'value_6', 'value_7'})

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    if PERFORMANCE_A = n:
        value_2
""")
        with self.assertRaises(ParseFatalException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_empty_file(self):
        test_fragment = self.create_fragment_file(u"""
""")

        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual(len(fragment_file.fragments), 0)

    def test_setting_indent(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
 value_1
 value_2
 value_3
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)

        self.assertEqual(fragment_file.fragments[0].name, 'test')
        self.assertEqual(fragment_file.fragments[0].entries, {'value_1', 'value_2', 'value_3'})

    def test_settings_unmatch_indent(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
 value_1
  value_2 # first element dictates indent
  value_3
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_unsupported_key(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
key_1:
    value_a
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_empty_fragment(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_empty_conditional(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    if PERFORMANCE_B = y:
    else:
        value_1
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    if PERFORMANCE_B = y:
        value_1
    else PERFORMANCE_B = y:
""")
        with self.assertRaises(ParseFatalException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    if PERFORMANCE_B = y:
        value_1
    elif PERFORMANCE_B = y:
    else:
        value_2
""")
        with self.assertRaises(ParseFatalException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_out_of_order_conditional(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    elif PERFORMANCE_B = y:
        value_1
    else:
        value_2
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    else:
        value_2
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_multiple_fragments(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test1]
entries:
    value_1

[scheme:test2]
entries:
    section -> target
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)

        self.assertEqual(fragment_file.fragments[0].name, 'test1')
        self.assertEqual(fragment_file.fragments[0].entries, {'value_1'})
        self.assertEqual(fragment_file.fragments[1].name, 'test2')
        self.assertEqual(fragment_file.fragments[1].entries, {('section', 'target')})

    def test_whole_conditional_fragment(self):
        test_fragment = self.create_fragment_file(u"""
if PERFORMANCE_B = y:
    [sections:test1]
    entries:
        value_1
else:
    [sections:test2]
    entries:
        value_2

    if PERFORMANCE_A = y:
        [sections:test3]
        entries:
            value_3
            if PERFORMANCE_C = y:
                value_6

    [sections:test4]
    entries:
        value_4

[sections:test5]
entries:
    value_5
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)

        self.assertEqual(len(fragment_file.fragments), 4)
        self.assertEqual(fragment_file.fragments[0].name, 'test2')
        self.assertEqual(fragment_file.fragments[0].entries, {'value_2'})
        self.assertEqual(fragment_file.fragments[1].name, 'test3')
        self.assertEqual(fragment_file.fragments[1].entries, {'value_3', 'value_6'})
        self.assertEqual(fragment_file.fragments[2].name, 'test4')
        self.assertEqual(fragment_file.fragments[2].entries, {'value_4'})
        self.assertEqual(fragment_file.fragments[3].name, 'test5')
        self.assertEqual(fragment_file.fragments[3].entries, {'value_5'})

    def test_equivalent_conditional_fragment(self):
        test_fragment1 = self.create_fragment_file(u"""
if PERFORMANCE_A = y:
    [sections:test1]
    entries:
        value_1
else:
    [sections:test2]
    entries:
        value_2
""")
        fragment_file1 = parse_fragment_file(test_fragment1, self.sdkconfig)

        self.assertEqual(fragment_file1.fragments[0].name, 'test1')
        self.assertEqual(fragment_file1.fragments[0].entries, {'value_1'})

        test_fragment2 = self.create_fragment_file(u"""
[sections:test1]
entries:
    if PERFORMANCE_A = y:
        value_1
    else:
        value_2
""")
        fragment_file2 = parse_fragment_file(test_fragment2, self.sdkconfig)

        self.assertEqual(fragment_file2.fragments[0].name, 'test1')
        self.assertEqual(fragment_file2.fragments[0].entries, {'value_1'})


class SectionsTest(FragmentTest):

    def test_basic(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    .section1
    .section2
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
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
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries, {'.section1', '.section2', '.section3'})

    def test_empty_entries(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    if PERFORMANCE_B = y:
        .section1
""")
        with self.assertRaises(ParseFatalException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_entries_grammar(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    _valid1
    valid2.
    .valid3_-
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries,
                         {'_valid1', 'valid2.', '.valid3_-'})

        # invalid starting char
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    1invalid
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    -invalid
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        # + notation
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    valid+
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries,
                         {'valid+'})

        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    inva+lid+
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)


class SchemeTest(FragmentTest):

    def test_basic(self):
        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
    sections1 -> target1
    sections2 -> target2
""")

        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
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

        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries,
                         {('sections1', 'target1'),
                          ('sections2', 'target2')})

    def test_empty_entries(self):
        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
    if PERFORMANCE_B = y:
        sections1 -> target1
""")
        with self.assertRaises(ParseFatalException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_improper_grammar(self):
        test_fragment = self.create_fragment_file(u"""
[scheme:test]
entries:
    sections1, target1 # improper separator
""")

        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)


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

        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_archive(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
entries:
    * (default)
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib1.a
    lib2.a
entries:
    * (default)
""")
        with self.assertRaises(ParseFatalException):
            parse_fragment_file(test_fragment, self.sdkconfig)

    def test_archive_allowed_names(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    libstdc++.a
entries:
    * (default)
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual('libstdc++.a', fragment_file.fragments[0].archive)

    def test_empty_entries(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib.a
entries:
    if PERFORMANCE_B = y:
        * (noflash) # if condition is false, then no 'entries' key value
""")

        expected = set()

        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual(expected, fragment_file.fragments[0].entries)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib.a
entries:
""")

        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

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

        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_invalid_grammar(self):
        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive:
    lib.a
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
entries:
    * (default)
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj: (noflash)
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj: ()
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj:symbol
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    (noflash)
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    obj:* (noflash)
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    :symbol (noflash)
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

        test_fragment = self.create_fragment_file(u"""
[mapping:test]
archive: lib.a
entries:
    *:symbol (noflash)
""")
        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

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
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)

        fragment = fragment_file.fragments[0]

        expected = [Flag('text', 'flash_text', [Keep()]),
                    Flag('rodata', 'flash_rodata', [Keep(), Keep()])]
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

        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [Flag('text', 'flash_text', [Align(8, True, False)]),
                    Flag('rodata', 'flash_rodata', [Align(8, True, False)]),
                    Flag('data', 'dram0_data', [Align(8, True, True)]),
                    Flag('bss', 'dram0_bss', [Align(8, False, True)]),
                    Flag('common', 'dram0_bss', [Align(8, True, True), Align(8, True, False)])]
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

        with self.assertRaises(ParseException):
            parse_fragment_file(test_fragment, self.sdkconfig)

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

        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [Flag('text', 'flash_text', [Sort('name')]),
                    Flag('rodata', 'flash_rodata', [Sort('alignment')]),
                    Flag('data', 'dram0_data', [Sort('init_priority')]),
                    Flag('bss', 'dram0_bss', [Sort('name', 'alignment')]),
                    Flag('common', 'dram0_bss', [Sort('alignment', 'name')]),
                    Flag('iram', 'iram0_text', [Sort('name', 'name')]),
                    Flag('dram', 'dram0_data', [Sort('alignment', 'alignment')])]
        actual = fragment.flags[('obj1', None, 'default')]
        self.assertEqual(expected, actual)

    def test_surround_flag(self):
        # Test parsing combinations and orders of flags
        test_fragment = self.create_fragment_file(u"""
[mapping:map]
archive: libmain.a
entries:
    obj1 (default);
        text->flash_text SURROUND(sym1)
""")
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [Flag('text', 'flash_text', [Surround('sym1')])]
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
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [Flag('text', 'flash_text', [Align(4, True, False),
                                                Keep(),
                                                Surround('sym1'),
                                                Align(8, True, False),
                                                Sort('name')]),
                    Flag('rodata', 'flash_rodata', [Keep(),
                                                    Align(4, True, False),
                                                    Keep(),
                                                    Surround('sym1'),
                                                    Align(8, True, False),
                                                    Align(4, True, False),
                                                    Sort('name')])]
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
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [Flag('text', 'flash_text', [Align(4, True, False),
                                                Keep(),
                                                Surround('sym1'),
                                                Sort('name')]),
                    Flag('text', 'flash_text', [Align(4, True, False),
                                                Keep(),
                                                Surround('sym1'),
                                                Sort('name')])]
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
        fragment_file = parse_fragment_file(test_fragment, self.sdkconfig)
        fragment = fragment_file.fragments[0]

        expected = [Flag('text', 'flash_text', [Align(4, True, False),
                                                Keep(),
                                                Surround('sym1'),
                                                Sort('name')]),
                    Flag('text', 'flash_text', [Align(4, True, False),
                                                Keep(),
                                                Surround('sym1'),
                                                Sort('name')])]
        actual = fragment.flags[('obj1', None, 'default')]
        self.assertEqual(expected, actual)


if __name__ == '__main__':
    unittest.main()

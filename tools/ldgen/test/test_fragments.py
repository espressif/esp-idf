#!/usr/bin/env python
#
# Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
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

import unittest
import sys
from pyparsing import ParseException
from pyparsing import restOfLine

try:
    import fragments
except ImportError:
    sys.path.append('../')
    import fragments

from sdkconfig import SDKConfig


class FragmentTest(unittest.TestCase):

    def parse(self, text):
        self.parser.ignore("#" + restOfLine)
        fragment = self.parser.parseString(text, parseAll=True)
        return fragment[0]


class SectionsTest(FragmentTest):

    def setUp(self):
        self.parser = fragments.Sections.get_fragment_grammar()

    def test_valid_entries(self):
        valid_entries = """
        [sections:test]
        entries:
            .section1
                .section2

            # Section 3 should not exist
            # section3
            .section4

            # This is a comment

            .section5
        """

        sections = self.parse(valid_entries)

        self.assertEqual("test", sections.name)

        entries = sections.entries

        expected = {
            ".section1",
            ".section2",
            ".section4",
            ".section5"
        }

        self.assertEqual(set(entries), expected)

    def test_blank_entries(self):
        blank_entries = """
        [sections:test]
        entries:
        """

        with self.assertRaises(ParseException):
            self.parse(blank_entries)

    def test_invalid_names(self):
        with_spaces = """
        [sections:invalid name 1]
        entries:
        """

        begins_with_number = """
        [sections:2invalid_name]
        entries:
        """

        with_special_character = """
        [sections:invalid_name~]
        entries:
        """

        with self.assertRaises(ParseException):
            self.parse(with_spaces)

        with self.assertRaises(ParseException):
            self.parse(begins_with_number)

        with self.assertRaises(ParseException):
            self.parse(with_special_character)

    def test_non_existent_entries(self):
        misspelled_entries_field = """
        [sections:test]
        entrie:
            .section1
        """

        missing_entries_field = """
        [sections:test]
        """

        with self.assertRaises(ParseException):
            self.parse(misspelled_entries_field)

        with self.assertRaises(ParseException):
            self.parse(missing_entries_field)

    def test_duplicate_entries(self):
        duplicate_entries = """
        [sections:test]
        entries:
            .section1
            .section3
            .section1
            .section1
            .section2
            .section3
            .section1
        """

        sections = self.parse(duplicate_entries)

        entries = sections.entries

        expected = {
            ".section1",
            ".section2",
            ".section3",
        }

        self.assertEqual(set(entries), expected)


class SchemeTest(FragmentTest):

    def setUp(self):
        self.parser = fragments.Scheme.get_fragment_grammar()

    def test_valid_entries(self):
        valid_entries = """
        [scheme:test]
        entries:
            sections1 -> target1
            sections2   ->    target2
        """

        scheme = self.parse(valid_entries)

        entries = scheme.entries

        expected = {
            ("sections1", "target1"),
            ("sections2", "target2")
        }

        self.assertEqual(entries, expected)

    def test_duplicate_same_mapping(self):
        duplicate_entries = """
        [scheme:duplicate_same_mapping]
        entries:
            sections1 -> target1
            sections2 -> target2
            sections1 -> target1
        """

        scheme = self.parse(duplicate_entries)

        entries = scheme.entries

        expected = {
            ("sections1", "target1"),
            ("sections2", "target2")
        }

        self.assertEqual(len(entries), 2)
        self.assertEqual(entries, expected)

    def test_invalid_separator(self):
        wrong_character = """
        [scheme:test]
        entries:
            sections1, target1
        """

        single_word = """
        [scheme:test]
        entries:
            sections1
        """

        with self.assertRaises(ParseException):
            self.parse(wrong_character)

        with self.assertRaises(ParseException):
            self.parse(single_word)

    def test_blank_entries(self):
        blank_entries = """
        [scheme:test]
        entries:
        """

        with self.assertRaises(ParseException):
            self.parse(blank_entries)

    def test_non_existent_entries(self):
        misspelled_entries_field = """
        [scheme:test]
        entrie:
            section -> target
        """

        missing_entries_field = """
        [scheme:test]
        """

        with self.assertRaises(ParseException):
            self.parse(misspelled_entries_field)

        with self.assertRaises(ParseException):
            self.parse(missing_entries_field)


class MappingTest(FragmentTest):

    def setUp(self):
        self.parser = fragments.Mapping.get_fragment_grammar()

    def parse_expression(self, expression):
        parser = SDKConfig.get_expression_grammar()
        return parser.parseString(expression, parseAll=True)

    def test_valid_grammar(self):
        valid_entries = """
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
        """

        mapping = self.parse(valid_entries)

        self.assertEqual("lib.a", mapping.archive)
        self.assertEqual("lib_a", mapping.name)

        entries = mapping.entries

        expected = [("default", {
                    ("obj", "symbol", "noflash"),
                    ("obj", None, "noflash"),
                    ("obj", "symbol_2", "noflash"),
                    ("obj_2", None, "noflash"),
                    ("*", None, "noflash")
                    })]

        self.assertEqual(entries, expected)

    def test_invalid_grammar(self):
        with_fragment_name = """
        [mapping:name]
        archive: lib.a
        entries:
            obj:symbol (noflash)
        """

        missing_archive = """
        [mapping:name]
        entries:
            obj:symbol (noflash)
        """

        misspelled_archive = """
        [mapping:name]
        archi: lib.a
        entries:
            obj:symbol (noflash)
        """

        missing_entries = """
        [mapping]
        archive: lib.a
        """

        misspelled_entries = """
        [mapping]
        archive: lib.a
        entrie:
            obj:symbol (noflash)
        """

        missing_symbols = """
        [mapping]
        archive: lib.a
        entries:
            obj: (noflash)
        """

        missing_scheme_1 = """
        [mapping]
        archive: lib.a
        entries:
            obj: ()
        """

        missing_scheme_2 = """
        [mapping]
        archive: lib.a
        entries:
            obj:symbol
        """

        missing_entity = """
        [mapping]
        archive: lib.a
        entries:
            (noflash)
        """

        wilcard_symbol = """
        [mapping]
        archive: lib.a
        entries:
            obj:* (noflash)
        """

        empty_object_with_symbol = """
        [mapping]
        archive: lib.a
        entries:
            :symbol (noflash)
        """

        wildcard_object_with_symbol = """
        [mapping]
        archive: lib.a
        entries:
            *:symbol (noflash)
        """

        empty_definition = """
        [mapping]
        """

        with self.assertRaises(ParseException):
            self.parse(with_fragment_name)

        with self.assertRaises(ParseException):
            self.parse(missing_archive)

        with self.assertRaises(ParseException):
            self.parse(misspelled_archive)

        with self.assertRaises(ParseException):
            self.parse(missing_entries)

        with self.assertRaises(ParseException):
            self.parse(misspelled_entries)

        with self.assertRaises(ParseException):
            self.parse(missing_symbols)

        with self.assertRaises(ParseException):
            self.parse(missing_scheme_1)

        with self.assertRaises(ParseException):
            self.parse(missing_scheme_2)

        with self.assertRaises(ParseException):
            self.parse(missing_entity)

        with self.assertRaises(ParseException):
            self.parse(wilcard_symbol)

        with self.assertRaises(ParseException):
            self.parse(empty_object_with_symbol)

        with self.assertRaises(ParseException):
            self.parse(wildcard_object_with_symbol)

        with self.assertRaises(ParseException):
            self.parse(empty_definition)

    def test_explicit_blank_default_w_others(self):
        expl_blnk_w_oth = """
        [mapping]
        archive: lib.a
        entries:
            : CONFIG_A = y
            obj_a (noflash)
            : default
        """

        mapping = self.parse(expl_blnk_w_oth)

        entries = mapping.entries

        expected = [(entries[0][0], {
                    ("obj_a", None, "noflash"),
                    }),
                    ("default", set())]

        self.assertEqual(entries, expected)

    def test_implicit_blank_default_w_others(self):
        impl_blnk_w_oth = """
        [mapping]
        archive: lib.a
        entries:
            : CONFIG_A = y
            obj_a (noflash)
        """

        mapping = self.parse(impl_blnk_w_oth)

        entries = mapping.entries

        expected = [(entries[0][0], {
                    ("obj_a", None, "noflash"),
                    }),
                    ("default", set())]

        self.assertEqual(entries, expected)

    def test_explicit_blank_default(self):
        expl_blnk_def = """
        [mapping]
        archive: lib.a
        entries:
            : default
        """
        mapping = self.parse(expl_blnk_def)
        entries = mapping.entries
        expected = [("default", set())]

        self.assertEqual(entries, expected)

    def test_implicit_blank_default(self):
        impl_blnk_def = """
        [mapping]
        archive: lib.a
        entries:
            : default
        """
        mapping = self.parse(impl_blnk_def)
        entries = mapping.entries
        expected = [("default", set())]

        self.assertEqual(entries, expected)

    def test_multiple_entries(self):
        multiple_entries = """
        [mapping]
        archive: lib.a
        entries:
            : CONFIG_A = y
            obj_a1 (noflash)
            obj_a2 (noflash)
            : CONFIG_B = y
            obj_b1 (noflash)
            obj_b2 (noflash)
            obj_b3 (noflash)
            : CONFIG_C = y
            obj_c1 (noflash)
        """

        mapping = self.parse(multiple_entries)

        entries = mapping.entries

        expected = [(entries[0][0], {
                    ("obj_a1", None, "noflash"),
                    ("obj_a2", None, "noflash"),
                    }),
                    (entries[1][0], {
                        ("obj_b1", None, "noflash"),
                        ("obj_b2", None, "noflash"),
                        ("obj_b3", None, "noflash"),
                    }),
                    (entries[2][0], {
                        ("obj_c1", None, "noflash"),
                    }),
                    ("default", set())]

        self.assertEqual(entries, expected)

    def test_blank_entries(self):
        blank_entries = """
        [mapping]
        archive: lib.a
        entries:
            : CONFIG_A = y
            obj_a (noflash)
            : CONFIG_B = y
            : CONFIG_C = y
            obj_c (noflash)
            : CONFIG_D = y
            : CONFIG_E = y
            : default
            obj (noflash)
        """

        mapping = self.parse(blank_entries)

        entries = mapping.entries

        expected = [(entries[0][0], {
                    ("obj_a", None, "noflash")
                    }),
                    (entries[1][0], set()),
                    (entries[2][0], {
                        ("obj_c", None, "noflash")
                    }),
                    (entries[3][0], set()),
                    (entries[4][0], set()),
                    ("default", {
                        ("obj", None, "noflash")
                    })]

        self.assertEqual(entries, expected)

    def test_blank_first_condition(self):
        blank_first_condition = """
        [mapping]
        archive: lib.a
        entries:
            obj_a (noflash)
            : CONFIG_B = y
            obj_b (noflash)
        """

        with self.assertRaises(ParseException):
            self.parse(blank_first_condition)

    def test_nonlast_default(self):
        nonlast_default_1 = """
        [mapping]
        archive: lib.a
        entries:
            : default
            obj_a (noflash)
            : CONFIG_A = y
            obj_A (noflash)
        """

        nonlast_default_2 = """
        [mapping]
        archive: lib.a
        entries:
            : CONFIG_A = y
            obj_A (noflash)
            : default
            obj_a (noflash)
            : CONFIG_B = y
            obj_B (noflash)
        """

        nonlast_default_3 = """
        [mapping]
        archive: lib.a
        entries:
            : CONFIG_A = y
            obj_A (noflash)
            :
            obj_a (noflash)
            : CONFIG_B = y
            obj_B (noflash)
        """

        with self.assertRaises(ParseException):
            self.parse(nonlast_default_1)

        with self.assertRaises(ParseException):
            self.parse(nonlast_default_2)

        with self.assertRaises(ParseException):
            self.parse(nonlast_default_3)

    def test_duplicate_default(self):
        duplicate_default_1 = """
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
        """

        duplicate_default_2 = """
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
        """

        with self.assertRaises(ParseException):
            self.parse(duplicate_default_1)

        with self.assertRaises(ParseException):
            self.parse(duplicate_default_2)


if __name__ == "__main__":
    unittest.main()

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
import sys
import unittest

from io import StringIO
from pyparsing import Word, ParseException, ParseFatalException, alphanums

try:
    from fragments import FragmentFile, FRAGMENT_TYPES, Fragment, KeyGrammar
    from sdkconfig import SDKConfig
except ImportError:
    sys.path.append('../')
    from fragments import FragmentFile, FRAGMENT_TYPES, Fragment, KeyGrammar
    from sdkconfig import SDKConfig


class SampleFragment(Fragment):

    grammars = {
        "key_1": KeyGrammar(Word(alphanums + "_").setResultsName("value"), 0, None, True),
        "key_2": KeyGrammar(Word(alphanums + "_").setResultsName("value"), 0, None, False),
        "key_3": KeyGrammar(Word(alphanums + "_").setResultsName("value"), 3, 5, False)
    }

    def set_key_value(self, key, parse_results):
        if key == "key_1":
            self.key_1 = list()
            for result in parse_results:
                self.key_1.append(result["value"])
        elif key == "key_2":
            self.key_2 = list()
            for result in parse_results:
                self.key_2.append(result["value"])

    def get_key_grammars(self):
        return self.__class__.grammars


FRAGMENT_TYPES["test"] = SampleFragment


class FragmentTest(unittest.TestCase):

    def setUp(self):
        self.sdkconfig = SDKConfig("data/Kconfig", "data/sdkconfig")

    @staticmethod
    def create_fragment_file(contents, name="test_fragment.lf"):
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
        self.assertEqual(fragment_file.fragments[0].key_1[0], "value_1")
        self.assertEqual(fragment_file.fragments[0].key_1[1], "value_2")
        self.assertEqual(fragment_file.fragments[0].key_1[2], "value_3")
        self.assertEqual(len(fragment_file.fragments[0].key_2), 1)
        self.assertEqual(fragment_file.fragments[0].key_2[0], "value_a")

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
        self.assertEqual(fragment_file.fragments[0].key_1[0], "value_1")
        self.assertEqual(fragment_file.fragments[0].key_1[1], "value_2")
        self.assertEqual(fragment_file.fragments[0].key_1[2], "value_3")
        self.assertEqual(fragment_file.fragments[0].key_1[3], "value_5")

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
        self.assertEqual(fragment_file.fragments[0].key_1[0], "value_1")
        self.assertEqual(fragment_file.fragments[0].key_1[1], "value_3")
        self.assertEqual(fragment_file.fragments[0].key_1[2], "value_6")

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
        self.assertEqual(fragment_file.fragments[0].key_1[0], "value_1")
        self.assertEqual(fragment_file.fragments[0].key_1[1], "value_2")
        self.assertEqual(fragment_file.fragments[0].key_1[2], "value_4")
        self.assertEqual(fragment_file.fragments[0].key_1[3], "value_5")
        self.assertEqual(fragment_file.fragments[0].key_1[4], "value_6")
        self.assertEqual(fragment_file.fragments[0].key_1[5], "value_7")
        self.assertEqual(fragment_file.fragments[0].key_2[0], "value_a")
        self.assertEqual(fragment_file.fragments[0].key_2[1], "value_b")

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
        self.assertEqual(fragment_file.fragments[0].key_1[0], "value_1")
        self.assertEqual(fragment_file.fragments[0].key_1[1], "value_2")
        self.assertEqual(fragment_file.fragments[0].key_1[2], "value_3")

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
        self.assertEqual(fragment_file.fragments[0].key_1[0], "value_1")
        self.assertEqual(fragment_file.fragments[1].key_1[0], "value_2")

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
        self.assertEqual(fragment_file.fragments[0].name, "test2")
        self.assertEqual(fragment_file.fragments[1].name, "test3")
        self.assertEqual(fragment_file.fragments[1].key_1[1], "value_6")
        self.assertEqual(fragment_file.fragments[2].name, "test4")
        self.assertEqual(fragment_file.fragments[3].name, "test5")

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
        self.assertEqual(fragment_file1.fragments[0].key_1[0], "value_1")

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
        self.assertEqual(fragment_file2.fragments[0].key_1[0], "value_1")


class SectionsTest(FragmentTest):

    def test_basic(self):
        test_fragment = self.create_fragment_file(u"""
[sections:test]
entries:
    .section1
    .section2
""")

        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        self.assertEqual(fragment_file.fragments[0].entries, {".section1", ".section2"})

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
        self.assertEqual(fragment_file.fragments[0].entries, {".section1", ".section2", ".section3"})

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
                         {("sections1", "target1"),
                          ("sections2", "target2")})

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
                         {("sections1", "target1"),
                          ("sections2", "target2")})

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

        expected = {("obj", "symbol", "noflash"),
                    ("obj", None, "noflash"),
                    ("obj", "symbol_2", "noflash"),
                    ("obj_2", None, "noflash"),
                    ("*", None, "noflash")}

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

        expected = {("obj", "symbol", "noflash")}

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
        self.assertEqual("lib.a", fragment_file.fragments[0].archive)
        self.assertEqual("lib_a", fragment_file.fragments[0].name)

        expected = {("obj", "symbol", "noflash"),
                    ("obj", None, "noflash"),
                    ("obj", "symbol_2", "noflash"),
                    ("obj_2", None, "noflash"),
                    ("*", None, "noflash")
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
        expected = {("*", None, "default")}

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
        expected = {("*", None, "default")}

        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_explicit_blank_default(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : default
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        expected = {("*", None, "default")}

        self.assertEqual(expected, fragment_file.fragments[0].entries)

    def test_implicit_blank_default(self):
        test_fragment = self.create_fragment_file(u"""
[mapping]
archive: lib.a
entries:
    : default
""")
        fragment_file = FragmentFile(test_fragment, self.sdkconfig)
        expected = {("*", None, "default")}

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
        expected = {("obj_b1", None, "noflash"),
                    ("obj_b2", None, "noflash"),
                    ("obj_b3", None, "noflash")}
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
        expected = {("*", None, "default")}
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


if __name__ == "__main__":
    unittest.main()

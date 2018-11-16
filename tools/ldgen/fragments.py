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

import re
import collections
import sys
import os

from sdkconfig import SDKConfig
from pyparsing import *

"""
Fragment file internal representation. Parses and stores instances of the fragment definitions
contained within the file.
"""
class FragmentFileModel():

    def __init__(self, fragment_file):
        path = os.path.realpath(fragment_file.name)

        sections = Sections.get_fragment_grammar()
        scheme = Scheme.get_fragment_grammar()
        mapping = Mapping.get_fragment_grammar()

        # Each fragment file is composed of sections, scheme or mapping fragments. The grammar
        # for each of those objects are defined it the respective classes
        parser = OneOrMore(sections | scheme | mapping)

        # Set any text beginnning with # as comment
        parser.ignore("#" + restOfLine)

        self.fragments = parser.parseFile(fragment_file, parseAll=True)

        for fragment in self.fragments:
            fragment.path = path

"""
Encapsulates a fragment as defined in the generator syntax. Sets values common to all fragment and performs processing
such as checking the validity of the fragment name and getting the entry values.
"""
class Fragment:

    IDENTIFIER = Word(alphas+"_", alphanums+"_")
    ENTITY = Word(alphanums + ".-_$")

    def __init__(self, name, entries):
        self.path = None
        self.name = name
        self.entries = entries

class Sections(Fragment):

    def __init__(self, name, entries):
        Fragment.__init__(self, name, entries)
        self._process_entries()

    def _process_entries(self):
        # Quietly ignore duplicate entries
        self.entries = set(self.entries)
        self.entries = list(self.entries)

    """
    Utility function that returns a list of sections given a sections fragment entry,
    with the '+' notation and symbol concatenation handled automatically.
    """
    @staticmethod
    def get_section_data_from_entry(sections_entry, symbol=None):
        if not symbol:
            sections = list()
            sections.append(sections_entry.replace("+", ""))
            sections.append(sections_entry.replace("+", ".*"))
            return sections
        else:
            if sections_entry.endswith("+"):
                section = sections_entry.replace("+", ".*")
                expansion = section.replace(".*", "." + symbol)
                return (section, expansion)
            else:
                return (sections_entry, None)

    @staticmethod
    def get_fragment_grammar():
        name = Fragment.IDENTIFIER
        header = Suppress("[") + Suppress("sections") + Suppress(":") + name.setResultsName("name") + Suppress("]")
        entry = Word(alphanums + "+" + ".")
        entries = Suppress("entries") + Suppress(":") + Group(OneOrMore(entry)).setResultsName("entries")

        sections = Group(header + entries)

        sections.setParseAction(lambda t: Sections(t[0].name, t[0].entries))

        sections.ignore("#" + restOfLine)

        return sections

"""
Encapsulates a scheme fragment, which defines what target input sections are placed under.
"""
class Scheme(Fragment):

    def __init__(self, name, items):
        Fragment.__init__(self, name, items)
        self._process_entries()

    def _process_entries(self):
        processed = set()

        # Store entries as a set of tuples. Quietly ignores duplicate entries.
        for entry in self.entries:
            processed.add((entry.sections, entry.target))

        self.entries = processed

    @staticmethod
    def get_fragment_grammar():
        name = Fragment.IDENTIFIER
        header = Suppress("[") + Suppress("scheme") + Suppress(":") + name.setResultsName("name") + Suppress("]")

        # Scheme entry in the form 'sections -> target'
        sections = Fragment.IDENTIFIER
        target = Fragment.IDENTIFIER
        entry = Group(sections.setResultsName("sections") + Suppress("->") + target.setResultsName("target"))

        entries = Suppress("entries") + Suppress(":") + Group(OneOrMore(entry)).setResultsName("entries")

        scheme = Group(header + entries)

        scheme.setParseAction(lambda t: Scheme(t[0].name, t[0].entries))

        scheme.ignore("#" + restOfLine)

        return scheme

"""
Encapsulates a mapping fragment, which defines what targets the input sections of mappable entties are placed under.
"""
class Mapping(Fragment):

    # Name of the default condition entry
    DEFAULT_CONDITION = "default"
    MAPPING_ALL_OBJECTS = "*"

    def __init__(self, archive, entries):
        self.archive = archive

        # Generate name from archive value by replacing all non-alphanumeric
        # characters with underscore
        name = Mapping.get_mapping_name_from_archive(self.archive)
        Fragment.__init__(self, name, entries)

        self._process_entries()

    def _create_mappings_set(self, mappings):
        mapping_set = set()

        for mapping in mappings:
            obj = mapping.object
            symbol = mapping.symbol
            scheme = mapping.scheme

            if symbol == "":
                symbol = None

            # Quietly handle duplicate definitions under the same condition
            mapping_set.add((obj, symbol, scheme))

        return mapping_set

    def _process_entries(self):
        processed = []

        for normal_group in self.entries.normal_groups:
            # Get the original string of the condition
            condition  = next(iter(normal_group.condition.asList())).strip()
            mappings = self._create_mappings_set(normal_group.mappings)

            processed.append((condition, mappings))

        default_group = self.entries.default_group
        mappings = self._create_mappings_set(default_group.mappings)
        processed.append(("default", mappings))

        self.entries = processed

    @staticmethod
    def get_mapping_name_from_archive(archive):
        return re.sub(r"[^0-9a-zA-Z]+", "_", archive)

    @staticmethod
    def get_fragment_grammar():

        # Match header [mapping]
        header = Suppress("[") + Suppress("mapping") + Suppress("]")

        # Define possbile values for input archive and object file
        filename = Word(alphanums + "-" + "_")

        # There are three possible patterns for mapping entries:
        #       obj:symbol (scheme)
        #       obj (scheme)
        #       * (scheme)
        obj = Fragment.ENTITY.setResultsName("object")
        symbol = Suppress(":") + Fragment.IDENTIFIER.setResultsName("symbol")
        scheme = Suppress("(") + Fragment.IDENTIFIER.setResultsName("scheme") + Suppress(")")

        pattern1 = Group(obj + symbol + scheme)
        pattern2 = Group(obj + scheme)
        pattern3 = Group(Literal(Mapping.MAPPING_ALL_OBJECTS).setResultsName("object") + scheme)

        mapping_entry = pattern1 | pattern2 | pattern3

        # To simplify parsing, classify groups of condition-mapping entry into two types: normal and default
        # A normal grouping is one with a non-default condition. The default grouping is one which contains the
        # default condition
        mapping_entries = Group(ZeroOrMore(mapping_entry)).setResultsName("mappings")

        normal_condition = Suppress(":") + originalTextFor(SDKConfig.get_expression_grammar())
        default_condition = Optional(Suppress(":") + Literal(Mapping.DEFAULT_CONDITION))

        normal_group = Group(normal_condition.setResultsName("condition") + mapping_entries)
        default_group = Group(default_condition + mapping_entries).setResultsName("default_group")

        normal_groups = Group(ZeroOrMore(normal_group)).setResultsName("normal_groups")

        # Any mapping fragment definition can have zero or more normal group and only one default group as a last entry.
        archive = Suppress("archive") + Suppress(":") + Fragment.ENTITY.setResultsName("archive")
        entries = Suppress("entries") + Suppress(":") + (normal_groups + default_group).setResultsName("entries")

        mapping = Group(header + archive + entries)

        mapping.setParseAction(lambda t: Mapping(t[0].archive, t[0].entries))

        mapping.ignore("#" + restOfLine)

        return mapping

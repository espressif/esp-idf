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
import os
import re

from sdkconfig import SDKConfig
from pyparsing import OneOrMore
from pyparsing import restOfLine
from pyparsing import alphanums
from pyparsing import Word
from pyparsing import alphas
from pyparsing import ParseFatalException
from pyparsing import Suppress
from pyparsing import Group
from pyparsing import Literal
from pyparsing import ZeroOrMore
from pyparsing import Optional
from pyparsing import originalTextFor
from pyparsing import Forward
from pyparsing import indentedBlock
from collections import namedtuple
import abc


KeyGrammar = namedtuple("KeyGrammar", "grammar min max required")


class FragmentFile():
    """
    Fragment file internal representation. Parses and stores instances of the fragment definitions
    contained within the file.
    """

    def __init__(self, fragment_file, sdkconfig):
        try:
            fragment_file = open(fragment_file, "r")
        except TypeError:
            pass

        path = os.path.realpath(fragment_file.name)

        indent_stack = [1]

        class parse_ctx:
            fragment = None  # current fragment
            key = ""  # current key
            keys = list()  # list of keys parsed
            key_grammar = None  # current key grammar

            @staticmethod
            def reset():
                parse_ctx.fragment_instance = None
                parse_ctx.key = ""
                parse_ctx.keys = list()
                parse_ctx.key_grammar = None

        def fragment_type_parse_action(toks):
            parse_ctx.reset()
            parse_ctx.fragment = FRAGMENT_TYPES[toks[0]]()  # create instance of the fragment
            return None

        def expand_conditionals(toks, stmts):
            try:
                stmt = toks["value"]
                stmts.append(stmt)
            except KeyError:
                try:
                    conditions = toks["conditional"]
                    for condition in conditions:
                        try:
                            _toks = condition[1]
                            _cond = condition[0]
                            if sdkconfig.evaluate_expression(_cond):
                                expand_conditionals(_toks, stmts)
                                break
                        except IndexError:
                            expand_conditionals(condition[0], stmts)
                except KeyError:
                    for tok in toks:
                        expand_conditionals(tok, stmts)

        def key_body_parsed(pstr, loc, toks):
            stmts = list()
            expand_conditionals(toks, stmts)

            if parse_ctx.key_grammar.min and len(stmts) < parse_ctx.key_grammar.min:
                raise ParseFatalException(pstr, loc, "fragment requires at least %d values for key '%s'" %
                                          (parse_ctx.key_grammar.min, parse_ctx.key))

            if parse_ctx.key_grammar.max and len(stmts) > parse_ctx.key_grammar.max:
                raise ParseFatalException(pstr, loc, "fragment requires at most %d values for key '%s'" %
                                          (parse_ctx.key_grammar.max, parse_ctx.key))

            try:
                parse_ctx.fragment.set_key_value(parse_ctx.key, stmts)
            except Exception as e:
                raise ParseFatalException(pstr, loc, "unable to add key '%s'; %s" % (parse_ctx.key, e.message))
            return None

        key = Word(alphanums + "_") + Suppress(":")
        key_stmt = Forward()

        condition_block = indentedBlock(key_stmt, indent_stack)
        key_stmts = OneOrMore(condition_block)
        key_body = Suppress(key) + key_stmts
        key_body.setParseAction(key_body_parsed)

        condition = originalTextFor(SDKConfig.get_expression_grammar()).setResultsName("condition")
        if_condition = Group(Suppress("if") + condition + Suppress(":") + condition_block)
        elif_condition = Group(Suppress("elif") + condition + Suppress(":") + condition_block)
        else_condition = Group(Suppress("else") + Suppress(":") + condition_block)
        conditional = (if_condition + Optional(OneOrMore(elif_condition)) + Optional(else_condition)).setResultsName("conditional")

        def key_parse_action(pstr, loc, toks):
            key = toks[0]

            if key in parse_ctx.keys:
                raise ParseFatalException(pstr, loc, "duplicate key '%s' value definition" % parse_ctx.key)

            parse_ctx.key = key
            parse_ctx.keys.append(key)

            try:
                parse_ctx.key_grammar = parse_ctx.fragment.get_key_grammars()[key]
                key_grammar = parse_ctx.key_grammar.grammar
            except KeyError:
                raise ParseFatalException(pstr, loc, "key '%s' is not supported by fragment" % key)
            except Exception as e:
                raise ParseFatalException(pstr, loc, "unable to parse key '%s'; %s" % (key, e.message))

            key_stmt << (conditional | Group(key_grammar).setResultsName("value"))

            return None

        def name_parse_action(pstr, loc, toks):
            parse_ctx.fragment.name = toks[0]

        key.setParseAction(key_parse_action)

        ftype = Word(alphas).setParseAction(fragment_type_parse_action)
        fid = Suppress(":") + Word(alphanums + "_.").setResultsName("name")
        fid.setParseAction(name_parse_action)
        header = Suppress("[") + ftype + fid + Suppress("]")

        def fragment_parse_action(pstr, loc, toks):
            key_grammars = parse_ctx.fragment.get_key_grammars()
            required_keys = set([k for (k,v) in key_grammars.items() if v.required])
            present_keys = required_keys.intersection(set(parse_ctx.keys))
            if present_keys != required_keys:
                raise ParseFatalException(pstr, loc, "required keys %s for fragment not found" %
                                          list(required_keys - present_keys))
            return parse_ctx.fragment

        fragment_stmt = Forward()
        fragment_block = indentedBlock(fragment_stmt, indent_stack)

        fragment_if_condition = Group(Suppress("if") + condition + Suppress(":") + fragment_block)
        fragment_elif_condition = Group(Suppress("elif") + condition + Suppress(":") + fragment_block)
        fragment_else_condition = Group(Suppress("else") + Suppress(":") + fragment_block)
        fragment_conditional = (fragment_if_condition + Optional(OneOrMore(fragment_elif_condition)) +
                                Optional(fragment_else_condition)).setResultsName("conditional")

        fragment = (header + OneOrMore(indentedBlock(key_body, indent_stack, False))).setResultsName("value")
        fragment.setParseAction(fragment_parse_action)
        fragment.ignore("#" + restOfLine)

        deprecated_mapping = DeprecatedMapping.get_fragment_grammar(sdkconfig, fragment_file.name).setResultsName("value")

        fragment_stmt << (Group(deprecated_mapping) | Group(fragment) | Group(fragment_conditional))

        def fragment_stmt_parsed(pstr, loc, toks):
            stmts = list()
            expand_conditionals(toks, stmts)
            return stmts

        parser = ZeroOrMore(fragment_stmt)
        parser.setParseAction(fragment_stmt_parsed)

        self.fragments = parser.parseFile(fragment_file, parseAll=True)

        for fragment in self.fragments:
            fragment.path = path


class Fragment():
    __metaclass__ = abc.ABCMeta
    """
    Encapsulates a fragment as defined in the generator syntax. Sets values common to all fragment and performs processing
    such as checking the validity of the fragment name and getting the entry values.
    """

    IDENTIFIER = Word(alphas + "_", alphanums + "_")
    ENTITY = Word(alphanums + ".-_$")

    @abc.abstractmethod
    def set_key_value(self, key, parse_results):
        pass

    @abc.abstractmethod
    def get_key_grammars(self):
        pass


class Sections(Fragment):

    grammars = {
        "entries": KeyGrammar(Word(alphanums + "+.").setResultsName("section"), 1, None, True)
    }

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

    def set_key_value(self, key, parse_results):
        if key == "entries":
            self.entries = set()
            for result in parse_results:
                self.entries.add(result["section"])

    def get_key_grammars(self):
        return self.__class__.grammars


class Scheme(Fragment):
    """
    Encapsulates a scheme fragment, which defines what target input sections are placed under.
    """

    grammars = {
        "entries": KeyGrammar(Fragment.IDENTIFIER.setResultsName("sections") + Suppress("->") +
                              Fragment.IDENTIFIER.setResultsName("target"), 1, None, True)
    }

    def set_key_value(self, key, parse_results):
        if key == "entries":
            self.entries = set()
            for result in parse_results:
                self.entries.add((result["sections"], result["target"]))

    def get_key_grammars(self):
        return self.__class__.grammars


class Mapping(Fragment):
    """
    Encapsulates a mapping fragment, which defines what targets the input sections of mappable entties are placed under.
    """

    MAPPING_ALL_OBJECTS = "*"

    def __init__(self):
        Fragment.__init__(self)
        self.entries = set()
        self.deprecated = False

    def set_key_value(self, key, parse_results):
        if key == "archive":
            self.archive = parse_results[0]["archive"]
        elif key == "entries":
            for result in parse_results:
                obj = None
                symbol = None
                scheme = None

                try:
                    obj = result["object"]
                except KeyError:
                    pass

                try:
                    symbol = result["symbol"]
                except KeyError:
                    pass

                try:
                    scheme = result["scheme"]
                except KeyError:
                    pass

                self.entries.add((obj, symbol, scheme))

    def get_key_grammars(self):
        # There are three possible patterns for mapping entries:
        #       obj:symbol (scheme)
        #       obj (scheme)
        #       * (scheme)
        obj = Fragment.ENTITY.setResultsName("object")
        symbol = Suppress(":") + Fragment.IDENTIFIER.setResultsName("symbol")
        scheme = Suppress("(") + Fragment.IDENTIFIER.setResultsName("scheme") + Suppress(")")

        pattern1 = obj + symbol + scheme
        pattern2 = obj + scheme
        pattern3 = Literal(Mapping.MAPPING_ALL_OBJECTS).setResultsName("object") + scheme

        entry = pattern1 | pattern2 | pattern3

        grammars = {
            "archive": KeyGrammar(Fragment.ENTITY.setResultsName("archive"), 1, 1, True),
            "entries": KeyGrammar(entry, 0, None, True)
        }

        return grammars


class DeprecatedMapping():
    """
    Encapsulates a mapping fragment, which defines what targets the input sections of mappable entties are placed under.
    """

    # Name of the default condition entry
    DEFAULT_CONDITION = "default"
    MAPPING_ALL_OBJECTS = "*"

    @staticmethod
    def get_fragment_grammar(sdkconfig, fragment_file):

        # Match header [mapping]
        header = Suppress("[") + Suppress("mapping") + Suppress("]")

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
        default_condition = Optional(Suppress(":") + Literal(DeprecatedMapping.DEFAULT_CONDITION))

        normal_group = Group(normal_condition.setResultsName("condition") + mapping_entries)
        default_group = Group(default_condition + mapping_entries).setResultsName("default_group")

        normal_groups = Group(ZeroOrMore(normal_group)).setResultsName("normal_groups")

        # Any mapping fragment definition can have zero or more normal group and only one default group as a last entry.
        archive = Suppress("archive") + Suppress(":") + Fragment.ENTITY.setResultsName("archive")
        entries = Suppress("entries") + Suppress(":") + (normal_groups + default_group).setResultsName("entries")

        mapping = Group(header + archive + entries)
        mapping.ignore("#" + restOfLine)

        def parsed_deprecated_mapping(pstr, loc, toks):
            fragment = Mapping()
            fragment.archive = toks[0].archive
            fragment.name = re.sub(r"[^0-9a-zA-Z]+", "_", fragment.archive)
            fragment.deprecated = True

            fragment.entries = set()
            condition_true = False
            for entries in toks[0].entries[0]:
                condition  = next(iter(entries.condition.asList())).strip()
                condition_val = sdkconfig.evaluate_expression(condition)

                if condition_val:
                    for entry in entries[1]:
                        fragment.entries.add((entry.object, None if entry.symbol == '' else entry.symbol, entry.scheme))
                    condition_true = True
                    break

            if not fragment.entries and not condition_true:
                try:
                    entries = toks[0].entries[1][1]
                except IndexError:
                    entries = toks[0].entries[1][0]
                for entry in entries:
                    fragment.entries.add((entry.object, None if entry.symbol == '' else entry.symbol, entry.scheme))

            if not fragment.entries:
                fragment.entries.add(("*", None, "default"))

            dep_warning = str(ParseFatalException(pstr, loc,
                              "Warning: Deprecated old-style mapping fragment parsed in file %s." % fragment_file))

            print(dep_warning)
            return fragment

        mapping.setParseAction(parsed_deprecated_mapping)
        return mapping


FRAGMENT_TYPES = {
    "sections": Sections,
    "scheme": Scheme,
    "mapping": Mapping
}

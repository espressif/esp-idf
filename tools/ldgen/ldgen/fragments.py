#
# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import abc
import os
import re
from collections import namedtuple
from enum import Enum

from pyparsing import (Combine, Forward, Group, Keyword, Literal, OneOrMore, Optional, Or, ParseFatalException,
                       Suppress, Word, ZeroOrMore, alphanums, alphas, delimitedList, indentedBlock, nums,
                       originalTextFor, restOfLine)

from .entity import Entity
from .sdkconfig import SDKConfig


class FragmentFile():
    """
    Processes a fragment file and stores all parsed fragments. For
    more information on how this class interacts with classes for the different fragment types,
    see description of Fragment.
    """

    def __init__(self, fragment_file, sdkconfig):
        try:
            fragment_file = open(fragment_file, 'r')
        except TypeError:
            pass

        path = os.path.realpath(fragment_file.name)

        indent_stack = [1]

        class parse_ctx:
            fragment = None  # current fragment
            key = ''  # current key
            keys = list()  # list of keys parsed
            key_grammar = None  # current key grammar

            @staticmethod
            def reset():
                parse_ctx.fragment_instance = None
                parse_ctx.key = ''
                parse_ctx.keys = list()
                parse_ctx.key_grammar = None

        def fragment_type_parse_action(toks):
            parse_ctx.reset()
            parse_ctx.fragment = FRAGMENT_TYPES[toks[0]]()  # create instance of the fragment
            return None

        def expand_conditionals(toks, stmts):
            try:
                stmt = toks['value']
                stmts.append(stmt)
            except KeyError:
                try:
                    conditions = toks['conditional']
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
                raise ParseFatalException(pstr, loc, "unable to parse key '%s'; %s" % (key, str(e)))

            key_stmt << (conditional | Group(key_grammar).setResultsName('value'))

            return None

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
                raise ParseFatalException(pstr, loc, "unable to add key '%s'; %s" % (parse_ctx.key, str(e)))
            return None

        key = (Word(alphanums + '_') + Suppress(':')).setParseAction(key_parse_action)
        key_stmt = Forward()

        condition_block = indentedBlock(key_stmt, indent_stack)
        key_stmts = OneOrMore(condition_block)
        key_body = Suppress(key) + key_stmts
        key_body.setParseAction(key_body_parsed)

        condition = originalTextFor(SDKConfig.get_expression_grammar()).setResultsName('condition')
        if_condition = Group(Suppress('if') + condition + Suppress(':') + condition_block)
        elif_condition = Group(Suppress('elif') + condition + Suppress(':') + condition_block)
        else_condition = Group(Suppress('else') + Suppress(':') + condition_block)
        conditional = (if_condition + Optional(OneOrMore(elif_condition)) + Optional(else_condition)).setResultsName(
            'conditional')

        def name_parse_action(pstr, loc, toks):
            parse_ctx.fragment.name = toks[0]

        ftype = Word(alphas).setParseAction(fragment_type_parse_action)
        fid = Suppress(':') + Word(alphanums + '_.').setResultsName('name')
        fid.setParseAction(name_parse_action)
        header = Suppress('[') + ftype + fid + Suppress(']')

        def fragment_parse_action(pstr, loc, toks):
            key_grammars = parse_ctx.fragment.get_key_grammars()
            required_keys = set([k for (k, v) in key_grammars.items() if v.required])
            present_keys = required_keys.intersection(set(parse_ctx.keys))
            if present_keys != required_keys:
                raise ParseFatalException(pstr, loc, 'required keys %s for fragment not found' %
                                          list(required_keys - present_keys))
            return parse_ctx.fragment

        fragment_stmt = Forward()
        fragment_block = indentedBlock(fragment_stmt, indent_stack)

        fragment_if_condition = Group(Suppress('if') + condition + Suppress(':') + fragment_block)
        fragment_elif_condition = Group(Suppress('elif') + condition + Suppress(':') + fragment_block)
        fragment_else_condition = Group(Suppress('else') + Suppress(':') + fragment_block)
        fragment_conditional = (fragment_if_condition + Optional(OneOrMore(fragment_elif_condition)) +
                                Optional(fragment_else_condition)).setResultsName('conditional')

        fragment = (header + OneOrMore(indentedBlock(key_body, indent_stack, False))).setResultsName('value')
        fragment.setParseAction(fragment_parse_action)
        fragment.ignore('#' + restOfLine)

        deprecated_mapping = DeprecatedMapping.get_fragment_grammar(sdkconfig, fragment_file.name).setResultsName(
            'value')

        fragment_stmt << (Group(deprecated_mapping) | Group(fragment) | Group(fragment_conditional))

        def fragment_stmt_parsed(pstr, loc, toks):
            stmts = list()
            expand_conditionals(toks, stmts)
            return stmts

        parser = ZeroOrMore(fragment_stmt).setParseAction(fragment_stmt_parsed)

        self.fragments = parser.parseFile(fragment_file, parseAll=True)

        for fragment in self.fragments:
            fragment.path = path


class Fragment:
    """
    Base class for a fragment that can be parsed from a fragment file. All fragments
    share the common grammar:

    [type:name]
    key1:value1
    key2:value2
    ...

    Supporting a new fragment type means deriving a concrete class which specifies
    key-value pairs that the fragment supports and what to do with the parsed key-value pairs.

    The new fragment must also be appended to FRAGMENT_TYPES, specifying the
    keyword for the type and the derived class.

    The key of the key-value pair is a simple keyword string. Other parameters
    that describe the key-value pair is specified in Fragment.KeyValue:
        1. grammar - pyparsing grammar to parse the value of key-value pair
        2. min - the minimum number of value in the key entry, None means no minimum
        3. max - the maximum number of value in the key entry, None means no maximum
        4. required - if the key-value pair is required in the fragment

    Setting min=max=1 means that the key has a single value.

    FragmentFile provides conditional expression evaluation, enforcing
    the parameters for Fragment.Keyvalue.
    """
    __metaclass__ = abc.ABCMeta

    KeyValue = namedtuple('KeyValue', 'grammar min max required')

    IDENTIFIER = Word(alphas + '_', alphanums + '_')
    ENTITY = Word(alphanums + '.-_$+')

    @abc.abstractmethod
    def set_key_value(self, key, parse_results):
        pass

    @abc.abstractmethod
    def get_key_grammars(self):
        pass


class Sections(Fragment):
    """
    Fragment which contains list of input sections.

    [sections:<name>]
    entries:
        .section1
        .section2
        ...
    """

    # Unless quoted, symbol names start with a letter, underscore, or point
    # and may include any letters, underscores, digits, points, and hyphens.
    GNU_LD_SYMBOLS = Word(alphas + '_.', alphanums + '._-')

    entries_grammar = Combine(GNU_LD_SYMBOLS + Optional('+'))

    grammars = {
        'entries': Fragment.KeyValue(entries_grammar.setResultsName('section'), 1, None, True)
    }

    """
    Utility function that returns a list of sections given a sections fragment entry,
    with the '+' notation and symbol concatenation handled automatically.
    """

    @staticmethod
    def get_section_data_from_entry(sections_entry, symbol=None):
        if not symbol:
            sections = list()
            sections.append(sections_entry.replace('+', ''))
            sections.append(sections_entry.replace('+', '.*'))
            return sections
        else:
            if sections_entry.endswith('+'):
                section = sections_entry.replace('+', '.*')
                expansion = section.replace('.*', '.' + symbol)
                return (section, expansion)
            else:
                return (sections_entry, None)

    def set_key_value(self, key, parse_results):
        if key == 'entries':
            self.entries = set()
            for result in parse_results:
                self.entries.add(result['section'])

    def get_key_grammars(self):
        return self.__class__.grammars


class Scheme(Fragment):
    """
    Fragment which defines where the input sections defined in a Sections fragment
    is going to end up, the target. The targets are markers in a linker script template
    (see LinkerScript in linker_script.py).

    [scheme:<name>]
    entries:
        sections1 -> target1
        ...
    """

    grammars = {
        'entries': Fragment.KeyValue(Fragment.IDENTIFIER.setResultsName('sections') + Suppress('->') +
                                     Fragment.IDENTIFIER.setResultsName('target'), 1, None, True)
    }

    def set_key_value(self, key, parse_results):
        if key == 'entries':
            self.entries = set()
            for result in parse_results:
                self.entries.add((result['sections'], result['target']))

    def get_key_grammars(self):
        return self.__class__.grammars


class Mapping(Fragment):
    """
    Fragment which attaches a scheme to entities (see Entity in entity.py), specifying where the input
    sections of the entity will end up.

    [mapping:<name>]
    archive: lib1.a
    entries:
        obj1:symbol1 (scheme1); section1 -> target1 KEEP SURROUND(sym1) ...
        obj2 (scheme2)
        ...

    Ultimately, an `entity (scheme)` entry generates an
    input section description (see https://sourceware.org/binutils/docs/ld/Input-Section.html)
    in the output linker script. It is possible to attach 'flags' to the
    `entity (scheme)` to generate different output commands or to
    emit additional keywords in the generated input section description. The
    input section description, as well as other output commands, is defined in
    output_commands.py.
    """

    class Flag():
        PRE_POST = (Optional(Suppress(',') + Suppress('pre').setParseAction(lambda: True).setResultsName('pre')) +
                    Optional(Suppress(',') + Suppress('post').setParseAction(lambda: True).setResultsName('post')))

    class Surround(Flag):
        def __init__(self, symbol):
            self.symbol = symbol
            self.pre = True
            self.post = True

        @staticmethod
        def get_grammar():
            # SURROUND(symbol)
            #
            # '__symbol_start', '__symbol_end' is generated before and after
            # the corresponding input section description, respectively.
            grammar = (Keyword('SURROUND').suppress() +
                       Suppress('(') +
                       Fragment.IDENTIFIER.setResultsName('symbol') +
                       Suppress(')'))

            grammar.setParseAction(lambda tok: Mapping.Surround(tok.symbol))
            return grammar

        def __eq__(self, other):
            return (isinstance(other, Mapping.Surround) and
                    self.symbol == other.symbol)

    class Align(Flag):

        def __init__(self, alignment, pre=True, post=False):
            self.alignment = alignment
            self.pre = pre
            self.post = post

        @staticmethod
        def get_grammar():
            # ALIGN(alignment, [, pre, post]).
            #
            # Generates alignment command before and/or after the corresponding
            # input section description, depending whether pre, post or
            # both are specified.
            grammar = (Keyword('ALIGN').suppress() +
                       Suppress('(') +
                       Word(nums).setResultsName('alignment') +
                       Mapping.Flag.PRE_POST +
                       Suppress(')'))

            def on_parse(tok):
                alignment = int(tok.alignment)
                if tok.pre == '' and tok.post == '':
                    res = Mapping.Align(alignment)
                elif tok.pre != '' and tok.post == '':
                    res = Mapping.Align(alignment, tok.pre)
                elif tok.pre == '' and tok.post != '':
                    res = Mapping.Align(alignment, False, tok.post)
                else:
                    res = Mapping.Align(alignment, tok.pre, tok.post)
                return res

            grammar.setParseAction(on_parse)
            return grammar

        def __eq__(self, other):
            return (isinstance(other, Mapping.Align) and
                    self.alignment == other.alignment and
                    self.pre == other.pre and
                    self.post == other.post)

    class Keep(Flag):

        def __init__(self):
            pass

        @staticmethod
        def get_grammar():
            # KEEP()
            #
            # Surrounds input section description with KEEP command.
            grammar = Keyword('KEEP()').setParseAction(Mapping.Keep)
            return grammar

        def __eq__(self, other):
            return isinstance(other, Mapping.Keep)

    class Sort(Flag):
        class Type(Enum):
            NAME = 0
            ALIGNMENT = 1
            INIT_PRIORITY = 2

        def __init__(self, first, second=None):
            self.first = first
            self.second = second

        @staticmethod
        def get_grammar():
            # SORT([sort_by_first, sort_by_second])
            #
            # where sort_by_first, sort_by_second = {name, alignment, init_priority}
            #
            # Emits SORT_BY_NAME, SORT_BY_ALIGNMENT or SORT_BY_INIT_PRIORITY
            # depending on arguments. Nested sort follows linker script rules.
            keywords = Keyword('name') | Keyword('alignment') | Keyword('init_priority')
            grammar = (Keyword('SORT').suppress() + Suppress('(') +
                       keywords.setResultsName('first') +
                       Optional(Suppress(',') + keywords.setResultsName('second')) + Suppress(')'))

            grammar.setParseAction(lambda tok: Mapping.Sort(tok.first, tok.second if tok.second != '' else None))
            return grammar

        def __eq__(self, other):
            return (isinstance(other, Mapping.Sort) and
                    self.first == other.first and
                    self.second == other.second)

    def __init__(self):
        Fragment.__init__(self)
        self.entries = set()
        # k = (obj, symbol, scheme)
        # v = list((section, target), Mapping.Flag))
        self.flags = dict()
        self.deprecated = False

    def set_key_value(self, key, parse_results):
        if key == 'archive':
            self.archive = parse_results[0]['archive']
        elif key == 'entries':
            for result in parse_results:
                obj = None
                symbol = None
                scheme = None

                obj = result['object']

                try:
                    symbol = result['symbol']
                except KeyError:
                    pass

                scheme = result['scheme']

                mapping = (obj, symbol, scheme)
                self.entries.add(mapping)

                try:
                    parsed_flags = result['sections_target_flags']
                except KeyError:
                    parsed_flags = []

                if parsed_flags:
                    entry_flags = []
                    for pf in parsed_flags:
                        entry_flags.append((pf.sections, pf.target, list(pf.flags)))

                    try:
                        existing_flags = self.flags[mapping]
                    except KeyError:
                        existing_flags = list()
                        self.flags[mapping] = existing_flags

                    existing_flags.extend(entry_flags)

    def get_key_grammars(self):
        # There are three possible patterns for mapping entries:
        #       obj:symbol (scheme)
        #       obj (scheme)
        #       * (scheme)
        # Flags can be specified for section->target in the scheme specified, ex:
        #       obj (scheme); section->target SURROUND(symbol), section2->target2 ALIGN(4)
        obj = Fragment.ENTITY.setResultsName('object')
        symbol = Suppress(':') + Fragment.IDENTIFIER.setResultsName('symbol')
        scheme = Suppress('(') + Fragment.IDENTIFIER.setResultsName('scheme') + Suppress(')')

        # The flags are specified for section->target in the scheme specified
        sections_target = Scheme.grammars['entries'].grammar

        flag = Or([f.get_grammar() for f in [Mapping.Keep, Mapping.Align, Mapping.Surround, Mapping.Sort]])

        section_target_flags = Group(sections_target + Group(OneOrMore(flag)).setResultsName('flags'))

        pattern1 = obj + symbol
        pattern2 = obj
        pattern3 = Literal(Entity.ALL).setResultsName('object')

        entry = ((pattern1 | pattern2 | pattern3) + scheme +
                 Optional(Suppress(';') + delimitedList(section_target_flags).setResultsName('sections_target_flags')))

        grammars = {
            'archive': Fragment.KeyValue(Or([Fragment.ENTITY, Word(Entity.ALL)]).setResultsName('archive'), 1, 1,
                                         True),
            'entries': Fragment.KeyValue(entry, 0, None, True)
        }

        return grammars


class DeprecatedMapping():
    """
    Mapping fragment with old grammar in versions older than ESP-IDF v4.0. Does not conform to
    requirements of the Fragment class and thus is limited when it comes to conditional expression
    evaluation.
    """

    # Name of the default condition entry
    DEFAULT_CONDITION = 'default'

    @staticmethod
    def get_fragment_grammar(sdkconfig, fragment_file):

        # Match header [mapping]
        header = Suppress('[') + Suppress('mapping') + Suppress(']')

        # There are three possible patterns for mapping entries:
        #       obj:symbol (scheme)
        #       obj (scheme)
        #       * (scheme)
        obj = Fragment.ENTITY.setResultsName('object')
        symbol = Suppress(':') + Fragment.IDENTIFIER.setResultsName('symbol')
        scheme = Suppress('(') + Fragment.IDENTIFIER.setResultsName('scheme') + Suppress(')')

        pattern1 = Group(obj + symbol + scheme)
        pattern2 = Group(obj + scheme)
        pattern3 = Group(Literal(Entity.ALL).setResultsName('object') + scheme)

        mapping_entry = pattern1 | pattern2 | pattern3

        # To simplify parsing, classify groups of condition-mapping entry into two types: normal and default
        # A normal grouping is one with a non-default condition. The default grouping is one which contains the
        # default condition
        mapping_entries = Group(ZeroOrMore(mapping_entry)).setResultsName('mappings')

        normal_condition = Suppress(':') + originalTextFor(SDKConfig.get_expression_grammar())
        default_condition = Optional(Suppress(':') + Literal(DeprecatedMapping.DEFAULT_CONDITION))

        normal_group = Group(normal_condition.setResultsName('condition') + mapping_entries)
        default_group = Group(default_condition + mapping_entries).setResultsName('default_group')

        normal_groups = Group(ZeroOrMore(normal_group)).setResultsName('normal_groups')

        # Any mapping fragment definition can have zero or more normal group and only one default group as a last entry.
        archive = Suppress('archive') + Suppress(':') + Fragment.ENTITY.setResultsName('archive')
        entries = Suppress('entries') + Suppress(':') + (normal_groups + default_group).setResultsName('entries')

        mapping = Group(header + archive + entries)
        mapping.ignore('#' + restOfLine)

        def parsed_deprecated_mapping(pstr, loc, toks):
            fragment = Mapping()
            fragment.archive = toks[0].archive
            fragment.name = re.sub(r'[^0-9a-zA-Z]+', '_', fragment.archive)
            fragment.deprecated = True

            fragment.entries = set()
            condition_true = False
            for entries in toks[0].entries[0]:
                condition = next(iter(entries.condition.asList())).strip()
                condition_val = sdkconfig.evaluate_expression(condition)

                if condition_val:
                    for entry in entries[1]:
                        fragment.entries.add(
                            (entry.object, None if entry.symbol == '' else entry.symbol, entry.scheme))
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
                fragment.entries.add(('*', None, 'default'))

            dep_warning = str(ParseFatalException(pstr, loc,
                                                  'Warning: Deprecated old-style mapping fragment parsed in file %s.' % fragment_file))

            print(dep_warning)
            return fragment

        mapping.setParseAction(parsed_deprecated_mapping)
        return mapping


FRAGMENT_TYPES = {
    'sections': Sections,
    'scheme': Scheme,
    'mapping': Mapping
}

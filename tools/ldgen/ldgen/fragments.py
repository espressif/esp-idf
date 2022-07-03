#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

from typing import Any, Dict, List, Optional, Set, Tuple, Union

from pyparsing import (Combine, Forward, Group, IndentedBlock, Keyword, LineEnd, Literal, OneOrMore, Opt,
                       ParseFatalException, SkipTo, Suppress, Word, ZeroOrMore, alphanums, alphas, delimited_list,
                       nums, rest_of_line)


class Empty:
    """
    Return `Empty()` when the sdkconfig does not meet the conditional statements.
    """

    def __repr__(self):
        return '<EMPTY>'

    def __bool__(self):
        return False


class Fragment:
    """
    Base class for a fragment that can be parsed from a fragment file.
    """
    IDENTIFIER = Word(alphas + '_', alphanums + '_')
    ENTITY = Word(alphanums + '.-_$+')

    def __init__(self, name: str, entries: Set[Union[str, Tuple[str]]]):
        self.name = name
        self.entries = entries
        self.path = ''

    def __repr__(self):
        return str(self.__dict__)


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
    ENTRY = Combine(Word(alphas + '_.', alphanums + '._-') + Opt('+')) + LineEnd().suppress()

    @staticmethod
    def parse_entry(toks):
        # section
        return toks[0]

    @staticmethod
    def parse(s, loc, toks):
        this = toks[0]

        name = this[0]
        entries = {entry for entry in this[1] if entry}

        if not entries:
            raise ParseFatalException(s, loc, 'Sections entries shouldn\'t be empty')

        return Sections(name, entries)

    @staticmethod
    def get_section_data_from_entry(sections_entry, symbol=None):
        """
        Returns a list of sections given a sections fragment entry,
        with the '+' notation and symbol concatenation handled automatically.
        """
        if not symbol:
            sections = list()
            sections.append(sections_entry.replace('+', ''))
            sections.append(sections_entry.replace('+', '.*'))
            return sections
        else:
            if sections_entry.endswith('+'):
                section = sections_entry.replace('+', '.*')
                expansion = section.replace('.*', '.' + symbol)
                return section, expansion
            else:
                return sections_entry, None


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
    ENTRY = Fragment.IDENTIFIER + Suppress('->') + Fragment.IDENTIFIER + LineEnd().suppress()

    @staticmethod
    def parse_entry(toks):
        # section, target
        return toks[0], toks[1]

    @staticmethod
    def parse(s, loc, toks):
        this = toks[0]

        name = this[0]
        entries = {entry for entry in this[1] if entry}

        if not entries:
            raise ParseFatalException(s, loc, 'Scheme entries shouldn\'t be empty')

        return Scheme(name, entries)


class EntryFlag:
    def __repr__(self):
        return str(self.__dict__)


class Surround(EntryFlag):
    """
    SURROUND(symbol)

    '__symbol_start', '__symbol_end' is generated before and after
    the corresponding input section description, respectively.
    """
    SURROUND = (Keyword('SURROUND').suppress()
                + Suppress('(')
                + Fragment.IDENTIFIER
                + Suppress(')'))

    def __init__(self, symbol: str):
        self.symbol = symbol
        self.pre = True
        self.post = True

    def __eq__(self, other):
        if isinstance(other, Surround):
            if self.symbol == other.symbol and self.pre == other.pre and self.post == other.post:
                return True

        return False

    @staticmethod
    def parse(toks):
        return Surround(toks[0])


class Align(EntryFlag):
    """
    ALIGN(alignment, [, pre, post]).

    Generates alignment command before and/or after the corresponding
    input section description, depending on whether pre, post or
    both are specified.
    """
    PRE = Opt(Suppress(',') + Suppress('pre')).set_results_name('pre')
    POST = Opt(Suppress(',') + Suppress('post')).set_results_name('post')

    ALIGN = (Keyword('ALIGN').suppress()
             + Suppress('(')
             + Word(nums)
             + PRE
             + POST
             + Suppress(')'))

    def __init__(self, alignment, pre=True, post=False):
        self.alignment = alignment
        self.pre = pre
        self.post = post

    def __eq__(self, other):
        if isinstance(other, Align):
            if self.alignment == other.alignment and self.pre == other.pre and self.post == other.post:
                return True

        return False

    @staticmethod
    def parse(toks):
        alignment = int(toks[0])
        if toks.post == '':
            return Align(alignment)

        if toks.pre == '' and toks.post != '':
            return Align(alignment, False, True)

        return Align(alignment, True, True)


class Keep(EntryFlag):
    """
    KEEP()

    Surrounds input section description with KEEP command.
    """
    KEEP = Keyword('KEEP()')

    def __eq__(self, other):
        if isinstance(other, Keep):
            return True

        return False

    @staticmethod
    def parse():
        return Keep()


class Sort(EntryFlag):
    """
    SORT([sort_by_first, sort_by_second])

    where sort_by_first, sort_by_second = {name, alignment, init_priority}

    Emits SORT_BY_NAME, SORT_BY_ALIGNMENT or SORT_BY_INIT_PRIORITY
    depending on arguments. Nested sort follows linker script rules.
    """
    _keywords = Keyword('name') | Keyword('alignment') | Keyword('init_priority')
    SORT = (Keyword('SORT').suppress()
            + Suppress('(')
            + _keywords.set_results_name('first')
            + Opt(Suppress(',') + _keywords.set_results_name('second'))
            + Suppress(')'))

    def __init__(self, first: str, second: Optional[str] = None):
        self.first = first
        self.second = second

    def __eq__(self, other):
        if isinstance(other, Sort):
            if self.first == other.first and self.second == other.second:
                return True

        return False

    @staticmethod
    def parse(toks):
        return Sort(toks.first, toks.second or None)


class Flag:
    _section_target = Fragment.IDENTIFIER + Suppress('->') + Fragment.IDENTIFIER
    _flag = (Surround.SURROUND.set_parse_action(Surround.parse)
             | Align.ALIGN.set_parse_action(Align.parse)
             | Keep.KEEP.set_parse_action(Keep.parse)
             | Sort.SORT.set_parse_action(Sort.parse))

    FLAG = _section_target + OneOrMore(_flag)

    def __init__(self, section: str, target: str, flags: List[EntryFlag]):
        self.section = section
        self.target = target
        self.flags = flags

    def __eq__(self, other):
        if isinstance(other, Flag):
            if self.section == other.section and self.target == other.target and len(self.flags) == len(other.flags):
                for i, j in zip(self.flags, other.flags):
                    if i != j:
                        break
                else:
                    return True

        return False

    @staticmethod
    def parse(toks):
        return Flag(toks[0], toks[1], toks[2:])

    def __repr__(self):
        return str(self.__dict__)


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

    _any = Literal('*')
    _obj = Word(alphas + '_', alphanums + '-_').set_results_name('object')
    _sym = Fragment.IDENTIFIER.set_results_name('symbol')

    # There are three possible patterns for mapping entries:
    #       obj:symbol (scheme)
    #       obj (scheme)
    #       * (scheme)
    _entry = (((_obj + Opt(Suppress(':') + _sym)) | _any.set_results_name('object'))
              + Suppress('(')
              + Fragment.IDENTIFIER.set_results_name('section')
              + Suppress(')'))

    ENTRY = _entry + LineEnd().suppress()
    ARCHIVE = (Word(alphanums + '.-_$+') | Literal('*')) + LineEnd().suppress()

    # Flags can be specified for section->target in the scheme specified, ex:
    #       obj (scheme);
    #           section->target SURROUND(symbol),
    #           section2->target2 ALIGN(4)
    ENTRY_WITH_FLAG = (_entry + Suppress(';')
                       + delimited_list(Flag.FLAG.set_parse_action(Flag.parse)))

    def __init__(self, archive: str, flags: Dict[Any, Flag], *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.archive = archive
        self.flags = flags

    @staticmethod
    def parse_archive(s, loc, toks):
        this = toks[0][0]
        if len(this) != 1:
            raise ParseFatalException(s, loc, 'Could only specify one archive file in one mapping fragment')

        return this[0]

    @staticmethod
    def parse_entry(toks):
        return toks.object, toks.symbol or None, toks.section

    @staticmethod
    def parse_entry_with_flag(toks):
        entry = toks.object, toks.symbol or None, toks.section
        return {
            entry: [tok for tok in toks if isinstance(tok, Flag)]
        }

    @staticmethod
    def parse_entries(toks):
        return toks[0]

    @staticmethod
    def parse(toks):
        this = toks[0]

        name = this[0]
        archive = this[1]
        entries_or_dict_with_flags = this[2]

        entries = set()
        flags = dict()
        for item in entries_or_dict_with_flags:
            if isinstance(item, Empty):
                continue
            elif isinstance(item, dict):  # entry with flags
                for k, v in item.items():
                    entries.add(k)
                    if k in flags:
                        flags[k].extend(v)
                    else:
                        flags[k] = v
            else:
                entries.add(item)

        return Mapping(archive=archive, name=name, entries=entries, flags=flags)


class FragmentFile:
    """
    Processes a fragment file and stores all parsed fragments. For
    more information on how this class interacts with classes for the different fragment types,
    see description of Fragment.
    """

    def __init__(self, fragments: List[Fragment]):
        self.path = None  # assign later, couldn't pass extra argument while parsing
        self.fragments: List[Fragment] = fragments

    def __repr__(self):
        return str(self.__dict__)


def parse_fragment_file(path, sdkconfig):
    def parse_conditional(toks):
        this = toks[0]
        for stmt in this:
            if stmt[0] in ['if', 'elif']:  # if/elif
                if sdkconfig.evaluate_expression(stmt.condition):
                    return stmt[-1]
            else:  # else
                return stmt[-1]

        return Empty()

    def get_conditional_stmt(_stmt):
        condition = SkipTo(':').set_results_name('condition') + Suppress(':')
        _suite = IndentedBlock(_stmt)

        if_decl = Literal('if') + condition
        elif_decl = Literal('elif') + condition
        else_decl = Literal('else:')
        if_ = Group(if_decl + _suite)
        elif_ = Group(elif_decl + _suite)
        else_ = Group(else_decl + _suite)
        return Group(if_ + Opt(OneOrMore(elif_)) + Opt(else_)).set_parse_action(parse_conditional)

    def get_suite(_stmt):
        __stmt = Forward()
        __conditional = get_conditional_stmt(__stmt)
        __stmt <<= (comment
                    | _stmt
                    | __conditional)
        return IndentedBlock(__stmt)

    def parse(toks):
        return FragmentFile([tok for tok in toks if not isinstance(tok, Empty)])

    # comment
    comment = (Literal('#') + rest_of_line).set_parse_action(lambda s, l, t: Empty())

    # section
    section_entry = Sections.ENTRY.set_parse_action(Sections.parse_entry)
    section_entries_suite = get_suite(section_entry)
    section_header = Suppress('[sections:') + Fragment.IDENTIFIER + Suppress(']') + LineEnd().suppress()
    section = Group(section_header
                    + Suppress('entries:')
                    + section_entries_suite).set_parse_action(Sections.parse)

    # scheme
    scheme_entry = Scheme.ENTRY.set_parse_action(Scheme.parse_entry)
    scheme_entries_suite = get_suite(scheme_entry)
    scheme_header = Suppress('[scheme:') + Fragment.IDENTIFIER + Suppress(']') + LineEnd().suppress()
    scheme = Group(scheme_header
                   + Suppress('entries:')
                   + scheme_entries_suite).set_parse_action(Scheme.parse)
    # mapping
    mapping_archive = Mapping.ARCHIVE
    mapping_archive_suite = get_suite(mapping_archive)

    mapping_entry = Mapping.ENTRY.set_parse_action(Mapping.parse_entry)
    mapping_entry_with_flag = Mapping.ENTRY_WITH_FLAG.set_parse_action(Mapping.parse_entry_with_flag)
    mapping_entries_suite = get_suite(mapping_entry | mapping_entry_with_flag)

    mapping_header = Suppress('[mapping:') + Fragment.IDENTIFIER + Suppress(']')
    mapping = Group(mapping_header
                    + Group(Suppress('archive:')
                            + mapping_archive_suite).set_parse_action(Mapping.parse_archive)
                    + Group(Suppress('entries:')
                            + mapping_entries_suite).set_parse_action(Mapping.parse_entries)
                    ).set_parse_action(Mapping.parse)

    # highest level
    fragment = (section
                | scheme
                | mapping
                | get_conditional_stmt(section | scheme | mapping))
    parser = ZeroOrMore(fragment).ignore(comment).set_parse_action(parse)
    fragment_file = parser.parse_file(path, parse_all=True)[0]
    fragment_file.path = path

    for frag in fragment_file.fragments:
        frag.path = path

    return fragment_file

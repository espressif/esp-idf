#
# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import collections
import fnmatch
import os
from enum import Enum
from functools import total_ordering

from pyparsing import (Group, Literal, OneOrMore, ParseException, Regex, SkipTo, Suppress, White, Word, ZeroOrMore,
                       alphas, nums, rest_of_line)


@total_ordering
class Entity:
    """
    An entity refers to a library, object, symbol whose input
    sections can be placed or excluded from placement.

    An important property of an entity is its specificity - the granularity
    of the the entity to be placed. Specificity increases in the following
    order: library, object, symbol. An entity with no specificity refers
    to all entities.
    """

    ALL = '*'

    class Specificity(Enum):
        NONE = 0
        ARCHIVE = 1
        OBJ = 2
        SYMBOL = 3

    def __init__(self, archive=None, obj=None, symbol=None):
        archive_spec = archive and archive != Entity.ALL
        obj_spec = obj and obj != Entity.ALL
        symbol_spec = symbol and symbol != Entity.ALL

        if not archive_spec and not obj_spec and not symbol_spec:
            self.specificity = Entity.Specificity.NONE
        elif archive_spec and not obj_spec and not symbol_spec:
            self.specificity = Entity.Specificity.ARCHIVE
        elif archive_spec and obj_spec and not symbol_spec:
            self.specificity = Entity.Specificity.OBJ
        elif archive_spec and obj_spec and symbol_spec:
            self.specificity = Entity.Specificity.SYMBOL
        else:
            raise ValueError("Invalid arguments '(%s, %s, %s)'" % (archive, obj, symbol))

        self.archive = archive
        self.obj = obj
        self.symbol = symbol

    def __eq__(self, other):
        return (self.specificity.value == other.specificity.value and
                self.archive == other.archive and
                self.obj == other.obj and
                self.symbol == other.symbol)

    def __lt__(self, other):
        res = False
        if self.specificity.value < other.specificity.value:
            res = True
        elif self.specificity == other.specificity:
            for s in Entity.Specificity:
                a = self[s] if self[s] else ''
                b = other[s] if other[s] else ''

                if a != b:
                    res = a < b
                    break
        else:
            res = False
        return res

    def __hash__(self):
        return hash(self.__repr__())

    def __str__(self):
        return '%s:%s %s' % self.__repr__()

    def __repr__(self):
        return self.archive, self.obj, self.symbol

    def __getitem__(self, spec):
        res = None
        if spec == Entity.Specificity.ARCHIVE:
            res = self.archive
        elif spec == Entity.Specificity.OBJ:
            res = self.obj
        elif spec == Entity.Specificity.SYMBOL:
            res = self.symbol
        else:
            res = None
        return res


class EntityDB:
    """
    Collection of entities extracted from libraries known in the build.
    Allows retrieving a list of archives, a list of object files in an archive
    or a list of symbols in an archive; as well as allows for checking if an
    entity exists in the collection.
    """

    __info = collections.namedtuple('__info', 'filename content')

    def __init__(self):
        self.sections = dict()

    def add_sections_info(self, sections_info_dump):
        first_line = sections_info_dump.readline()

        archive_path = (Literal('In archive').suppress() +
                        White().suppress() +
                        # trim the colon and line ending characters from archive_path
                        rest_of_line.set_results_name('archive_path').set_parse_action(
                            lambda s, loc, toks: s.rstrip(':\n\r ')))
        parser = archive_path

        try:
            results = parser.parseString(first_line, parseAll=True)
        except ParseException as p:
            raise ParseException('Parsing sections info for library ' + sections_info_dump.name + ' failed. ' + p.msg)

        archive = os.path.basename(results.archive_path)
        self.sections[archive] = EntityDB.__info(sections_info_dump.name, sections_info_dump.read())

    def _get_infos_from_file(self, info):
        # {object}:  file format elf32-xtensa-le
        object_line = SkipTo(':').set_results_name('object') + Suppress(rest_of_line)

        # Sections:
        # Idx Name ...
        section_start = Suppress(Literal('Sections:'))
        section_header = Suppress(OneOrMore(Word(alphas)))

        # 00 {section} 0000000 ...
        #              CONTENTS, ALLOC, ....
        section_entry = (Suppress(Word(nums)) + Regex(r'\.\S+') + Suppress(rest_of_line)
                         + Suppress(ZeroOrMore(Word(alphas) + Literal(',')) + Word(alphas)))

        content = Group(object_line
                        + section_start
                        + section_header
                        + Group(OneOrMore(section_entry)).set_results_name('sections'))
        parser = Group(ZeroOrMore(content)).set_results_name('contents')

        try:
            results = parser.parseString(info.content, parseAll=True)
        except ParseException as p:
            raise ParseException('Unable to parse section info file ' + info.filename + '. ' + p.msg)

        return results

    def _process_archive(self, archive):
        stored = self.sections[archive]

        # Parse the contents of the sections file on-demand,
        # save the result for later
        if not isinstance(stored, dict):
            parsed = self._get_infos_from_file(stored)
            stored = dict()
            for content in parsed.contents:
                sections = list(map(lambda s: s, content.sections))
                stored[content.object] = sections
            self.sections[archive] = stored

    def get_archives(self):
        return self.sections.keys()

    def get_objects(self, archive):
        try:
            self._process_archive(archive)
        except KeyError:
            return []

        return self.sections[archive].keys()

    def _match_obj(self, archive, obj):
        objs = self.get_objects(archive)
        match_objs = (fnmatch.filter(objs, obj + '.*.o')
                      + fnmatch.filter(objs, obj + '.o')
                      + fnmatch.filter(objs, obj + '.*.obj')
                      + fnmatch.filter(objs, obj + '.obj'))

        if len(match_objs) > 1:
            raise ValueError("Multiple matches for object: '%s: %s': %s" % (archive, obj, str(match_objs)))

        try:
            return match_objs[0]
        except IndexError:
            return None

    def get_sections(self, archive, obj):
        obj = self._match_obj(archive, obj)
        res = []
        if obj:
            res = self.sections[archive][obj]
        return res

    def _match_symbol(self, archive, obj, symbol):
        sections = self.get_sections(archive, obj)
        return [s for s in sections if s.endswith(symbol)]

    def check_exists(self, entity):
        res = True

        if entity.specificity != Entity.Specificity.NONE:
            if entity.specificity == Entity.Specificity.ARCHIVE:
                res = entity.archive in self.get_archives()
            elif entity.specificity == Entity.Specificity.OBJ:
                res = self._match_obj(entity.archive, entity.obj) is not None
            elif entity.specificity == Entity.Specificity.SYMBOL:
                res = len(self._match_symbol(entity.archive, entity.obj, entity.symbol)) > 0
            else:
                res = False

        return res

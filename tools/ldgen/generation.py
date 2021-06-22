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

import collections
import itertools
import os
import fnmatch

from fragments import Sections, Scheme, Mapping, Fragment
from pyparsing import Suppress, White, ParseException, Literal, Group, ZeroOrMore
from pyparsing import Word, OneOrMore, nums, alphas, restOfLine, SkipTo
from ldgen_common import LdGenFailure


class PlacementRule():
    """
    Encapsulates a generated placement rule placed under a target
    """

    DEFAULT_SPECIFICITY = 0
    ARCHIVE_SPECIFICITY = 1
    OBJECT_SPECIFICITY = 2
    SYMBOL_SPECIFICITY = 3

    class __container():
        def __init__(self, content):
            self.content = content

    __metadata = collections.namedtuple("__metadata", "excludes expansions expanded")

    def __init__(self, archive, obj, symbol, sections, target):
        if archive == "*":
            archive = None

        if obj == "*":
            obj = None

        self.archive = archive
        self.obj = obj
        self.symbol = symbol
        self.target = target
        self.sections = dict()

        self.specificity = 0
        self.specificity += 1 if self.archive else 0
        self.specificity += 1 if (self.obj and not self.obj == '*') else 0
        self.specificity += 1 if self.symbol else 0

        for section in sections:
            section_data = Sections.get_section_data_from_entry(section, self.symbol)

            if not self.symbol:
                for s in section_data:
                    metadata = self.__metadata(self.__container([]), self.__container([]), self.__container(False))
                    self.sections[s] = metadata
            else:
                (section, expansion) = section_data
                if expansion:
                    metadata = self.__metadata(self.__container([]), self.__container([expansion]), self.__container(True))
                    self.sections[section] = metadata

    def get_section_names(self):
        return self.sections.keys()

    def add_exclusion(self, other, sections_infos=None):
        # Utility functions for this method
        def do_section_expansion(rule, section):
            if section in rule.get_section_names():
                sections_in_obj = sections_infos.get_obj_sections(rule.archive, rule.obj)
                expansions = fnmatch.filter(sections_in_obj, section)
                return expansions

        def remove_section_expansions(rule, section, expansions):
            existing_expansions = self.sections[section].expansions.content
            self.sections[section].expansions.content = [e for e in existing_expansions if e not in expansions]

        # Exit immediately if the exclusion to be added is more general than this rule.
        if not other.is_more_specific_rule_of(self):
            return

        for section in self.get_sections_intersection(other):
            if(other.specificity == PlacementRule.SYMBOL_SPECIFICITY):
                # If this sections has not been expanded previously, expand now and keep track.
                previously_expanded = self.sections[section].expanded.content
                if not previously_expanded:
                    expansions = do_section_expansion(self, section)
                    if expansions:
                        self.sections[section].expansions.content = expansions
                        self.sections[section].expanded.content = True
                        previously_expanded = True

                # Remove the sections corresponding to the symbol name
                remove_section_expansions(self, section, other.sections[section].expansions.content)

                # If it has been expanded previously but now the expansions list is empty,
                # it means adding exclusions has exhausted the list. Remove the section entirely.
                if previously_expanded and not self.sections[section].expanded.content:
                    del self.sections[section]
            else:
                # A rule section can have multiple rule sections excluded from it. Get the
                # most specific rule from the list, and if an even more specific rule is found,
                # replace it entirely. Otherwise, keep appending.
                exclusions = self.sections[section].excludes
                exclusions_list = exclusions.content if exclusions.content is not None else []
                exclusions_to_remove = filter(lambda r: r.is_more_specific_rule_of(other), exclusions_list)

                remaining_exclusions = [e for e in exclusions_list if e not in exclusions_to_remove]
                remaining_exclusions.append(other)

                self.sections[section].excludes.content = remaining_exclusions

    def get_sections_intersection(self, other):
        return set(self.sections.keys()).intersection(set(other.sections.keys()))

    def is_more_specific_rule_of(self, other):
        if (self.specificity <= other.specificity):
            return False

        # Compare archive, obj and target
        for entity_index in range(1, other.specificity + 1):
            if self[entity_index] != other[entity_index] and other[entity_index] is not None:
                return False

        return True

    def maps_same_entities_as(self, other):
        if self.specificity != other.specificity:
            return False

        # Compare archive, obj and target
        for entity_index in range(1, other.specificity + 1):
            if self[entity_index] != other[entity_index] and other[entity_index] is not None:
                return False

        return True

    def __getitem__(self, key):
        if key == PlacementRule.ARCHIVE_SPECIFICITY:
            return self.archive
        elif key == PlacementRule.OBJECT_SPECIFICITY:
            return self.obj
        elif key == PlacementRule.SYMBOL_SPECIFICITY:
            return self.symbol
        else:
            return None

    def __str__(self):
        sorted_sections = sorted(self.get_section_names())

        sections_string = list()

        for section in sorted_sections:
            exclusions = self.sections[section].excludes.content

            exclusion_string = None

            if exclusions:
                exclusion_string = " ".join(map(lambda e: "*" + e.archive + (":" + e.obj + ".*" if e.obj else ""), exclusions))
                exclusion_string = "EXCLUDE_FILE(" + exclusion_string + ")"
            else:
                exclusion_string = ""

            section_string = None
            exclusion_section_string = None

            section_expansions = self.sections[section].expansions.content
            section_expanded = self.sections[section].expanded.content

            if section_expansions and section_expanded:
                section_string = " ".join(section_expansions)
                exclusion_section_string = section_string
            else:
                section_string = section
                exclusion_section_string = exclusion_string + " " + section_string

            sections_string.append(exclusion_section_string)

        sections_string = " ".join(sections_string)

        archive = str(self.archive) if self.archive else ""
        obj = (str(self.obj) + (".*" if self.obj else "")) if self.obj else ""

        # Handle output string generation based on information available
        if self.specificity == PlacementRule.DEFAULT_SPECIFICITY:
            rule_string = "*(%s)" % (sections_string)
        elif self.specificity == PlacementRule.ARCHIVE_SPECIFICITY:
            rule_string = "*%s:(%s)" % (archive, sections_string)
        else:
            rule_string = "*%s:%s(%s)" % (archive, obj, sections_string)

        return rule_string

    def __eq__(self, other):
        if id(self) == id(other):
            return True

        def exclusions_set(exclusions):
            exclusions_set = {(e.archive, e.obj, e.symbol, e.target) for e in exclusions}
            return exclusions_set

        if self.archive != other.archive:
            return False

        if self.obj != other.obj:
            return False

        if self.symbol != other.symbol:
            return False

        if set(self.sections.keys()) != set(other.sections.keys()):
            return False

        for (section, metadata) in self.sections.items():

            self_meta = metadata
            other_meta = other.sections[section]

            if exclusions_set(self_meta.excludes.content) != exclusions_set(other_meta.excludes.content):
                return False

            if set(self_meta.expansions.content) != set(other_meta.expansions.content):
                return False

        return True

    def __ne__(self, other):
        return not self.__eq__(other)

    def __iter__(self):
        yield self.archive
        yield self.obj
        yield self.symbol
        raise StopIteration


class GenerationModel:
    """
    Implements generation of placement rules based on collected sections, scheme and mapping fragment.
    """

    DEFAULT_SCHEME = "default"

    def __init__(self, check_mappings=False, check_mapping_exceptions=None):
        self.schemes = {}
        self.sections = {}
        self.mappings = {}

        self.check_mappings = check_mappings

        if check_mapping_exceptions:
            self.check_mapping_exceptions = check_mapping_exceptions
        else:
            self.check_mapping_exceptions = []

    def _add_mapping_rules(self, archive, obj, symbol, scheme_name, scheme_dict, rules):
        # Use an ordinary dictionary to raise exception on non-existing keys
        temp_dict = dict(scheme_dict)

        sections_bucket = temp_dict[scheme_name]

        for (target, sections) in sections_bucket.items():
            section_entries = []

            for section in sections:
                section_entries.extend(section.entries)

            rule = PlacementRule(archive, obj, symbol, section_entries, target)

            if rule not in rules:
                rules.append(rule)

    def _build_scheme_dictionary(self):
        scheme_dictionary = collections.defaultdict(dict)

        # Collect sections into buckets based on target name
        for scheme in self.schemes.values():
            sections_bucket = collections.defaultdict(list)

            for (sections_name, target_name) in scheme.entries:
                # Get the sections under the bucket 'target_name'. If this bucket does not exist
                # is is created automatically
                sections_in_bucket = sections_bucket[target_name]

                try:
                    sections = self.sections[sections_name]
                except KeyError:
                    message = GenerationException.UNDEFINED_REFERENCE + " to sections '" + sections + "'."
                    raise GenerationException(message, scheme)

                sections_in_bucket.append(sections)

            scheme_dictionary[scheme.name] = sections_bucket

        # Search for and raise exception on first instance of sections mapped to multiple targets
        for (scheme_name, sections_bucket) in scheme_dictionary.items():
            for sections_a, sections_b in itertools.combinations(sections_bucket.values(), 2):
                set_a = set()
                set_b = set()

                for sections in sections_a:
                    set_a.update(sections.entries)

                for sections in sections_b:
                    set_b.update(sections.entries)

                intersection = set_a.intersection(set_b)

                # If the intersection is a non-empty set, it means sections are mapped to multiple
                # targets. Raise exception.
                if intersection:
                    scheme = self.schemes[scheme_name]
                    message = "Sections " + str(intersection) + " mapped to multiple targets."
                    raise GenerationException(message, scheme)

        return scheme_dictionary

    def generate_rules(self, sections_infos):
        scheme_dictionary = self._build_scheme_dictionary()

        # Generate default rules
        default_rules = list()
        self._add_mapping_rules(None, None, None, GenerationModel.DEFAULT_SCHEME, scheme_dictionary, default_rules)

        all_mapping_rules = collections.defaultdict(list)

        # Generate rules based on mapping fragments
        for mapping in self.mappings.values():
            archive = mapping.archive
            mapping_rules = all_mapping_rules[archive]
            for (obj, symbol, scheme_name) in mapping.entries:
                try:
                    if not (obj == Mapping.MAPPING_ALL_OBJECTS and symbol is None and
                            scheme_name == GenerationModel.DEFAULT_SCHEME):
                        if self.check_mappings and mapping.name not in self.check_mapping_exceptions:
                            if not obj == Mapping.MAPPING_ALL_OBJECTS:
                                obj_sections = sections_infos.get_obj_sections(archive, obj)
                                if not obj_sections:
                                    message = "'%s:%s' not found" % (archive, obj)
                                    raise GenerationException(message, mapping)

                                if symbol:
                                    obj_sym = fnmatch.filter(obj_sections, "*%s" % symbol)
                                    if not obj_sym:
                                        message = "'%s:%s %s' not found" % (archive, obj, symbol)
                                        raise GenerationException(message, mapping)

                        self._add_mapping_rules(archive, obj, symbol, scheme_name, scheme_dictionary, mapping_rules)
                except KeyError:
                    message = GenerationException.UNDEFINED_REFERENCE + " to scheme '" + scheme_name + "'."
                    raise GenerationException(message, mapping)

        # Detect rule conflicts
        for mapping_rules in all_mapping_rules.items():
            self._detect_conflicts(mapping_rules)

        # Add exclusions
        for mapping_rules in all_mapping_rules.values():
            self._create_exclusions(mapping_rules, default_rules, sections_infos)

        placement_rules = collections.defaultdict(list)

        # Add the default rules grouped by target
        for default_rule in default_rules:
            existing_rules = placement_rules[default_rule.target]
            if default_rule.get_section_names():
                existing_rules.append(default_rule)

        archives = sorted(all_mapping_rules.keys())

        for archive in archives:
            # Add the mapping rules grouped by target
            mapping_rules = sorted(all_mapping_rules[archive], key=lambda m: (m.specificity, str(m)))
            for mapping_rule in mapping_rules:
                existing_rules = placement_rules[mapping_rule.target]
                if mapping_rule.get_section_names():
                    existing_rules.append(mapping_rule)

        return placement_rules

    def _detect_conflicts(self, rules):
        (archive, rules_list) = rules

        for specificity in range(0, PlacementRule.OBJECT_SPECIFICITY + 1):
            rules_with_specificity = filter(lambda r: r.specificity == specificity, rules_list)

            for rule_a, rule_b in itertools.combinations(rules_with_specificity, 2):
                intersections = rule_a.get_sections_intersection(rule_b)

                if intersections and rule_a.maps_same_entities_as(rule_b):
                    rules_string = str([str(rule_a), str(rule_b)])
                    message = "Rules " + rules_string + " map sections " + str(list(intersections)) + " into multiple targets."
                    raise GenerationException(message)

    def _create_extra_rules(self, rules):
        # This function generates extra rules for symbol specific rules. The reason for generating extra rules is to isolate,
        # as much as possible, rules that require expansion. Particularly, object specific extra rules are generated.
        rules_to_process = sorted(rules, key=lambda r: r.specificity)
        symbol_specific_rules = list(filter(lambda r: r.specificity == PlacementRule.SYMBOL_SPECIFICITY, rules_to_process))

        extra_rules = dict()

        for symbol_specific_rule in symbol_specific_rules:
            extra_rule_candidate = {s: None for s in symbol_specific_rule.get_section_names()}

            super_rules = filter(lambda r: symbol_specific_rule.is_more_specific_rule_of(r), rules_to_process)

            # Take a look at the existing rules that are more general than the current symbol-specific rule.
            # Only generate an extra rule if there is no existing object specific rule for that section
            for super_rule in super_rules:
                intersections = symbol_specific_rule.get_sections_intersection(super_rule)
                for intersection in intersections:
                    if super_rule.specificity != PlacementRule.OBJECT_SPECIFICITY:
                        extra_rule_candidate[intersection] = super_rule
                    else:
                        extra_rule_candidate[intersection] = None

            # Generate the extra rules for the symbol specific rule section, keeping track of the generated extra rules
            for (section, section_rule) in extra_rule_candidate.items():
                if section_rule:
                    extra_rule = None
                    extra_rules_key = (symbol_specific_rule.archive, symbol_specific_rule.obj, section_rule.target)

                    try:
                        extra_rule = extra_rules[extra_rules_key]

                        if section not in extra_rule.get_section_names():
                            new_rule = PlacementRule(extra_rule.archive, extra_rule.obj, extra_rule.symbol,
                                                     list(extra_rule.get_section_names()) + [section], extra_rule.target)
                            extra_rules[extra_rules_key] = new_rule
                    except KeyError:
                        extra_rule = PlacementRule(symbol_specific_rule.archive, symbol_specific_rule.obj, None, [section], section_rule.target)
                        extra_rules[extra_rules_key] = extra_rule

        return extra_rules.values()

    def _create_exclusions(self, mapping_rules, default_rules, sections_info):
        rules = list(default_rules)
        rules.extend(mapping_rules)

        extra_rules = self._create_extra_rules(rules)

        mapping_rules.extend(extra_rules)
        rules.extend(extra_rules)

        # Sort the rules by means of how specific they are. Sort by specificity from lowest to highest
        # * -> lib:* -> lib:obj -> lib:obj:symbol
        sorted_rules = sorted(rules, key=lambda r: r.specificity)

        # Now that the rules have been sorted, loop through each rule, and then loop
        # through rules below it (higher indeces), adding exclusions whenever appropriate.
        for general_rule in sorted_rules:
            for specific_rule in reversed(sorted_rules):
                if (specific_rule.specificity > general_rule.specificity and
                    specific_rule.specificity != PlacementRule.SYMBOL_SPECIFICITY) or \
                    (specific_rule.specificity == PlacementRule.SYMBOL_SPECIFICITY and
                     general_rule.specificity == PlacementRule.OBJECT_SPECIFICITY):
                    general_rule.add_exclusion(specific_rule, sections_info)

    def add_fragments_from_file(self, fragment_file):
        for fragment in fragment_file.fragments:
            dict_to_append_to = None

            if isinstance(fragment, Mapping) and fragment.deprecated and fragment.name in self.mappings.keys():
                self.mappings[fragment.name].entries |= fragment.entries
            else:
                if isinstance(fragment, Scheme):
                    dict_to_append_to = self.schemes
                elif isinstance(fragment, Sections):
                    dict_to_append_to = self.sections
                else:
                    dict_to_append_to = self.mappings

                # Raise exception when the fragment of the same type is already in the stored fragments
                if fragment.name in dict_to_append_to.keys():
                    stored = dict_to_append_to[fragment.name].path
                    new = fragment.path
                    message = "Duplicate definition of fragment '%s' found in %s and %s." % (fragment.name, stored, new)
                    raise GenerationException(message)

                dict_to_append_to[fragment.name] = fragment


class TemplateModel:
    """
    Encapsulates a linker script template file. Finds marker syntax and handles replacement to generate the
    final output.
    """

    Marker = collections.namedtuple("Marker", "target indent rules")

    def __init__(self, template_file):
        self.members = []
        self.file = os.path.realpath(template_file.name)

        self._generate_members(template_file)

    def _generate_members(self, template_file):
        lines = template_file.readlines()

        target = Fragment.IDENTIFIER
        reference = Suppress("mapping") + Suppress("[") + target.setResultsName("target") + Suppress("]")
        pattern = White(" \t").setResultsName("indent") + reference

        # Find the markers in the template file line by line. If line does not match marker grammar,
        # set it as a literal to be copied as is to the output file.
        for line in lines:
            try:
                parsed = pattern.parseString(line)

                indent = parsed.indent
                target = parsed.target

                marker = TemplateModel.Marker(target, indent, [])

                self.members.append(marker)
            except ParseException:
                # Does not match marker syntax
                self.members.append(line)

    def fill(self, mapping_rules):
        for member in self.members:
            target = None
            try:
                target = member.target
                rules = member.rules

                del rules[:]

                rules.extend(mapping_rules[target])
            except KeyError:
                message = GenerationException.UNDEFINED_REFERENCE + " to target '" + target + "'."
                raise GenerationException(message)
            except AttributeError:
                pass

    def write(self, output_file):
        # Add information that this is a generated file.
        output_file.write("/* Automatically generated file; DO NOT EDIT */\n")
        output_file.write("/* Espressif IoT Development Framework Linker Script */\n")
        output_file.write("/* Generated from: %s */\n" % self.file)
        output_file.write("\n")

        # Do the text replacement
        for member in self.members:
            try:
                indent = member.indent
                rules = member.rules

                for rule in rules:
                    generated_line = "".join([indent, str(rule), '\n'])
                    output_file.write(generated_line)
            except AttributeError:
                output_file.write(member)


class GenerationException(LdGenFailure):
    """
    Exception for linker script generation failures such as undefined references/ failure to
    evaluate conditions, duplicate mappings, etc.
    """

    UNDEFINED_REFERENCE = "Undefined reference"

    def __init__(self, message, fragment=None):
        self.fragment = fragment
        self.message = message

    def __str__(self):
        if self.fragment:
            return "%s\nIn fragment '%s' defined in '%s'." % (self.message, self.fragment.name, self.fragment.path)
        else:
            return self.message


class SectionsInfo(dict):
    """
    Encapsulates an output of objdump. Contains information about the static library sections
    and names
    """

    __info = collections.namedtuple("__info", "filename content")

    def __init__(self):
        self.sections = dict()

    def add_sections_info(self, sections_info_dump):
        first_line = sections_info_dump.readline()

        archive_path = (Literal("In archive").suppress() +
                        White().suppress() +
                        # trim the colon and line ending characters from archive_path
                        restOfLine.setResultsName("archive_path").setParseAction(lambda s, loc, toks: s.rstrip(":\n\r ")))
        parser = archive_path

        results = None

        try:
            results = parser.parseString(first_line, parseAll=True)
        except ParseException as p:
            raise ParseException("Parsing sections info for library " + sections_info_dump.name + " failed. " + p.message)

        archive = os.path.basename(results.archive_path)
        self.sections[archive] = SectionsInfo.__info(sections_info_dump.name, sections_info_dump.read())

    def _get_infos_from_file(self, info):
        # {object}:  file format elf32-xtensa-le
        object_line = SkipTo(":").setResultsName("object") + Suppress(restOfLine)

        # Sections:
        # Idx Name ...
        section_start = Suppress(Literal("Sections:"))
        section_header = Suppress(OneOrMore(Word(alphas)))

        # 00 {section} 0000000 ...
        #              CONTENTS, ALLOC, ....
        section_entry = Suppress(Word(nums)) + SkipTo(' ') + Suppress(restOfLine) + \
            Suppress(ZeroOrMore(Word(alphas) + Literal(",")) + Word(alphas))

        content = Group(object_line + section_start + section_header + Group(OneOrMore(section_entry)).setResultsName("sections"))
        parser = Group(ZeroOrMore(content)).setResultsName("contents")

        results = None

        try:
            results = parser.parseString(info.content, parseAll=True)
        except ParseException as p:
            raise ParseException("Unable to parse section info file " + info.filename + ". " + p.message)

        return results

    def get_obj_sections(self, archive, obj):
        res = []
        try:
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

            try:
                res = stored[obj + ".o"]
            except KeyError:
                try:
                    res = stored[obj + ".c.obj"]
                except KeyError:
                    try:
                        res = stored[obj + ".cpp.obj"]
                    except KeyError:
                        res = stored[obj + ".S.obj"]
        except KeyError:
            pass

        return res

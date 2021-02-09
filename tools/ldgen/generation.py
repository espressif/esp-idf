#
# Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
import fnmatch
import itertools
from collections import namedtuple

from entity import Entity
from fragments import Mapping, Scheme, Sections
from ldgen_common import LdGenFailure
from output_commands import InputSectionDesc


class RuleNode():

    class Section():

        def __init__(self, target, exclusions, explicit=False):
            self.target = target
            self.exclusions = set(exclusions)

            # Indicate whether this node has been created explicitly from a mapping,
            # or simply just to create a path to the explicitly created node.
            #
            # For example,
            #
            # lib.a
            #       obj:sym (scheme)
            #
            # Nodes for lib.a and obj will be created, but only the node for
            # sym will have been created explicitly.
            #
            # This is used in deciding whether or not an output command should
            # be emitted for this node, or for exclusion rule generation.
            self.explicit = explicit

    def __init__(self, parent, name, sections):
        self.children = []
        self.parent = parent
        self.name = name
        self.child_node = None
        self.entity = None

        self.sections = dict()

        # A node inherits the section -> target entries from
        # its parent. This is to simplify logic, avoiding
        # going up the parental chain to try a 'basis' rule
        # in creating exclusions. This relies on the fact that
        # the mappings must be inserted from least to most specific.
        # This sort is done in generate_rules().
        if sections:
            for (s, v) in sections.items():
                self.sections[s] = RuleNode.Section(v.target, [], [])

    def add_exclusion(self, sections, exclusion):
        self.sections[sections].exclusions.add(exclusion)

        # Recursively create exclusions in parents
        if self.parent:
            self.exclude_from_parent(sections)

    def add_sections(self, sections, target):
        try:
            _sections = self.sections[sections]
            if not _sections.explicit:
                _sections.target = target
                _sections.explicit = True
            else:
                if target != _sections.target:
                    raise GenerationException('Sections mapped to multiple targets')
        except KeyError:
            self.sections[sections] = RuleNode.Section(target, [], True)

    def exclude_from_parent(self, sections):
        self.parent.add_exclusion(sections, self.entity)

    def add_child(self, entity):
        child_specificity = self.entity.specificity.value + 1
        assert(child_specificity <= Entity.Specificity.SYMBOL.value)
        name = entity[Entity.Specificity(child_specificity)]
        assert(name and name != Entity.ALL)

        child = [c for c in self.children if c.name == name]
        assert(len(child) <= 1)

        if not child:
            child = self.child_node(self, name, self.sections)
            self.children.append(child)
        else:
            child = child[0]

        return child

    def get_output_commands(self):
        commands = collections.defaultdict(list)

        def process_commands(cmds):
            for (target, commands_list) in cmds.items():
                commands[target].extend(commands_list)

        # Process the commands generated from this node
        node_commands = self.get_node_output_commands()
        process_commands(node_commands)

        # Process the commands generated from this node's children
        # recursively
        for child in sorted(self.children, key=lambda c: c.name):
            children_commands = child.get_output_commands()
            process_commands(children_commands)

        return commands

    def add_node_child(self, entity, sections, target, sections_db):
        child = self.add_child(entity)
        child.insert(entity, sections, target, sections_db)

    def get_node_output_commands(self):
        commands = collections.defaultdict(list)

        for sections in self.get_section_keys():
            info = self.sections[sections]
            if info.exclusions or info.explicit:
                command = InputSectionDesc(self.entity, sections, info.exclusions)
                commands[info.target].append(command)

        return commands

    def insert(self, entity, sections, target, sections_db):
        if self.entity.specificity == entity.specificity:
            if self.parent.sections[sections].target != target:
                self.add_sections(sections, target)
                self.exclude_from_parent(sections)
        else:
            self.add_node_child(entity, sections, target, sections_db)

    def get_section_keys(self):
        return sorted(self.sections.keys(), key=' '.join)


class SymbolNode(RuleNode):

    def __init__(self, parent, name, sections):
        RuleNode.__init__(self, parent, name, sections)
        self.entity = Entity(self.parent.parent.name, self.parent.name, self.name)

    def insert(self, entity, sections, target, sections_db):
        self.add_sections(sections, target)

    def get_node_output_commands(self):
        commands = collections.defaultdict(list)

        for sections in self.get_section_keys():
            info = self.sections[sections]
            if info.explicit:
                command = InputSectionDesc(Entity(self.parent.parent.name, self.parent.name), sections, [])
                commands[info.target].append(command)

        return commands


class ObjectNode(RuleNode):

    def __init__(self, parent, name, sections):
        RuleNode.__init__(self, parent, name, sections)
        self.child_node = SymbolNode
        self.expanded_sections = dict()
        self.entity = Entity(self.parent.name, self.name)

    def add_node_child(self, entity, sections, target, sections_db):
        if self.sections[sections].target != target:
            symbol = entity.symbol
            match_sections = None

            obj_sections = sections_db.get_sections(self.parent.name, self.name)

            try:
                match_sections = self.expanded_sections[sections]
            except KeyError:
                match_sections = []
                for s in sections:
                    match_sections.extend(fnmatch.filter(obj_sections, s))

            if match_sections:
                remove_sections = [s.replace('.*', '.%s' % symbol) for s in sections if '.*' in s]
                filtered_sections = [s for s in match_sections if s not in remove_sections]

                if set(filtered_sections) != set(match_sections):  # some sections removed
                    child = self.add_child(entity)
                    child.insert(entity, frozenset(remove_sections), target, obj_sections)

                    # Remember the result for node command generation
                    self.expanded_sections[sections] = filtered_sections
                    self.exclude_from_parent(sections)

    def get_node_output_commands(self):
        commands = collections.defaultdict(list)

        for sections in self.get_section_keys():
            info = self.sections[sections]

            try:
                match_sections = self.expanded_sections[sections]
            except KeyError:
                match_sections = []

            if match_sections or info.explicit:
                command_sections = match_sections if match_sections else sections
                command = InputSectionDesc(self.entity, command_sections, [])
                commands[info.target].append(command)

        return commands

    def exclude_from_parent(self, sections):
        # Check if there is an explicit emmission for the parent node, which is an archive node.
        # If there is, make the exclusion there. If not, make the exclusion on the root node.
        # This is to avoid emitting unecessary command and exclusions for the archive node and
        # from the root node, respectively.
        if self.parent.sections[sections].explicit:
            self.parent.add_exclusion(sections, self.entity)
        else:
            self.parent.parent.add_exclusion(sections, self.entity)


class ArchiveNode(RuleNode):

    def __init__(self, parent, name, sections):
        RuleNode.__init__(self, parent, name, sections)
        self.child_node = ObjectNode
        self.entity = Entity(self.name)


class RootNode(RuleNode):
    def __init__(self):
        RuleNode.__init__(self, None, Entity.ALL, None)
        self.child_node = ArchiveNode
        self.entity = Entity('*')

    def insert(self, entity, sections, target, sections_db):
        if self.entity.specificity == entity.specificity:
            self.add_sections(sections, target)
        else:
            self.add_node_child(entity, sections, target, sections_db)


class Generation:
    """
    Implements generation of placement rules based on collected sections, scheme and mapping fragment.
    """

    DEFAULT_SCHEME = 'default'

    # Processed mapping, scheme and section entries
    EntityMapping = namedtuple('EntityMapping', 'entity sections_group target')

    def __init__(self, check_mappings=False, check_mapping_exceptions=None):
        self.schemes = {}
        self.sections = {}
        self.mappings = {}

        self.check_mappings = check_mappings

        if check_mapping_exceptions:
            self.check_mapping_exceptions = check_mapping_exceptions
        else:
            self.check_mapping_exceptions = []

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
                    message = GenerationException.UNDEFINED_REFERENCE + " to sections '" + sections_name + "'."
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
                    message = 'Sections ' + str(intersection) + ' mapped to multiple targets.'
                    raise GenerationException(message, scheme)

        return scheme_dictionary

    def get_section_strs(self, section):
        s_list = [Sections.get_section_data_from_entry(s) for s in section.entries]
        return frozenset([item for sublist in s_list for item in sublist])

    def _generate_entity_mappings(self, scheme_dictionary, entities):
        entity_mappings = []

        for mapping in self.mappings.values():
            archive = mapping.archive

            for (obj, symbol, scheme_name) in mapping.entries:
                entity = Entity(archive, obj, symbol)

                # Check the entity exists
                if (self.check_mappings and
                        entity.specificity.value > Entity.Specificity.ARCHIVE.value and
                        mapping.name not in self.check_mapping_exceptions):
                    if not entities.check_exists(entity):
                        message = "'%s' not found" % str(entity)
                        raise GenerationException(message, mapping)

                # Create placement rule for each 'section -> target' in the scheme.
                #
                # For example. for the mapping entry:
                #
                # obj (scheme)
                #
                # The enumrated to:
                #
                # obj (section1 -> target1)
                # obj (section2 -> target2)
                # ...
                for (target, sections) in scheme_dictionary[scheme_name].items():
                    for section in sections:
                        entity_mappings.append(Generation.EntityMapping(entity, self.get_section_strs(section), target))

        return entity_mappings

    def generate_rules(self, entities):
        scheme_dictionary = self._build_scheme_dictionary()

        entity_mappings = self._generate_entity_mappings(scheme_dictionary, entities)

        entity_mappings.sort(key=lambda m: m.entity)

        # Create root nodes dictionary for the default scheme, whose
        # key is the target name and value is a list of the root nodes for that target.
        root_node = RootNode()
        for (target, sections) in scheme_dictionary['default'].items():
            for section in sections:
                root_node.insert(Entity(), self.get_section_strs(section), target, entities)

        for mapping in entity_mappings:
            (entity, sections, target) = mapping
            try:
                root_node.insert(entity, sections, target, entities)
            except ValueError as e:
                raise GenerationException(str(e))

        # Traverse the tree, creating the rules
        commands = root_node.get_output_commands()

        return commands

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


class GenerationException(LdGenFailure):
    """
    Exception for linker script generation failures such as undefined references/ failure to
    evaluate conditions, duplicate mappings, etc.
    """

    UNDEFINED_REFERENCE = 'Undefined reference'

    def __init__(self, message, fragment=None):
        self.fragment = fragment
        self.message = message

    def __str__(self):
        if self.fragment:
            return "%s\nIn fragment '%s' defined in '%s'." % (self.message, self.fragment.name, self.fragment.path)
        else:
            return self.message

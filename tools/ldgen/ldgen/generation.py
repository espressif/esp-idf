#
# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import collections
import fnmatch
import itertools
from collections import namedtuple

from .entity import Entity
from .fragments import Keep
from .fragments import Scheme
from .fragments import Sections
from .fragments import Sort
from .fragments import Surround
from .ldgen_common import LdGenFailure
from .output_commands import AlignAtAddress
from .output_commands import InputSectionDesc
from .output_commands import SymbolAtAddress


class Placement:
    """
    A Placement is an assignment of an entity's input sections to a target
    in the output linker script - a precursor to the input section description.

    A placement can be excluded from another placement. These are represented
    as contents of EXCLUDE_FILE in the input section description. Since the linker uses the
    first matching rule, these exclusions make sure that accidental matching
    of entities with higher specificity does not occur.

    The placement which a placement is excluded from is referred to as the
    'basis' placement. It operates on the same input section of the entity on
    one of the parent (or parent's parent and so forth), but might have
    a different target (see is_significant() for the criteria).

    A placement is explicit if it was derived from an actual entry in one of
    the mapping fragments. Just as intermediate entity nodes are created in some cases,
    intermediate placements are created particularly for symbol placements.
    The reason is that EXCLUDE_FILE does not work on symbols (see ObjectNode
    for details).
    """

    def __init__(self, node, sections, target, flags, explicit, force=False, dryrun=False):
        self.node = node
        self.sections = sections
        self.target = target
        self.flags = flags

        self.exclusions = set()
        self.subplacements = set()

        # Force this placement to be output
        self.force = force

        # This placement was created from a mapping
        # fragment entry.
        self.explicit = explicit

        # Find basis placement.
        parent = node.parent
        candidate = None
        while parent:
            try:
                candidate = parent.placements[sections]
            except KeyError:
                pass

            if candidate and candidate.is_significant():
                break
            else:
                parent = parent.parent

        self.basis = candidate

        if self.is_significant() and not dryrun and self.basis:
            self.basis.add_exclusion(self)

    def is_significant(self):
        # Check if the placement is significant. Significant placements
        # are the end of a basis chain (not self.basis) or a change
        # in target (self.target != self.basis.target)
        #
        # Placement can also be a basis if it has flags
        # (self.flags) or its basis has flags (self.basis.flags)
        return (not self.basis or
                self.target != self.basis.target or
                (self.flags and not self.basis.flags) or
                (not self.flags and self.basis.flags) or
                self.force)

    def force_significant(self):
        if not self.is_significant():
            self.force = True
            if self.basis:
                self.basis.add_exclusion(self)

    def add_exclusion(self, exclusion):
        self.exclusions.add(exclusion)

    def add_subplacement(self, subplacement):
        self.subplacements.add(subplacement)


class EntityNode:
    """
    Node in entity tree. An EntityNode
    is created from an Entity (see entity.py).

    The entity tree has a maximum depth of 3. Nodes at different
    depths are derived from this class for special behavior (see
    RootNode, ArchiveNode, ObjectNode, SymbolNode) depending
    on entity specificity.

    Nodes for entities are inserted at the appropriate depth, creating
    intermediate nodes along the path if necessary. For example, a node
    for entity `lib1.a:obj1:sym1` needs to be inserted. If the node for `lib1:obj1`
    does not exist, then it needs to be created.

    A node contains a dictionary of placements (see Placement).
    The key to this dictionary are contents of sections fragments,
    representing the input sections of an entity. For example,
    a node for entity `lib1.a` might have a placement entry for its `.text` input section
    in this dictionary. The placement will contain details about the
    target, the flags, etc.

    Generation of output commands to be written to the output linker script
    requires traversal of the tree, each node collecting the output commands
    from its children, so on and so forth.
    """

    def __init__(self, parent, name):
        self.children = []
        self.parent = parent
        self.name = name
        self.child_t = EntityNode
        self.entity = None
        self.placements = dict()

    def add_child(self, entity):
        child_specificity = self.entity.specificity.value + 1
        assert child_specificity <= Entity.Specificity.SYMBOL.value
        name = entity[Entity.Specificity(child_specificity)]
        assert name and name != Entity.ALL

        child = [c for c in self.children if c.name == name]
        assert (len(child) <= 1)

        if not child:
            child = self.child_t(self, name)
            self.children.append(child)
        else:
            child = child[0]

        return child

    def get_output_commands(self, non_contiguous_sram):
        commands = collections.defaultdict(list)

        def process_commands(cmds):
            for (target, commands_list) in cmds.items():
                commands[target].extend(commands_list)

        # Process the commands generated from this node
        node_commands = self.get_node_output_commands(non_contiguous_sram)
        process_commands(node_commands)

        # Process the commands generated from this node's children
        # recursively
        for child in sorted(self.children, key=lambda c: c.name):
            children_commands = child.get_output_commands(non_contiguous_sram)
            process_commands(children_commands)

        return commands

    def get_node_output_commands(self, non_contiguous_sram):
        commands = collections.defaultdict(list)

        for sections in self.get_output_sections():
            placement = self.placements[sections]
            if placement.is_significant():
                assert placement.node == self

                tied = False
                keep = False
                sort = None
                surround_type = []

                placement_flags = placement.flags if placement.flags is not None else []

                for flag in placement_flags:
                    if isinstance(flag, Keep):
                        keep = True
                    elif isinstance(flag, Sort):
                        sort = (flag.first, flag.second)
                    else:  # SURROUND or ALIGN
                        if non_contiguous_sram and isinstance(flag, Surround):
                            tied = True
                        surround_type.append(flag)

                for flag in surround_type:
                    if flag.pre:
                        if isinstance(flag, Surround):
                            commands[placement.target].append(SymbolAtAddress(f'_{flag.symbol}_start', tied))
                        else:  # ALIGN
                            commands[placement.target].append(AlignAtAddress(flag.alignment, tied))

                # This is for expanded object node and symbol node placements without checking for
                # the type.
                placement_sections = frozenset(placement.sections)
                command_sections = sections if sections == placement_sections else placement_sections

                command = InputSectionDesc(placement.node.entity, command_sections,
                                           [e.node.entity for e in placement.exclusions], keep, sort, tied)
                commands[placement.target].append(command)

                # Generate commands for intermediate, non-explicit exclusion placements here,
                # so that they can be enclosed by flags that affect the parent placement.
                for subplacement in placement.subplacements:
                    if not subplacement.flags and not subplacement.explicit:
                        command = InputSectionDesc(subplacement.node.entity, subplacement.sections,
                                                   [e.node.entity for e in subplacement.exclusions], keep, sort, tied)
                        commands[placement.target].append(command)

                for flag in surround_type:
                    if flag.post:
                        if isinstance(flag, Surround):
                            commands[placement.target].append(SymbolAtAddress(f'_{flag.symbol}_end', tied))
                        else:  # ALIGN
                            commands[placement.target].append(AlignAtAddress(flag.alignment, tied))

        return commands

    def self_placement(self, sections, target, flags, explicit=True, force=False):
        placement = Placement(self, sections, target, flags, explicit, force)
        self.placements[sections] = placement
        return placement

    def child_placement(self, entity, sections, target, flags, sections_db):
        child = self.add_child(entity)
        child.insert(entity, sections, target, flags, sections_db)

    def insert(self, entity, sections, target, flags, sections_db):
        if self.entity.specificity == entity.specificity:
            # Since specificities match, create the placement in this node.
            self.self_placement(sections, target, flags)
        else:
            # If not, create a child node and try to create the placement there.
            self.child_placement(entity, sections, target, flags, sections_db)

    def get_output_sections(self):
        return sorted(self.placements.keys(), key=lambda x: sorted(x))  # pylint: disable=W0108


class SymbolNode(EntityNode):
    """
    Entities at depth=3. Represents entities with archive, object
    and symbol specified.
    """

    def __init__(self, parent, name):
        EntityNode.__init__(self, parent, name)
        self.entity = Entity(self.parent.parent.name, self.parent.name)


class ObjectNode(EntityNode):
    """
    Entities at depth=2. Represents entities with archive
    and object specified.

    Creating a placement on a child node (SymbolNode) has a different behavior, since
    exclusions using EXCLUDE_FILE for symbols does not work.

    The sections of this entity has to be 'expanded'. That is, we must
    look into the actual input sections of this entity and remove
    the ones corresponding to the symbol. The remaining sections of an expanded
    object entity will be listed one-by-one in the corresponding
    input section description.

    An intermediate placement on this node is created, if one does not exist,
    and is the one excluded from its basis placement.
    """

    def __init__(self, parent, name):
        EntityNode.__init__(self, parent, name)
        self.child_t = SymbolNode
        self.entity = Entity(self.parent.name, self.name)
        self.subplacements = list()

    def child_placement(self, entity, sections, target, flags, sections_db):
        child = self.add_child(entity)
        sym_placement = Placement(child, sections, target, flags, True, dryrun=True)

        # The basis placement for sym_placement can either be
        # an existing placement on this node, or nonexistent.
        if sym_placement.is_significant():
            try:
                obj_sections = self.placements[sections].sections
            except KeyError:
                obj_sections = None

            if not obj_sections or obj_sections == sections:
                # Expand this section for the first time
                found_sections = sections_db.get_sections(self.parent.name, self.name)
                obj_sections = []
                for s in sections:
                    obj_sections.extend(fnmatch.filter(found_sections, s))

            if obj_sections:
                symbol = entity.symbol
                remove_sections = [s.replace('.*', '.%s' % symbol) for s in sections if '.*' in s]
                filtered_sections = [s for s in obj_sections if s not in remove_sections]

                if set(filtered_sections) != set(obj_sections):
                    if sym_placement.basis:
                        subplace = False
                        try:
                            # If existing placement exists, make sure that
                            # it is emitted.
                            obj_placement = self.placements[sections]
                        except KeyError:
                            # Create intermediate placement.
                            obj_placement = self.self_placement(sections, sym_placement.basis.target, None, False)
                            if obj_placement.basis.flags:
                                subplace = True

                        if subplace:
                            obj_placement.basis.add_subplacement(obj_placement)
                            self.subplacements.append(sections)
                        else:
                            obj_placement.force_significant()

                        obj_placement.sections = filtered_sections
                        sym_placement.basis = obj_placement

                    sym_placement.sections = remove_sections
                    child.placements[sections] = sym_placement

    def get_output_sections(self):
        output_sections = [key for key in self.placements if key not in self.subplacements]
        return sorted(output_sections, key=' '.join)


class ArchiveNode(EntityNode):
    """
    Entities at depth=1. Represents entities with archive specified.
    """

    def __init__(self, parent, name):
        EntityNode.__init__(self, parent, name)
        self.child_t = ObjectNode
        self.entity = Entity(self.name)


class RootNode(EntityNode):
    """
    Single entity at depth=0. Represents entities with no specific members
    specified.
    """

    def __init__(self):
        EntityNode.__init__(self, None, Entity.ALL)
        self.child_t = ArchiveNode
        self.entity = Entity(Entity.ALL)


class Generation:
    """
    Processes all fragments processed from fragment files included in the build.
    Generates output commands (see output_commands.py) that LinkerScript (see linker_script.py) can
    write to the output linker script.
    """

    # Processed mapping, scheme and section entries
    EntityMapping = namedtuple('EntityMapping', 'entity sections_group target flags')

    def __init__(self, check_mappings=False, check_mapping_exceptions=None):
        self.schemes = {}
        self.placements = {}
        self.mappings = {}

        self.check_mappings = check_mappings

        if check_mapping_exceptions:
            self.check_mapping_exceptions = check_mapping_exceptions
        else:
            self.check_mapping_exceptions = []

    def _prepare_scheme_dictionary(self):
        scheme_dictionary = collections.defaultdict(dict)

        # Collect sections into buckets based on target name
        for scheme in self.schemes.values():
            sections_bucket = collections.defaultdict(list)

            for (sections_name, target_name) in scheme.entries:
                # Get the sections under the bucket 'target_name'. If this bucket does not exist
                # is created automatically
                sections_in_bucket = sections_bucket[target_name]

                try:
                    sections = self.placements[sections_name]
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

    def _prepare_entity_mappings(self, scheme_dictionary, entities):
        # Prepare entity mappings processed from mapping fragment entries.
        def get_section_strs(section):
            s_list = [Sections.get_section_data_from_entry(s) for s in section.entries]
            return frozenset([item for sublist in s_list for item in sublist])

        entity_mappings = dict()

        for mapping in self.mappings.values():
            archive = mapping.archive

            for (obj, symbol, scheme_name) in mapping.entries:
                entity = Entity(archive, obj, symbol)

                # Check the entity exists
                if (self.check_mappings
                        and entity.specificity.value > Entity.Specificity.ARCHIVE.value
                        and mapping.name not in self.check_mapping_exceptions):
                    if not entities.check_exists(entity):
                        message = "'%s' not found" % str(entity)
                        raise GenerationException(message, mapping)

                if (obj, symbol, scheme_name) in mapping.flags.keys():
                    flags = mapping.flags[(obj, symbol, scheme_name)]
                    # Check if all section->target defined in the current
                    # scheme.
                    for flag in flags:
                        if (flag.target not in scheme_dictionary[scheme_name].keys()
                                or flag.section not in
                                [_s.name for _s in scheme_dictionary[scheme_name][flag.target]]):
                            message = "%s->%s not defined in scheme '%s'" % (flag.section, flag.target, scheme_name)
                            raise GenerationException(message, mapping)
                else:
                    flags = None

                # Create placement for each 'section -> target' in the scheme.
                for (target, sections) in scheme_dictionary[scheme_name].items():
                    for section in sections:
                        # Find the applicable flags
                        _flags = []

                        if flags:
                            for flag in flags:
                                if (flag.section, flag.target) == (section.name, target):
                                    _flags.extend(flag.flags)

                        sections_str = get_section_strs(section)

                        key = (entity, sections_str)

                        try:
                            existing = entity_mappings[key]
                        except KeyError:
                            existing = None

                        if not existing:
                            entity_mappings[key] = Generation.EntityMapping(entity, sections_str, target, _flags)
                        else:
                            # Check for conflicts.
                            if target != existing.target:
                                raise GenerationException('Sections mapped to multiple targets.', mapping)

                            # Combine flags here if applicable, to simplify
                            # insertion logic.
                            if _flags or existing.flags:
                                if (_flags and not existing.flags) or (not _flags and existing.flags):
                                    _flags.extend(existing.flags)
                                    entity_mappings[key] = Generation.EntityMapping(entity,
                                                                                    sections_str,
                                                                                    target, _flags)
                                elif _flags == existing.flags:
                                    pass
                                else:
                                    raise GenerationException('Conflicting flags specified.', mapping)

        # Sort the mappings by specificity, so as to simplify
        # insertion logic.
        res = list(entity_mappings.values())
        res.sort(key=lambda m: m.entity)
        return res

    def generate(self, entities, non_contiguous_sram):
        scheme_dictionary = self._prepare_scheme_dictionary()
        entity_mappings = self._prepare_entity_mappings(scheme_dictionary, entities)
        root_node = RootNode()
        for mapping in entity_mappings:
            (entity, sections, target, flags) = mapping
            try:
                root_node.insert(entity, sections, target, flags, entities)
            except ValueError as e:
                raise GenerationException(str(e))

        # Traverse the tree, creating the placements
        commands = root_node.get_output_commands(non_contiguous_sram)

        return commands

    def add_fragments_from_file(self, fragment_file):
        for fragment in fragment_file.fragments:
            if isinstance(fragment, Scheme):
                dict_to_append_to = self.schemes
            elif isinstance(fragment, Sections):
                dict_to_append_to = self.placements
            else:
                dict_to_append_to = self.mappings

            # Raise exception when the fragment of the same type is already in the stored fragments
            if fragment.name in dict_to_append_to:
                stored = dict_to_append_to[fragment.name].path
                new = fragment.path
                message = "Duplicate definition of fragment '%s' found in %s and %s." % (
                    fragment.name, stored, new)
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

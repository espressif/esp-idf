#
# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
from .entity import Entity

# Contains classes for output section commands referred to in
# https://www.acrc.bris.ac.uk/acrc/RedHat/rhel-ld-en-4/sections.html#OUTPUT-SECTION-DESCRIPTION.


class AlignAtAddress:
    """
    Outputs assignment of builtin function ALIGN to current
    position:

    . = ALIGN(<alignment>)

    Mapping fragment flag ALIGN causes this output section
    command to be emitted.
    """

    def __init__(self, alignment, tied=False):
        self.alignment = alignment
        self.tied = tied

    def __str__(self):
        return ('. = ALIGN(%d);' % self.alignment)

    def __eq__(self, other):
        return (isinstance(other, AlignAtAddress) and
                self.alignment == other.alignment)


class SymbolAtAddress:
    """
    Outputs assignment of builtin function ABSOLUTE to a symbol
    for current position:

    <symbol> = ABSOLUTE(.)

    Mapping fragment flag SURROUND causes this
    output section command to be emitted before and after
    an InputSectionDesc.
    """

    def __init__(self, symbol, tied=False):
        self.symbol = symbol
        self.tied = tied

    def __str__(self):
        return ('%s = ABSOLUTE(.);' % self.symbol)

    def __eq__(self, other):
        return (isinstance(other, SymbolAtAddress) and
                self.symbol == other.symbol)


class InputSectionDesc:
    """
    Outputs an input section description as described in
    https://www.acrc.bris.ac.uk/acrc/RedHat/rhel-ld-en-4/sections.html#INPUT-SECTION.

    These commands are emmited from mapping fragment entries, specifically attaching
    a scheme onto an entity. Mapping fragment flags KEEP, SORT will also affect
    the emitted input section description.
    """

    def __init__(self, entity, sections, exclusions=None, keep=False, sort=None, tied=False):
        assert entity.specificity != Entity.Specificity.SYMBOL

        self.entity = entity
        self.sections = set(sections)

        self.exclusions = set()

        if exclusions:
            assert not [e for e in exclusions if e.specificity == Entity.Specificity.SYMBOL or
                        e.specificity == Entity.Specificity.NONE]
            self.exclusions = set(exclusions)
        else:
            self.exclusions = set()

        self.keep = keep
        self.sort = sort
        self.tied = tied

    def __str__(self):
        sections_string = '( )'

        if self.sections:
            exclusion_strings = []

            for exc in sorted(self.exclusions):
                if exc.specificity == Entity.Specificity.ARCHIVE:
                    exc_string = '*%s' % (exc.archive)
                else:
                    exc_string = '*%s:%s.*' % (exc.archive, exc.obj)

                exclusion_strings.append(exc_string)

            section_strings = []

            if exclusion_strings:
                exclusion_string = 'EXCLUDE_FILE(%s)' % ' '.join(exclusion_strings)

                for section in sorted(self.sections):
                    section_strings.append('%s %s' % (exclusion_string, section))
            else:
                for section in sorted(self.sections):
                    section_strings.append(section)

            if self.sort:
                if self.sort == (None, None):
                    pattern = 'SORT(%s)'
                elif self.sort == ('name', None):
                    pattern = 'SORT_BY_NAME(%s)'
                elif self.sort == ('alignment', None):
                    pattern = 'SORT_BY_ALIGNMENT(%s)'
                elif self.sort == ('init_priority', None):
                    pattern = 'SORT_BY_INIT_PRIORITY(%s)'
                elif self.sort == ('name', 'alignment'):
                    pattern = 'SORT_BY_NAME(SORT_BY_ALIGNMENT(%s))'
                elif self.sort == ('alignment', 'name'):
                    pattern = 'SORT_BY_ALIGNMENT(SORT_BY_NAME(%s))'
                elif self.sort == ('name', 'name'):
                    pattern = 'SORT_BY_NAME(SORT_BY_NAME(%s))'
                elif self.sort == ('alignment', 'alignment'):
                    pattern = 'SORT_BY_ALIGNMENT(SORT_BY_ALIGNMENT(%s))'
                else:
                    raise Exception('Invalid sort arguments')

                section_strings = [(pattern % s) for s in section_strings]

            sections_string = '(%s)' % ' '.join(section_strings)

        if self.entity.specificity == Entity.Specificity.NONE:
            entry = '*%s' % (sections_string)
        elif self.entity.specificity == Entity.Specificity.ARCHIVE:
            entry = '*%s:%s' % (self.entity.archive, sections_string)
        else:
            entry = '*%s:%s.*%s' % (self.entity.archive, self.entity.obj, sections_string)

        if self.keep:
            res = 'KEEP(%s)' % entry
        else:
            res = entry

        return res

    def __eq__(self, other):
        return (isinstance(other, InputSectionDesc) and
                self.entity == other.entity and
                self.sections == other.sections and
                self.exclusions == other.exclusions and
                self.keep == other.keep and
                self.sort == other.sort)

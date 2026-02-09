#
# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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

    def __init__(self, alignment, tied=False, mutable=False):
        self.alignment = alignment
        self.tied = tied
        self.mutable = mutable

    def __str__(self):
        return f'. = ALIGN({self.alignment});'

    def __eq__(self, other):
        return isinstance(other, AlignAtAddress) and self.alignment == other.alignment


class SymbolAtAddress:
    """
    Outputs assignment of builtin function ABSOLUTE to a symbol
    for current position:

    <symbol> = ABSOLUTE(.)

    Mapping fragment flag SURROUND causes this
    output section command to be emitted before and after
    an InputSectionDesc.
    """

    def __init__(self, symbol, tied=False, mutable=False):
        self.symbol = symbol
        self.tied = tied
        self.mutable = mutable

    def __str__(self):
        return f'{self.symbol} = ABSOLUTE(.);'

    def __eq__(self, other):
        return isinstance(other, SymbolAtAddress) and self.symbol == other.symbol


class InputSectionDesc:
    """
    Outputs an input section description as described in
    https://www.acrc.bris.ac.uk/acrc/RedHat/rhel-ld-en-4/sections.html#INPUT-SECTION.

    These commands are emitted from mapping fragment entries, specifically attaching
    a scheme onto an entity. Mapping fragment flags KEEP, SORT will also affect
    the emitted input section description.
    """

    def __init__(self, entity, sections, exclusions=None, keep=False, sort=None, tied=False, mutable=False):
        assert entity.specificity != Entity.Specificity.SYMBOL

        self.entity = entity
        self.sections = set(sections)

        self.exclusions = set()

        if exclusions:
            assert not [
                e
                for e in exclusions
                if e.specificity == Entity.Specificity.SYMBOL or e.specificity == Entity.Specificity.NONE
            ]
            self.exclusions = set(exclusions)
        else:
            self.exclusions = set()

        self.keep = keep
        self.sort = sort
        self.tied = tied
        self.mutable = mutable

    def __str__(self):
        sections_string = ''

        if self.sections:
            sections_string = '( )'
            exclusion_strings = []

            for exc in sorted(self.exclusions):
                if exc.specificity == Entity.Specificity.ARCHIVE:
                    exc_string = f'*{exc.archive}'
                else:
                    exc_string = f'*{exc.archive}:{exc.obj}.*'

                exclusion_strings.append(exc_string)

            section_strings = []

            if exclusion_strings:
                exclusion_string = 'EXCLUDE_FILE({})'.format(' '.join(exclusion_strings))

                for section in sorted(self.sections):
                    section_strings.append(f'{exclusion_string} {section}')
            else:
                for section in sorted(self.sections):
                    section_strings.append(section)

            if self.sort:
                if self.sort == (None, None):
                    pattern = 'SORT({})'
                elif self.sort == ('name', None):
                    pattern = 'SORT_BY_NAME({})'
                elif self.sort == ('alignment', None):
                    pattern = 'SORT_BY_ALIGNMENT({})'
                elif self.sort == ('init_priority', None):
                    pattern = 'SORT_BY_INIT_PRIORITY({})'
                elif self.sort == ('name', 'alignment'):
                    pattern = 'SORT_BY_NAME(SORT_BY_ALIGNMENT({}))'
                elif self.sort == ('alignment', 'name'):
                    pattern = 'SORT_BY_ALIGNMENT(SORT_BY_NAME({}))'
                elif self.sort == ('name', 'name'):
                    pattern = 'SORT_BY_NAME(SORT_BY_NAME({}))'
                elif self.sort == ('alignment', 'alignment'):
                    pattern = 'SORT_BY_ALIGNMENT(SORT_BY_ALIGNMENT({}))'
                else:
                    raise Exception('Invalid sort arguments')

                section_strings = [pattern.format(s) for s in section_strings]

            sections_string = '({})'.format(' '.join(section_strings))

        if self.entity.specificity == Entity.Specificity.NONE:
            entry = f'*{sections_string}'
        elif self.entity.specificity == Entity.Specificity.ARCHIVE:
            entry = f'*{self.entity.archive}:{sections_string}'
        else:
            entry = f'*{self.entity.archive}:{self.entity.obj}.*{sections_string}'

        if self.keep:
            res = f'KEEP({entry})'
        else:
            res = entry

        return res

    def __eq__(self, other):
        return (
            isinstance(other, InputSectionDesc)
            and self.entity == other.entity
            and self.sections == other.sections
            and self.exclusions == other.exclusions
            and self.keep == other.keep
            and self.sort == other.sort
        )

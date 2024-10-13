#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import collections
import fnmatch
import os
import sys
import tempfile
import unittest
from io import StringIO

try:
    from ldgen.entity import Entity, EntityDB
    from ldgen.fragments import parse_fragment_file
    from ldgen.generation import Generation, GenerationException
    from ldgen.linker_script import LinkerScript
    from ldgen.output_commands import AlignAtAddress, InputSectionDesc, SymbolAtAddress
    from ldgen.sdkconfig import SDKConfig
except ImportError:
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))
    from ldgen.entity import Entity, EntityDB
    from ldgen.fragments import parse_fragment_file
    from ldgen.generation import Generation, GenerationException
    from ldgen.linker_script import LinkerScript
    from ldgen.output_commands import AlignAtAddress, InputSectionDesc, SymbolAtAddress
    from ldgen.sdkconfig import SDKConfig

ROOT = Entity('*')

FREERTOS = Entity('libfreertos.a')
CROUTINE = Entity('libfreertos.a', 'croutine')
TIMERS = Entity('libfreertos.a', 'timers')

FREERTOS2 = Entity('libfreertos2.a')


class GenerationTest(unittest.TestCase):

    def setUp(self):
        self.generation = Generation()
        self.entities = None
        self.linker_script = None

        with tempfile.NamedTemporaryFile(delete=False) as f:
            self.kconfigs_source_file = os.path.join(tempfile.gettempdir(), f.name)
            self.addCleanup(os.remove, self.kconfigs_source_file)
        with tempfile.NamedTemporaryFile(delete=False) as f:
            self.kconfig_projbuilds_source_file = os.path.join(tempfile.gettempdir(), f.name)
            self.addCleanup(os.remove, self.kconfig_projbuilds_source_file)

        os.environ['COMPONENT_KCONFIGS_SOURCE_FILE'] = self.kconfigs_source_file
        os.environ['COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE'] = self.kconfig_projbuilds_source_file
        os.environ['COMPONENT_KCONFIGS'] = ''
        os.environ['COMPONENT_KCONFIGS_PROJBUILD'] = ''

        # prepare_kconfig_files.py doesn't have to be called because COMPONENT_KCONFIGS and
        # COMPONENT_KCONFIGS_PROJBUILD are empty

        self.sdkconfig = SDKConfig('data/Kconfig', 'data/sdkconfig')

        fragment_file = parse_fragment_file('data/base.lf', self.sdkconfig)
        self.generation.add_fragments_from_file(fragment_file)

        self.entities = EntityDB()

        with open('data/libfreertos.a.txt') as objdump:
            self.entities.add_sections_info(objdump)

        with open('data/linker_script.ld') as linker_script:
            self.linker_script_expect = LinkerScript(linker_script)

        with open('data/linker_script.ld') as linker_script:
            self.linker_script_actual = LinkerScript(linker_script)

    @staticmethod
    def create_fragment_file(contents, name='test_fragment.lf'):
        f = StringIO(contents)
        f.name = name
        return f

    def add_fragments(self, text):
        fragment_file = self.create_fragment_file(text)
        fragment_file = parse_fragment_file(fragment_file, self.sdkconfig)
        self.generation.add_fragments_from_file(fragment_file)

    def write(self, expected, actual):
        self.linker_script_expect.fill(expected)
        self.linker_script_expect.write(open('expected.ld', 'w'))

        self.linker_script_actual.fill(actual)
        self.linker_script_actual.write(open('actual.ld', 'w'))

    def generate_default_rules(self):
        rules = collections.defaultdict(list)

        rules['dram0_bss'].append(InputSectionDesc(ROOT, ['.bss', '.bss.*'], []))
        rules['dram0_bss'].append(InputSectionDesc(ROOT, ['COMMON'], []))
        rules['dram0_data'].append(InputSectionDesc(ROOT, ['.data', '.data.*'], []))
        rules['dram0_data'].append(InputSectionDesc(ROOT, ['.dram', '.dram.*'], []))
        rules['flash_text'].append(InputSectionDesc(ROOT, ['.literal', '.literal.*', '.text', '.text.*'], []))
        rules['flash_rodata'].append(InputSectionDesc(ROOT, ['.rodata', '.rodata.*'], []))
        rules['iram0_text'].append(InputSectionDesc(ROOT, ['.iram', '.iram.*'], []))
        rules['rtc_bss'].append(InputSectionDesc(ROOT, ['.rtc.bss'], []))
        rules['rtc_data'].append(InputSectionDesc(ROOT, ['.rtc.data'], []))
        rules['rtc_data'].append(InputSectionDesc(ROOT, ['.rtc.rodata'], []))
        rules['rtc_text'].append(InputSectionDesc(ROOT, ['.rtc.text', '.rtc.literal'], []))

        return rules

    def compare_rules(self, expected, actual):
        self.assertEqual(expected, actual)

    def get_default(self, target, rules):
        return rules[target][0]


class DefaultMappingTest(GenerationTest):

    def test_rule_generation_default(self):
        # Checks that default rules are generated from
        # the default scheme properly and even if no mappings
        # are defined.
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        self.compare_rules(expected, actual)

    def test_default_mapping_lib(self):
        # Mapping a library with default mapping. This should not emit additional rules,
        # other than the default ones.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (default)
"""
        self.add_fragments(mapping)
        self.test_rule_generation_default()

    def test_default_mapping_obj(self):
        # Mapping an object with default mapping. This should not emit additional rules,
        # other than the default ones.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (default)
"""
        self.add_fragments(mapping)
        self.test_rule_generation_default()

    def test_default_mapping_symbol(self):
        # Mapping a symbol with default mapping. This should not emit additional rules,
        # other than the default ones.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckPendingReadyList (default)                 #1
"""
        self.add_fragments(mapping)
        self.test_rule_generation_default()

    def test_default_mapping_all(self):
        # Mapping a library, object, and symbol with default mapping. This should not emit additional rules,
        # other than the default ones.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (default)                                     #1
    croutine (default)                              #2
    croutine:prvCheckPendingReadyList (default)     #3
"""
        self.add_fragments(mapping)
        self.test_rule_generation_default()

    def test_default_mapping_lib_symbol(self):
        # Mapping a library, and symbol with default mapping. This should not emit additional rules,
        # other than the default ones.
        #
        # This is a check needed to make sure generation does not generate
        # intermediate commands due to presence of symbol mapping.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (default)                                     #1
    croutine:prvCheckPendingReadyList (default)     #2
"""
        self.add_fragments(mapping)
        self.test_rule_generation_default()

    def test_default_mapping_obj_symbol(self):
        # Mapping a library, and symbol with default mapping. This should not emit additional rules,
        # other than the default ones.
        #
        # This is a check needed to make sure generation does not generate
        # intermediate commands due to presence of symbol mapping.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (default)                              #1
    croutine:prvCheckPendingReadyList (default)     #2
"""
        self.add_fragments(mapping)
        self.test_rule_generation_default()


class BasicTest(GenerationTest):
    # Test basic and fundamental interactions between typical
    # entries.

    def test_nondefault_mapping_lib(self, alt=None):
        # Test mapping entry different from default for a library.
        # There should be exclusions in the default commands for flash_text and flash_rodata:
        #
        # flash_text
        #   *((EXCLUDE_FILE(libfreertos.a)) .literal ...)                                                          A
        #
        # Commands placing the entire library in iram, dram should be generated:
        #
        # iram0_text
        #   *(.iram ...)
        #   *libfreertos.a(.literal  ...)                                                                          B
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (noflash)                     #1
"""
        self.add_fragments(alt if alt else mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        iram0_text = expected['iram0_text']
        dram0_data = expected['dram0_data']

        # Generate exclusions in flash_text and flash_rodata                                                 A
        flash_text[0].exclusions.add(FREERTOS)
        flash_rodata[0].exclusions.add(FREERTOS)

        # Input section commands in iram_text and dram0_data for #1                                          B
        iram0_text.append(InputSectionDesc(FREERTOS, flash_text[0].sections, []))
        dram0_data.append(InputSectionDesc(FREERTOS, flash_rodata[0].sections, []))

        self.compare_rules(expected, actual)

    def test_nondefault_mapping_obj(self, alt=None):
        # Test mapping entry different from default for an object.
        # There should be exclusions in the default commands for flash_text and flash_rodata:
        #
        # flash_text
        #   *((EXCLUDE_FILE(libfreertos.a:croutine)) .literal ...)                                                          A
        #
        # Commands placing the entire library in iram, dram should be generated:
        #
        # iram0_text
        #   *(.iram ...)
        #   *libfreertos.a:croutine(.literal  ...)                                                                          B
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash)                              #1
"""

        self.add_fragments(alt if alt else mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        iram0_text = expected['iram0_text']
        dram0_data = expected['dram0_data']

        # Generate exclusions in flash_text and flash_rodata                                                A
        flash_text[0].exclusions.add(CROUTINE)
        flash_rodata[0].exclusions.add(CROUTINE)

        # Input section commands in iram_text and dram0_data for #1                                         B
        iram0_text.append(InputSectionDesc(CROUTINE, flash_text[0].sections, []))
        dram0_data.append(InputSectionDesc(CROUTINE, flash_rodata[0].sections, []))

        self.compare_rules(expected, actual)

    def test_nondefault_mapping_symbol(self):
        # Test mapping entry different from default for symbol.
        # There should be exclusions in the default commands for flash_text, as well as the implicit intermediate object command
        # with an exclusion from default:
        #
        # flash_text
        #   *((EXCLUDE_FILE(libfreertos.a:croutine)) .literal ...)                                                          A
        #   *libfreertos.a:croutine(.literal  .literal.prvCheckDelayedList ...)                                             B
        #
        # Commands placing the entire library in iram should be generated:
        #
        # iram0_text
        #   *(.iram ...)
        #   *libfreertos.a:croutine(.text.prvCheckPendingReadyList .literal.prvCheckPendingReadyList)                       C
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckPendingReadyList (noflash)         #1
"""
        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Generate exclusion in flash_text                                                A
        flash_text[0].exclusions.add(CROUTINE)

        # Generate intermediate command                                                   B
        # List all relevant sections except the symbol
        # being mapped
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')

        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))

        # Input section commands in iram_text for #1                                     C
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_default_symbol_nondefault_lib(self):
        # Test default symbol mapping with different lib mapping. This should create an implicit intermediate object command.
        # The significant targets are flash_text, flash_rodata, iram0_text, dram0_data.
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a) .text ...)                                                        A
        #  libfreertos.a:croutine (.text.prvCheckPendingReadyList .literal.prvCheckPendingReadyList)        B
        #
        # flash_rodata
        #   *(EXCLUDE_FILE(libfreertos.a) .rodata ...)                                                      A
        #
        # iram0_text
        #  * ( .iram ...)
        #  libfreertos.a (EXCLUDE_FILE(libfreertos:croutine) .text ...)                                     C.1
        #   *libfreertos.a:croutine(.literal  .literal.prvCheckDelayedList ...)                             D
        #
        # dram0_data
        #  * ( .dram ...)
        #  libfreertos.a ( .rodata ...)                                                                     C.2
        #
        # Only default commands are in the other targets.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (noflash)                                         #1
    croutine:prvCheckPendingReadyList (default)         #2
"""

        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        iram0_text = expected['iram0_text']
        dram0_data = expected['dram0_data']

        # Exclusions for #1                                                                                 A
        flash_text[0].exclusions.add(FREERTOS)
        flash_rodata[0].exclusions.add(FREERTOS)

        # Commands for #1                                                                                   C.1 & C.2
        # C.1 excludes intermediate command for #2
        iram0_text.append(InputSectionDesc(FREERTOS, flash_text[0].sections, [CROUTINE]))
        dram0_data.append(InputSectionDesc(FREERTOS, flash_rodata[0].sections, []))

        # Intermediate command for excluding #2                                                             D
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')

        iram0_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))

        # Command for #2                                                                                    B
        flash_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_default_symbol_nondefault_obj(self):
        # Test default symbol mapping with different obj mapping. Since there is an explicit entry for the object,
        # the sections for that object should just be expanded and the symbol section subtracted, to be placed
        # using another command.
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a:croutine) .text ...)                                                        A
        #   libfreertos.a:croutine (.text.prvCheckPendingReadyList .literal.prvCheckPendingReadyList)                B
        #
        # flash_rodata
        #   *(EXCLUDE_FILE(libfreertos.a:croutine) .rodata ...)                                                      A
        #
        # iram0_text
        #   *( .iram ...)
        #   *libfreertos.a:croutine(.literal  .literal.prvCheckDelayedList ...)                                      C.1
        #
        # dram0_data
        #   *(.data ..)
        #   *libfreertos.a:croutine(.rodata ....)                                                                    C.2
        #
        # Only default commands are in the other targets
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash)                                  #1
    croutine:prvCheckPendingReadyList (default)         #2
"""
        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        iram0_text = expected['iram0_text']
        dram0_data = expected['dram0_data']

        # Exclusions for #1                                                                                 A
        flash_text[0].exclusions.add(CROUTINE)
        flash_rodata[0].exclusions.add(CROUTINE)

        # Commands for #1                                                                                   C.1 & C.2
        # C.1 list relevant sections for libfreertos.a:croutine to
        # exclude symbol to map
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')

        iram0_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))
        dram0_data.append(InputSectionDesc(CROUTINE, flash_rodata[0].sections, []))

        # Command for #2                                                                                    B
        flash_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_default_nondefault_alternating(self):
        # Here, each of the entries map sections to something different
        # than its one-level-up entry.
        #
        # *                                         text -> flash, rodata -> flash
        # libfreertos.a                             text -> iram, rodata -> dram
        # libfreertos.a:croutine                    text -> flash, rodata -> flash
        # croutine:prvCheckPendingReadyList         text  -> iram
        #
        # The significant targets are flash_text, flash_rodata, iram0_text, and dram0_data.
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a) .text ...)                                                         A
        #   *libfreertos.a:croutine(.literal  .literal.prvCheckDelayedList ...)                              B.1
        #
        # flash_rodata
        #   *(EXCLUDE_FILE(libfreertos.a) .rodata ...)                                                       A
        #   *libfreertos.a:croutine(.rodata .rodata.*)                                                       B.2
        #
        # iram0_text
        #  * ( .iram ...)
        #  libfreertos.a (EXCLUDE_FILE(libfreertos:croutine) .text ...)                                      C
        #  libfreertos.a:croutine (.text.prvCheckPendingReadyList .literal.prvCheckPendingReadyList)         D
        #
        # dram0_data
        #  * ( .dram ...)
        #  libfreertos.a (EXCLUDE_FILE(libfreertos:croutine) .rodata ...)                                    C
        #
        # For the other targets only the default commands should be present.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (noflash)                                         #1
    croutine (default)                                  #2
    croutine:prvCheckPendingReadyList (noflash)         #3
"""

        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        iram0_text = expected['iram0_text']
        dram0_data = expected['dram0_data']

        # Exclusions for #1                                                                                 A
        # Only for flash_text and flash_rodata
        flash_text[0].exclusions.add(FREERTOS)
        flash_rodata[0].exclusions.add(FREERTOS)

        # Commands for #1                                                                                   C
        # with exclusions for #2
        iram0_text.append(InputSectionDesc(FREERTOS, flash_text[0].sections, [CROUTINE]))
        dram0_data.append(InputSectionDesc(FREERTOS, flash_rodata[0].sections, [CROUTINE]))

        # Commands for #2                                                                                   B.1
        flash_rodata.append(InputSectionDesc(CROUTINE, flash_rodata[0].sections, []))

        # List all relevant sections in case of flash_text                                                  B.2
        # as exclusion for #3
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')

        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))

        # Command for #3                                                                                    D
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_nondefault_but_same_lib_and_obj(self):
        # Extension of DefaultMappingTest. Commands should not be generated for #2, since it does similar mapping
        # to #1. Output is similar to test_different_mapping_lib.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (noflash)                                     #1
    croutine (noflash)                              #2
"""
        self.test_nondefault_mapping_lib(mapping)

    def test_nondefault_but_same_lib_and_sym(self):
        # Extension of DefaultMappingTest. Commands should not be generated for #2, since it does similar mapping
        # to #1. Output is similar to test_different_mapping_lib.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (noflash)                                     #1
    croutine:prvCheckPendingReadyList (noflash)     #2
"""
        self.test_nondefault_mapping_lib(mapping)

    def test_nondefault_but_same_obj_and_sym(self):
        # Commands should not be generated for #2, since it does similar mapping
        # to #1. Output is similar to test_different_mapping_obj.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash)                                     #1
    croutine:prvCheckPendingReadyList (noflash)            #2
"""
        self.test_nondefault_mapping_obj(mapping)

    def test_multiple_symbols_excluded_from_intermediate_command(self):
        # Test mapping multiple symbols from the same object.
        # All these symbols must be succesfully excluded from
        # the intermediate command.
        #
        # flash_text
        #   * (EXCLUDE_FILE(libfreertos.a:croutine) .text ...)                  A
        #   libfreertos:croutine(.text ...)                                     B
        #
        # iram0_text
        #
        #
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckPendingReadyList (noflash)            #1
    croutine:prvCheckDelayedList (noflash)                 #2
"""

        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Exclusions for #1 & #2 intermediate command                                       A
        flash_text[0].exclusions.add(CROUTINE)

        # Intermediate command for #1 & #2 which lists                                      B
        # all relevant sections in croutine except prvCheckPendingReadyList
        # and prvCheckDelayedList
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckDelayedList')]
        filtered_sections.append('.text')

        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))

        # Commands for #1 & 2
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckDelayedList', '.literal.prvCheckDelayedList']), []))
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_root_mapping_fragment(self):
        # Test creation of a mapping fragment that maps '*'.
        # This should generate another default command in iram0_text:
        #
        # iram0_text
        #   * (.custom_section)                                  A
        #   * (.iram .iram.*)
        mapping = u"""
[sections:custom_section]
entries:
    .custom_section

[scheme:custom_scheme]
entries:
    custom_section -> iram0_text

[mapping:default2]
archive: *
entries:
    * (custom_scheme)                                           #1
"""

        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        # Generate default command                              A
        # Since these are the same 'specificity', the commands
        # are arranged alphabetically.
        expected['iram0_text'].append(expected['iram0_text'][0])
        expected['iram0_text'][0] = InputSectionDesc(ROOT, ['.custom_section'], [])

        self.compare_rules(expected, actual)


class AdvancedTest(GenerationTest):

    # Test valid but quirky cases, corner cases, failure cases, and
    # cases involving interaction between schemes, other mapping
    # fragments.

    def test_same_entity_no_scheme_common(self):
        # Test same entity being mapped by schemes that have nothing in common.
        #
        # noflash_data: rodata -> dram0_data
        # noflash_text: text -> iram0_text
        #
        # This operation should succeed with the following commands:
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a:croutine) .text ...)                                                        A
        #
        # flash_rodata
        #   *(EXCLUDE_FILE(libfreertos.a:croutine) .rodata ...)                                                      B
        #
        # iram0_text
        #   *(.iram ...)
        #   *libfreertos.a:croutine(.text .text.* ...)                                                               C
        #
        # dram0_data
        #   *(.data ..)
        #   *(.dram ...)
        #   *libfreertos.a:croutine(.rodata .rodata.*)                                                               D
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash_text)                         #1
    croutine (noflash_data)                         #2
"""
        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        iram0_text = expected['iram0_text']
        dram0_data = expected['dram0_data']

        # Exclusions for #1                                                                 A
        flash_text[0].exclusions.add(CROUTINE)

        # Exclusions for #2                                                                 B
        flash_rodata[0].exclusions.add(CROUTINE)

        # Command for #1                                                                    C
        iram0_text.append(InputSectionDesc(CROUTINE, flash_text[0].sections, []))

        # Command for #2                                                                    D
        dram0_data.append(InputSectionDesc(CROUTINE, flash_rodata[0].sections, []))

        self.compare_rules(expected, actual)

    def test_same_entity_sub_scheme(self):
        # Test same entity being mapped by scheme that is a subset of the other.
        #
        # noflash: text -> iram0_text, rodata -> dram0_data
        # noflash_text: text -> iram0_text
        #
        # `text -> iram0_text` is common between the two schemes.
        #
        # This operation should succeed with the following commands:
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a:croutine) .text ...)                                                        A
        #
        # flash_rodata
        #   *(EXCLUDE_FILE(libfreertos.a:croutine) .rodata ...)                                                      B
        #
        # iram0_text
        #   *(.iram ...)
        #   *libfreertos.a:croutine(.text .text.* ...)                                                               C
        #
        # dram0_data
        #   *(.data ..)
        #   *(.dram ...)
        #   *libfreertos.a:croutine(.rodata .rodata.*)                                                               D
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash)                              #1
    croutine (noflash_data)                         #2
"""
        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        iram0_text = expected['iram0_text']
        dram0_data = expected['dram0_data']

        # Exclusions for #1                                                                 A
        flash_text[0].exclusions.add(CROUTINE)

        # Exclusions for #1 & #2                                                            B
        flash_rodata[0].exclusions.add(CROUTINE)

        # Command for #1                                                                    C
        iram0_text.append(InputSectionDesc(CROUTINE, flash_text[0].sections, []))

        # Command for #1 & #2                                                               D
        dram0_data.append(InputSectionDesc(CROUTINE, flash_rodata[0].sections, []))

        self.compare_rules(expected, actual)

    def test_same_entity_conflicting_scheme(self, alt=None):
        # Test same entity being mapped by scheme conflicting with another.
        #
        # rtc = text -> rtc_text, rodata -> rtc_data
        # noflash = text -> iram0_text, rodata -> dram0_data
        #
        # This operation should fail.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash)                              #1
    croutine (rtc)                                  #2
"""
        self.add_fragments(alt if alt else mapping)

        with self.assertRaises(GenerationException):
            self.generation.generate(self.entities, False)

    def test_same_entity_conflicting_section(self):
        # Test same entity being mapped by scheme conflicting with another.
        #
        # custom_rtc = .text -> rtc_text
        # noflash = .text -> iram0_text, .rodata -> dram0_data
        #
        # This operation should fail.
        mapping = u"""
[sections:custom_text]
entries:
    .text+
    .literal+

[scheme:custom_rtc]
entries:
    custom_text -> rtc_text

[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash)                              #1
    croutine (custom_rtc)                           #2
"""
        self.test_same_entity_conflicting_scheme(mapping)

    def test_complex_mapping_case(self, alt=None):
        # Test a complex case where an object is mapped using
        # one scheme, but a specific symbol in that object is mapped
        # using another. Another object and symbol is mapped the other way around.
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a:croutine libfreertos.a:timers) .text ...)                                                        A, B
        #
        # flash_rodata
        #   *(EXCLUDE_FILE(libfreertos.a:croutine libfreertos.a:timers) .rodata ...)                                                      A, B
        #
        # dram0_data
        #   *(EXCLUDE_FILES(libfreertos.a:timers) .data ..)                                                                               B
        #   *(.dram ...)
        #   *libfreertos.a:croutine(.rodata .rodata.*)                                                                                    C
        #   *libfreertos.a:timers(.rodata.prvProcessReceivedCommands ...)                                                                 E
        #
        # dram0_bss
        #   *(EXCLUDE_FILE(libfreertos.a:timers) .bss .bss.* ...)                                                                         B
        #   *(EXCLUDE_FILE(libfreertos.a:timers) COMMON)                                                                                  B
        #
        # iram0_text
        #   *(.iram ...)
        #   *libfreertos.a:croutine(.literal  .literal.prvCheckDelayedList ...)                                                           C
        #   *libfreertos.a:timers(.literal  .literal.prvProcessReceivedCommands ...)                                                      E
        #
        # rtc_text
        #   *(rtc.text .rtc.literal)
        #   libfreertos.a:croutine (.text.prvCheckPendingReadyList .literal.prvCheckPendingReadyList)                                     F
        #   libfreertos.a:timers (.text .text.prvCheckForValidListAndQueue ...)                                                           D.2
        #
        # rtc_data
        #   *(rtc.data)
        #   *(rtc.rodata)
        #   libfreertos.a:timers (.data .data.*)                                                                                          D
        #   libfreertos.a:timers (.rodata ...)                                                                                            D.2
        #
        # rtc_bss
        #   *(rtc.bss .rtc.bss)
        #   libfreertos.a:timers (.bss .bss.*)                                                                                            D
        #   libfreertos.a:timers (COMMON)                                                                                                 D
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash)                                  #1
    timers (rtc)                                        #2
    timers:prvProcessReceivedCommands (noflash)         #3
    croutine:prvCheckPendingReadyList (rtc)             #4
"""

        self.add_fragments(alt if alt else mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        dram0_data = expected['dram0_data']
        iram0_text = expected['iram0_text']
        dram0_bss = expected['dram0_bss']
        rtc_text = expected['rtc_text']
        rtc_data = expected['rtc_data']
        rtc_bss = expected['rtc_bss']

        # Exclusions for #1                                                                 A
        flash_text[0].exclusions.add(CROUTINE)
        flash_rodata[0].exclusions.add(CROUTINE)

        # Exclusions for #2                                                                 B
        flash_text[0].exclusions.add(TIMERS)
        flash_rodata[0].exclusions.add(TIMERS)
        dram0_data[0].exclusions.add(TIMERS)
        dram0_bss[0].exclusions.add(TIMERS)
        dram0_bss[1].exclusions.add(TIMERS)

        # Commands for #1                                                                   C
        # List all relevant sections excluding #4 for text -> iram0_text
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')

        iram0_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))
        dram0_data.append(InputSectionDesc(CROUTINE, flash_rodata[0].sections, []))

        # Commands for #4                                                                   F
        # Processed first due to alphabetical ordering
        rtc_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        # Commands for #2                                                                   D
        # List all relevant sections excluding #3 for text -> rtc_text and                  D.2
        # rodata -> rtc_data
        timers_sections = self.entities.get_sections('libfreertos.a', 'timers')
        filtered_sections = fnmatch.filter(timers_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(timers_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvProcessReceivedCommands')]
        filtered_sections.append('.text')
        rtc_text.append(InputSectionDesc(TIMERS, set(filtered_sections), []))

        rtc_data.append(InputSectionDesc(TIMERS, dram0_data[0].sections, []))
        filtered_sections = fnmatch.filter(timers_sections, '.rodata.*')
        filtered_sections = [s for s in filtered_sections if not s.endswith('prvProcessReceivedCommands')]
        rtc_data.append(InputSectionDesc(TIMERS, set(filtered_sections), []))

        rtc_bss.append(InputSectionDesc(TIMERS, dram0_bss[0].sections, []))
        rtc_bss.append(InputSectionDesc(TIMERS, dram0_bss[1].sections, []))

        # Commands for #3                                                                  E
        iram0_text.append(InputSectionDesc(TIMERS, set(['.text.prvProcessReceivedCommands', '.literal.prvProcessReceivedCommands']), []))
        dram0_data.append(InputSectionDesc(TIMERS, set(['.rodata.prvProcessReceivedCommands']), []))

        self.compare_rules(expected, actual)

    def test_multiple_mapping_fragments(self):
        # Test mapping multiple fragments succeeds, particularly
        # generating exclusions from the default command of archive
        # and object specificity.
        #
        # flash_text
        #   * (EXCLUDE_FILE(libfreertos.a libfreertos.a:croutine) .text ...)
        #
        # flash_rodata
        #   * (EXCLUDE_FILE(libfreertos.a libfreertos.a:croutine) .text ...)
        #
        # iram0_text
        mapping = u"""
[mapping:test_1]
archive: libfreertos.a
entries:
    croutine (noflash)                              #1

[mapping:test_2]
archive: libfreertos2.a
entries:
    * (noflash)                                     #2
"""

        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_rodata = expected['flash_rodata']
        iram0_text = expected['iram0_text']
        dram0_data = expected['dram0_data']

        # Exclusions for #1                                                                 A
        flash_text[0].exclusions.add(CROUTINE)
        flash_rodata[0].exclusions.add(CROUTINE)

        # Exclusions for #1 & #2                                                            B
        flash_text[0].exclusions.add(FREERTOS2)
        flash_rodata[0].exclusions.add(FREERTOS2)

        # Command for #1                                                                    C
        iram0_text.append(InputSectionDesc(CROUTINE, flash_text[0].sections, []))
        dram0_data.append(InputSectionDesc(CROUTINE, flash_rodata[0].sections, []))

        # Command for #1 & #2                                                               D
        iram0_text.append(InputSectionDesc(FREERTOS2, flash_text[0].sections, []))
        dram0_data.append(InputSectionDesc(FREERTOS2, flash_rodata[0].sections, []))

        self.compare_rules(expected, actual)

    def test_mapping_same_lib_in_multiple_fragments_no_conflict(self):
        # Test mapping fragments operating on the same archive.
        # In these cases, the entries are taken together.
        #
        # Uses the same entries as C_05 but spreads them across
        # two fragments. The output should still be the same.
        mapping = u"""
[mapping:test_1]
archive: libfreertos.a
entries:
    croutine (noflash)                                  #1
    timers:prvProcessReceivedCommands (noflash)         #3

[mapping:test_2]
archive: libfreertos.a
entries:
    timers (rtc)                                        #2
    croutine:prvCheckPendingReadyList (rtc)             #4
"""
        self.test_complex_mapping_case(mapping)

    def test_mapping_same_lib_in_multiple_fragments_conflict(self):
        # Test mapping fragments operating on the same archive
        # with conflicting mappings.
        mapping = u"""
[mapping:test_1]
archive: libfreertos.a
entries:
    croutine (noflash)                              #1

[mapping:test_2]
archive: libfreertos.a
entries:
    croutine (rtc)                                  #2
"""
        self.test_same_entity_conflicting_scheme(mapping)

    def test_command_order(self):
        # Test command order sorting: the commands should be sorted by specificity, then
        # alphabetically. This contributes to deterministic output given
        # the same input mapping entries.
        #
        # This ordering is also tested in other tests as a side-effect.
        #
        # flash_text
        #   * (EXCLUDE_FILE(libfreertos.a:croutine libfreertos.a:croutine2))                                    A
        #   libfreertos.a:croutine(.text ....)                                                                  B
        #
        # iram0_text
        #
        #   * (.iram .iram.*)
        #   libfreertos:croutine(.text .literal ...)                                                            C
        #   libfreertos:croutine(.text.prvCheckDelayedList .literal.prvCheckDelayedList)                        F
        #   libfreertos:croutine(.text.prvCheckPendingReadyList .literal.prvCheckPendingReadyList)              G
        #   libfreertos2:croutine(.text .literal ...)                                                           D
        #   libfreertos2:croutine2(.text .literal ...)                                                          E
        mapping = u"""
[mapping:freertos2]
archive: libfreertos2.a
entries:
    croutine2 (noflash_text)                                    #1
    croutine (noflash_text)                                     #2

[mapping:freertos]
archive: libfreertos.a
entries:
    croutine:prvCheckPendingReadyList (noflash_text)            #3
    croutine:prvCheckDelayedList (noflash_text)                 #4
"""

        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Exclusions for #1                                                                 A
        flash_text[0].exclusions.add(CROUTINE)
        flash_text[0].exclusions.add(Entity(FREERTOS2.archive, 'croutine2'))
        flash_text[0].exclusions.add(Entity(FREERTOS2.archive, 'croutine'))

        # Intermediate command for #3 and #4                                                B
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')

        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckDelayedList')]
        filtered_sections.append('.text')
        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))

        # Command for
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckDelayedList', '.literal.prvCheckDelayedList']), []))
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        iram0_text.append(InputSectionDesc(Entity(FREERTOS2.archive, 'croutine'), flash_text[0].sections, []))
        iram0_text.append(InputSectionDesc(Entity(FREERTOS2.archive, 'croutine2'), flash_text[0].sections, []))

        self.compare_rules(expected, actual)

    def test_ambigious_obj(self):
        # Command generation for ambiguous entry should fail.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    port:xPortGetTickRateHz (noflash)                 #1
"""
        self.add_fragments(mapping)

        with self.assertRaises(GenerationException):
            self.generation.generate(self.entities, False)

    def test_root_mapping_fragment_conflict(self):
        # Test that root mapping fragments are also checked for
        # conflicts.
        #
        # 'custom_scheme' entries conflict the 'default' scheme
        # entries.
        mapping = u"""
[scheme:custom_scheme]
entries:
    flash_text -> iram0_text

[mapping:default2]
archive: *
entries:
    * (custom_scheme)
"""

        self.add_fragments(mapping)
        with self.assertRaises(GenerationException):
            self.generation.generate(self.entities, False)

    def test_root_mapping_fragment_duplicate(self):
        # Same root mappings have no effect.
        #
        # custom_scheme has the 'iram -> iram0_text' in common with
        # default scheme
        mapping = u"""
[sections:custom_section]
entries:
    .custom_section

[scheme:custom_scheme]
entries:
    iram -> iram0_text
    custom_section -> iram0_text

[mapping:default2]
archive: *
entries:
    * (custom_scheme)
"""

        self.add_fragments(mapping)
        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        # Generate default command                              A
        # Since these are the same 'specificity', the commands
        # are arranged alphabetically.
        expected['iram0_text'].append(expected['iram0_text'][0])
        expected['iram0_text'][0] = InputSectionDesc(ROOT, ['.custom_section'], [])

        self.compare_rules(expected, actual)


class ConfigTest(GenerationTest):
    # Test command generation with conditions

    def _test_conditional_on_scheme(self, perf, alt=None):
        # Test that proper commands are generated if using
        # schemes with conditional entries.
        scheme = u"""
[sections:cond_text_data]
entries:
    if PERFORMANCE_LEVEL >= 1:
        .text+
        .literal+
    else:
        .rodata+

[scheme:cond_noflash]
entries:
    if PERFORMANCE_LEVEL >= 1:
        cond_text_data -> iram0_text
    else:
        cond_text_data -> dram0_data
"""

        mapping = u"""
[mapping:test]
archive: lib.a
entries:
    * (cond_noflash)
"""
        self.sdkconfig.config.syms['PERFORMANCE_LEVEL'].set_value(str(perf))
        self.add_fragments(scheme)
        self.add_fragments(alt if alt else mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        if perf >= 1:
            flash_text = expected['flash_text']
            iram0_text = expected['iram0_text']
            flash_text[0].exclusions.add(Entity('lib.a'))
            iram0_text.append(InputSectionDesc(Entity('lib.a'), flash_text[0].sections, []))
        else:
            flash_rodata = expected['flash_rodata']
            dram0_data = expected['dram0_data']
            flash_rodata[0].exclusions.add(Entity('lib.a'))
            dram0_data.append(InputSectionDesc(Entity('lib.a'), flash_rodata[0].sections, []))

        self.compare_rules(expected, actual)

    def test_conditional_on_scheme_00(self):
        self._test_conditional_on_scheme(0)

    def test_conditional_on_scheme_01(self):
        self._test_conditional_on_scheme(1)

    def test_conditional_mapping(self, alt=None):
        # Test that proper commands are generated
        # in conditional mapping entries.
        mapping = u"""
[mapping:default]
archive: *
entries:
    * (default)

[mapping:test]
archive: lib.a
entries:
    if PERFORMANCE_LEVEL = 1:
        obj1 (noflash)
    elif PERFORMANCE_LEVEL = 2:
        obj1 (noflash)
        obj2 (noflash)
    elif PERFORMANCE_LEVEL = 3:
        obj1 (noflash)
        obj2 (noflash)
        obj3 (noflash)
"""

        for perf_level in range(0, 4):
            self.sdkconfig.config.syms['PERFORMANCE_LEVEL'].set_value(str(perf_level))

            self.generation.mappings = {}
            self.add_fragments(alt if alt else mapping)

            actual = self.generation.generate(self.entities, False)
            expected = self.generate_default_rules()

            if perf_level < 4 and perf_level > 0:
                for append_no in range(1, perf_level + 1):
                    flash_text = expected['flash_text']
                    flash_rodata = expected['flash_rodata']
                    iram0_text = expected['iram0_text']
                    dram0_data = expected['dram0_data']

                    obj_str = 'obj' + str(append_no)

                    flash_text[0].exclusions.add(Entity('lib.a', obj_str))
                    flash_rodata[0].exclusions.add(Entity('lib.a', obj_str))

                    iram0_text.append(InputSectionDesc(Entity('lib.a', obj_str), flash_text[0].sections, []))
                    dram0_data.append(InputSectionDesc(Entity('lib.a', obj_str), flash_rodata[0].sections, []))

            self.compare_rules(expected, actual)

    def test_multiple_fragment_same_lib_conditional(self):
        # Test conditional entries on new mapping fragment grammar.
        # across multiple fragments.
        mapping = u"""
[mapping:default]
archive: *
entries:
    * (default)

[mapping:base]
archive: lib.a
entries:
    if PERFORMANCE_LEVEL = 1:
        obj1 (noflash)
    elif PERFORMANCE_LEVEL = 2:
        obj1 (noflash)
    elif PERFORMANCE_LEVEL = 3:
        obj1 (noflash)

[mapping:extra]
archive: lib.a
entries:
    if PERFORMANCE_LEVEL = 1:
        obj1 (noflash) # ignore duplicate definition
    elif PERFORMANCE_LEVEL = 2:
        obj2 (noflash)
    elif PERFORMANCE_LEVEL = 3:
        obj2 (noflash)
        obj3 (noflash)
"""

        self.test_conditional_mapping(mapping)


class FlagTest(GenerationTest):

    # Test correct generation of mapping fragment entries
    # with flags.

    def test_flags_basics(self):
        # Test that input section commands additions are done (KEEP SORT).
        # Test that order dependent commands are properly generated (ALIGN, SURROUND)
        # Normally, if an entry has the same mapping as parent, commands.
        #   are not emitted for them. However, if there are flags, they should be -
        #   only for the scheme entries that have flags, though.
        # Flag entries split across multiple entries work.
        #
        # flash_text
        #   *((EXCLUDE_FILE(libfreertos:timers libfreertos:croutine).text ...)          A
        #   KEEP(* (SORT_BY_NAME(EXCLUDE_FILE(libfreertos:timers).text) ...)            B
        #
        # flash_rodata
        #   *((EXCLUDE_FILE(libfreertos:timers) .rodata ...)                            C
        #   _sym2_start                                                                 D.1
        #   . = ALIGN(4)                                                                E.1
        #   KEEP(* (EXCLUDE_FILE(libfreertos:timers) .rodata ...)                       F
        #   _sym2_end                                                                   D.2
        #   . = ALIGN(4)                                                                E.2
        #
        # iram0_text
        #   *(.iram .iram.*)
        #   . = ALIGN(4)                                                                G.1
        #   _sym1_start                                                                 H.1
        #   libfreertos.a:croutine(.text .literal ...)                                  I
        #   . = ALIGN(4)                                                                G.2
        #   _sym1_end                                                                   H.2
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash_text);
        text->iram0_text ALIGN(4, pre, post) SURROUND(sym1)                             #1
    timers (default);
        text->flash_text KEEP() SORT(name)                                                #2
    timers (default);
        rodata->flash_rodata SURROUND(sym2) ALIGN(4, pre, post)                         #3
"""

        self.add_fragments(mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']
        flash_rodata = expected['flash_rodata']

        # Exclusions in flash_text for timers and croutine                      A
        flash_text[0].exclusions.add(CROUTINE)
        flash_text[0].exclusions.add(TIMERS)

        # Command for #3                                                        B
        flash_text.append(InputSectionDesc(TIMERS, flash_text[0].sections, [], keep=True, sort=('name', None)))

        # Exclusions in flash_rodata for timers                                 C
        flash_rodata[0].exclusions.add(TIMERS)

        # Commands for #3                                                       D.1, E.1, F, D.2, E.2
        flash_rodata.append(SymbolAtAddress('_sym2_start'))
        flash_rodata.append(AlignAtAddress(4))
        flash_rodata.append(InputSectionDesc(TIMERS, flash_rodata[0].sections, []))
        flash_rodata.append(SymbolAtAddress('_sym2_end'))
        flash_rodata.append(AlignAtAddress(4))

        # Commands for #                                                        G.1, H.1, I, G.2, H.2
        iram0_text.append(AlignAtAddress(4))
        iram0_text.append(SymbolAtAddress('_sym1_start'))
        iram0_text.append(InputSectionDesc(CROUTINE, flash_text[0].sections, []))
        iram0_text.append(AlignAtAddress(4))
        iram0_text.append(SymbolAtAddress('_sym1_end'))

        self.compare_rules(expected, actual)

    def test_flags_intermediate_exclusion_command_root(self):
        # Test that intermediate exclusion commands from root-level commands
        # are included in the flags.
        #
        # flash_text
        #   _sym1_start                                                                 A.1
        #   KEEP(* (EXCLUDE_FILE(libfreertos:croutine).text ...)                        B
        #   KEEP(libfreertos.a:croutine(...)))                                          C
        #   _sym1_end                                                                   A.2
        #
        # iram0_text
        #   *(.iram .iram.*)
        #   libfreertos.a:croutine(.text.prvCheckPendingReadyList ...)                  D
        mapping = u"""
[mapping:default]
archive: *
entries:
    # 1
    * (default);
        text->flash_text SURROUND(sym1) KEEP()                            #2

[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckPendingReadyList (noflash_text)                    #3
"""

        self.generation.mappings = {}
        self.add_fragments(mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Command for #2, pre                                          A.1
        flash_text.insert(0, SymbolAtAddress('_sym1_start'))

        # Command for #1 with KEEP()                                     B
        # and exclusion for #3
        flash_text[1].keep = True
        flash_text[1].exclusions.add(CROUTINE)

        # Implicit exclusion command for #3                            C
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')
        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), [], keep=True))

        # Command for #2, post                                         A.2
        flash_text.append(SymbolAtAddress('_sym1_end'))

        # Command for #3                                               D
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_flags_intermediate_exclusion_command_lib(self):
        # Test that intermediate exclusion commands from lib-level commands
        # are included in the flags.
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a).text  ...)
        #   _sym1_start                                                                 A.1
        #   KEEP(libfreertos.a(EXCLUDE_FILE(libfreertos:croutine).text.* ...))          B
        #   KEEP(libfreertos.a:croutine(...)))                                          C
        #   _sym1_end                                                                   A.2
        #
        # iram0_text
        #   *(.iram .iram.*)
        #   libfreertos.a:croutine(.text.prvCheckPendingReadyList ...)                  D
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    # 1
    * (default);
        text->flash_text SURROUND(sym1) KEEP()                            #2
    croutine:prvCheckPendingReadyList (noflash_text)                    #3
"""

        self.add_fragments(mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Command for #2, pre                                          A.1
        flash_text.append(SymbolAtAddress('_sym1_start'))
        flash_text[0].exclusions.add(FREERTOS)

        # Command for #1 with KEEP()                                     B
        # and exclusion for #3
        flash_text.append(InputSectionDesc(FREERTOS, flash_text[0].sections, [CROUTINE], keep=True))

        # Implicit exclusion command for #3                            C
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')
        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), [], keep=True))

        # Command for #2, post                                         A.2
        flash_text.append(SymbolAtAddress('_sym1_end'))

        # Command for #3                                               C
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_flags_intermediate_exclusion_command_obj(self):
        # Test that intermediate exclusion commands from obj-level commands
        # are included in the flags.
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a).text  ...)
        #   _sym1_start                                                                 A.1
        #   KEEP(libfreertos.a:croutine(...)))                                          B
        #   _sym1_end                                                                   A.2
        #
        # iram0_text
        #   *(.iram .iram.*)
        #   libfreertos.a:croutine(.text.prvCheckPendingReadyList ...)                  C
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    # 1
    croutine (default);
        text->flash_text SURROUND(sym1) KEEP()                            #2
    croutine:prvCheckPendingReadyList (noflash_text)                    #3
"""

        self.add_fragments(mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Command for #2, pre                                          A.1
        flash_text.append(SymbolAtAddress('_sym1_start'))
        flash_text[0].exclusions.add(CROUTINE)

        # Implicit exclusion command for #3                            B
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')
        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), [], keep=True))

        # Command for #2, post                                         A.2
        flash_text.append(SymbolAtAddress('_sym1_end'))

        # Command for #3                                               C
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_flags_separate_exclusion_command_if_explicit_root(self):
        # Explicit commands are separated from the parent's flags.
        #
        # flash_text
        #   _sym1_start                                                                 A.1
        #   KEEP(* (EXCLUDE_FILE(libfreertos:croutine).text ...)                        B
        #   _sym1_end                                                                   A.2
        #   KEEP(libfreertos.a:croutine(...)))                                          C
        #
        # iram0_text
        #   *(.iram .iram.*)
        #   libfreertos.a:croutine(.text.prvCheckPendingReadyList ...)                  D
        mapping = u"""
[mapping:default]
archive: *
entries:
    # 1
    * (default);
        text->flash_text SURROUND(sym1) KEEP()                            #2

[mapping:test]
archive: libfreertos.a
entries:
    croutine (default)                                                  #3
    croutine:prvCheckPendingReadyList (noflash_text)                    #4
"""

        self.generation.mappings = {}
        self.add_fragments(mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Command for #2, pre                                          A.1
        flash_text.insert(0, SymbolAtAddress('_sym1_start'))

        # Command for #1 with KEEP()                                     B
        # and exclusion for #3
        flash_text[1].keep = True
        flash_text[1].exclusions.add(CROUTINE)

        # Command for #2, post                                         A.2
        flash_text.append(SymbolAtAddress('_sym1_end'))

        # Command for #3                                               C
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')
        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))

        # Command for #4                                               D
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_flags_separate_exclusion_command_if_explicit_lib(self):
        # Explicit commands are separated from the parent's flags.
        #
        # flash_text
        #   *(EXCLUDE_FILE(libfreertos.a).text  ...)
        #   _sym1_start                                                                 A.1
        #   KEEP(libfreertos.a(EXCLUDE_FILE(libfreertos:croutine).text.* ...))          B
        #   _sym1_end                                                                   A.2
        #   KEEP(libfreertos.a:croutine(...)))                                          C
        #
        # iram0_text
        #   *(.iram .iram.*)
        #   libfreertos.a:croutine(.text.prvCheckPendingReadyList ...)                  D
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    # 1
    * (default);
        text->flash_text SURROUND(sym1) KEEP()
    croutine (default)                                              #2
    croutine:prvCheckPendingReadyList (noflash_text)                #3
"""

        self.add_fragments(mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Command for #2, pre                                          A.1
        flash_text.append(SymbolAtAddress('_sym1_start'))
        flash_text[0].exclusions.add(FREERTOS)

        # Command for #1 with KEEP()                                     B
        # and exclusion for #3
        flash_text.append(InputSectionDesc(FREERTOS, flash_text[0].sections, [CROUTINE], keep=True))

        # Command for #2, post                                         A.2
        flash_text.append(SymbolAtAddress('_sym1_end'))

        # Implicit exclusion command for #3                            C
        croutine_sections = self.entities.get_sections('libfreertos.a', 'croutine')
        filtered_sections = fnmatch.filter(croutine_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(croutine_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('prvCheckPendingReadyList')]
        filtered_sections.append('.text')
        flash_text.append(InputSectionDesc(CROUTINE, set(filtered_sections), []))

        # Command for #3                                               C
        iram0_text.append(InputSectionDesc(CROUTINE, set(['.text.prvCheckPendingReadyList', '.literal.prvCheckPendingReadyList']), []))

        self.compare_rules(expected, actual)

    def test_flag_additions(self):
        # Test ability to add flags as long as no other mapping fragments
        # does the same thing.
        mapping = u"""
[mapping:default_add_flag]
archive: *
entries:
    * (default);
        text->flash_text KEEP()
"""

        self.add_fragments(mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_text[0].keep = True

        self.compare_rules(expected, actual)

    def test_flags_flag_additions_duplicate(self):
        # Test same flags added to same entity - these
        # are ignored.
        mapping = u"""
[mapping:default_add_flag_1]
archive: *
entries:
    * (default);
        text->flash_text KEEP()

[mapping:default_add_flag_2]
archive: *
entries:
    * (default);
        text->flash_text KEEP()
"""

        self.add_fragments(mapping)

        actual = self.generation.generate(self.entities, False)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        flash_text[0].keep = True

        self.compare_rules(expected, actual)

    def test_flags_flag_additions_conflict(self):
        # Test condition where multiple fragments specifies flags
        # to same entity - should generate exception.
        mapping = u"""
[mapping:default_add_flag_1]
archive: *
entries:
    * (default);
        text->flash_text ALIGN(2)

[mapping:default_add_flag_2]
archive: *
entries:
    * (default);
        text->flash_text SURROUND(sym1)
"""
        self.add_fragments(mapping)

        with self.assertRaises(GenerationException):
            self.generation.generate(self.entities, False)


if __name__ == '__main__':
    unittest.main()

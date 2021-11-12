#!/usr/bin/env python
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
import os
import sys
import tempfile
import unittest

try:
    from generation import Generation, GenerationException
except ImportError:
    sys.path.append('../')
    from generation import Generation, GenerationException

from io import StringIO

from entity import Entity, EntityDB
from fragments import FragmentFile
from linker_script import LinkerScript
from output_commands import InputSectionDesc
from sdkconfig import SDKConfig

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

        with open('data/base.lf') as fragment_file_obj:
            fragment_file = FragmentFile(fragment_file_obj, self.sdkconfig)
            self.generation.add_fragments_from_file(fragment_file)

        self.entities = EntityDB()

        with open('data/libfreertos.a.txt') as objdump:
            self.entities.add_sections_info(objdump)

        with open('data/linker_script.ld') as linker_script:
            self.linker_script = LinkerScript(linker_script)

    @staticmethod
    def create_fragment_file(contents, name='test_fragment.lf'):
        f = StringIO(contents)
        f.name = name
        return f

    def add_fragments(self, text):
        fragment_file = self.create_fragment_file(text)
        fragment_file = FragmentFile(fragment_file, self.sdkconfig)
        self.generation.add_fragments_from_file(fragment_file)

    def write(self, expected, actual):
        self.linker_script.fill(expected)
        self.linker_script.write(open('expected.ld', 'w'))

        self.linker_script.fill(actual)
        self.linker_script.write(open('actual.ld', 'w'))

    def generate_default_rules(self):
        rules = collections.defaultdict(list)

        rules['flash_text'].append(InputSectionDesc(ROOT, ['.literal', '.literal.*', '.text', '.text.*'], []))
        rules['flash_rodata'].append(InputSectionDesc(ROOT, ['.rodata', '.rodata.*'], []))
        rules['dram0_data'].append(InputSectionDesc(ROOT, ['.data', '.data.*'], []))
        rules['dram0_data'].append(InputSectionDesc(ROOT, ['.dram', '.dram.*'], []))
        rules['dram0_bss'].append(InputSectionDesc(ROOT, ['.bss', '.bss.*'], []))
        rules['dram0_bss'].append(InputSectionDesc(ROOT, ['COMMON'], []))
        rules['iram0_text'].append(InputSectionDesc(ROOT, ['.iram', '.iram.*'], []))
        rules['rtc_text'].append(InputSectionDesc(ROOT, ['.rtc.text', '.rtc.literal'], []))
        rules['rtc_data'].append(InputSectionDesc(ROOT, ['.rtc.data'], []))
        rules['rtc_data'].append(InputSectionDesc(ROOT, ['.rtc.rodata'], []))
        rules['rtc_bss'].append(InputSectionDesc(ROOT, ['.rtc.bss'], []))

        return rules

    def compare_rules(self, expected, actual):
        self.assertEqual(set(expected.keys()), set(actual.keys()))

        for target in sorted(actual.keys()):
            a_cmds = actual[target]
            e_cmds = expected[target]

            self.assertEqual(len(a_cmds), len(e_cmds))

            for a, e in zip(a_cmds, e_cmds):
                self.assertEqual(a, e)

    def get_default(self, target, rules):
        return rules[target][0]

    def test_rule_generation_default(self):
        # Checks that default rules are generated from
        # the default scheme properly and even if no mappings
        # are defined.
        actual = self.generation.generate_rules(self.entities)
        expected = self.generate_default_rules()

        self.compare_rules(expected, actual)


class DefaultMappingTest(GenerationTest):

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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
            self.generation.generate_rules(self.entities)

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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
        actual = self.generation.generate_rules(self.entities)
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
            self.generation.generate_rules(self.entities)

    def test_disambiguated_obj(self):
        # Test command generation for disambiguated entry. Should produce similar
        # results to test_nondefault_mapping_symbol.
        mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    port.c:xPortGetTickRateHz (noflash)                 #1
"""
        port = Entity('libfreertos.a', 'port.c')
        self.add_fragments(mapping)
        actual = self.generation.generate_rules(self.entities)
        expected = self.generate_default_rules()

        flash_text = expected['flash_text']
        iram0_text = expected['iram0_text']

        # Generate exclusion in flash_text                                                A
        flash_text[0].exclusions.add(port)

        # Generate intermediate command                                                   B
        # List all relevant sections except the symbol
        # being mapped
        port_sections = self.entities.get_sections('libfreertos.a', 'port.c')
        filtered_sections = fnmatch.filter(port_sections, '.literal.*')
        filtered_sections.extend(fnmatch.filter(port_sections, '.text.*'))

        filtered_sections = [s for s in filtered_sections if not s.endswith('xPortGetTickRateHz')]
        filtered_sections.append('.text')

        flash_text.append(InputSectionDesc(port, set(filtered_sections), []))

        # Input section commands in iram_text for #1                                     C
        iram0_text.append(InputSectionDesc(port, set(['.text.xPortGetTickRateHz', '.literal.xPortGetTickRateHz']), []))

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

        actual = self.generation.generate_rules(self.entities)
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

            actual = self.generation.generate_rules(self.entities)
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

    def test_conditional_on_scheme_legacy_mapping_00(self):
        # Test use of conditional scheme on legacy mapping fragment grammar.
        mapping = u"""
[mapping]
archive: lib.a
entries:
    * (cond_noflash)
"""
        self._test_conditional_on_scheme(0, mapping)

    def test_conditional_on_scheme_legacy_mapping_01(self):
        # Test use of conditional scheme on legacy mapping fragment grammar.
        mapping = u"""
[mapping]
archive: lib.a
entries:
    * (cond_noflash)
"""
        self._test_conditional_on_scheme(0, mapping)

    def test_conditional_entries_legacy_mapping_fragment(self):
        # Test conditional entries on legacy mapping fragment grammar.
        mapping = u"""
[mapping]
archive: lib.a
entries:
    : PERFORMANCE_LEVEL = 0
    : PERFORMANCE_LEVEL = 1
    obj1 (noflash)
    : PERFORMANCE_LEVEL = 2
    obj1 (noflash)
    obj2 (noflash)
    : PERFORMANCE_LEVEL = 3
    obj1 (noflash)
    obj2 (noflash)
    obj3 (noflash)
"""
        self.test_conditional_mapping(mapping)

    def test_multiple_fragment_same_lib_conditional_legacy(self):
        # Test conditional entries on legacy mapping fragment grammar
        # across multiple fragments.
        mapping = u"""
[mapping]
archive: lib.a
entries:
    : PERFORMANCE_LEVEL = 0
    : PERFORMANCE_LEVEL = 1
    obj1 (noflash)
    : PERFORMANCE_LEVEL = 2
    obj1 (noflash)
    : PERFORMANCE_LEVEL = 3
    obj1 (noflash)

[mapping]
archive: lib.a
entries:
    : PERFORMANCE_LEVEL = 1
    obj1 (noflash) # ignore duplicate definition
    : PERFORMANCE_LEVEL = 2
    obj2 (noflash)
    : PERFORMANCE_LEVEL = 3
    obj2 (noflash)
    obj3 (noflash)
"""

        self.test_conditional_mapping(mapping)

    def test_multiple_fragment_same_lib_conditional(self):
        # Test conditional entries on new mapping fragment grammar.
        # across multiple fragments.
        mapping = u"""
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


if __name__ == '__main__':
    unittest.main()

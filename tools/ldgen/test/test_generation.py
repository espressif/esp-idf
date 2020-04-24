#!/usr/bin/env python
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

import unittest
import sys

try:
    from generation import PlacementRule
except ImportError:
    sys.path.append('../')
    from generation import PlacementRule

from generation import GenerationException
from generation import SectionsInfo
from generation import TemplateModel
from generation import GenerationModel

from fragments import FragmentFileModel
from fragments import Mapping
from fragments import Sections
from fragments import Scheme

from sdkconfig import SDKConfig


class GenerationModelTest(unittest.TestCase):

    def setUp(self):
        self.model = GenerationModel()
        self.sdkconfig = None
        self.sections_info = None
        self.script_model = None

        with open("data/Kconfig") as kconfig_file_obj:
            with open("data/sdkconfig") as sdkconfig_file_obj:
                self.sdkconfig = SDKConfig(kconfig_file_obj, sdkconfig_file_obj)

        with open("data/sample.lf") as fragment_file_obj:
            fragment_file = FragmentFileModel(fragment_file_obj)
            self.model.add_fragments_from_file(fragment_file)

        self.sections_info = SectionsInfo()

        with open("data/sections.info") as sections_info_file_obj:
            self.sections_info.add_sections_info(sections_info_file_obj)

        with open("data/template.ld") as template_file_obj:
            self.script_model = TemplateModel(template_file_obj)

    def _add_mapping(self, text):
        parser = Mapping.get_fragment_grammar()
        fragment = parser.parseString(text, parseAll=True)
        try:
            mappings = self.model.mappings[fragment[0].name]
        except KeyError:
            mappings = list()
            self.model.mappings[fragment[0].name] = mappings
        mappings.append(fragment[0])

    def _add_sections(self, text):
        parser = Sections.get_fragment_grammar()
        fragment = parser.parseString(text, parseAll=True)
        self.model.sections[fragment[0].name] = fragment[0]

    def _add_scheme(self, text):
        parser = Scheme.get_fragment_grammar()
        fragment = parser.parseString(text, parseAll=True)
        self.model.schemes[fragment[0].name] = fragment[0]

    def _write(self, expected, actual):
        self.script_model.fill(expected, self.sdkconfig)
        self.script_model.write(open("expected.ld", "w"))

        self.script_model.fill(actual, self.sdkconfig)
        self.script_model.write(open("actual.ld", "w"))

    def _generate_default_rules(self):
        rules = dict()

        # flash_text
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["text"].entries, "flash_text")
        placement_rules.append(rule)
        rules["flash_text"] = placement_rules

        # flash_rodata
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["rodata"].entries, "flash_rodata")
        placement_rules.append(rule)
        rules["flash_rodata"] = placement_rules

        # dram0_data
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["data"].entries + self.model.sections["dram"].entries, "dram0_data")
        placement_rules.append(rule)
        rules["dram0_data"] = placement_rules

        # dram0_bss
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "dram0_bss")
        placement_rules.append(rule)
        rules["dram0_bss"] = placement_rules

        # iram0_text
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["iram"].entries, "iram0_text")
        placement_rules.append(rule)
        rules["iram0_text"] = placement_rules

        # rtc_text
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["rtc_text"].entries, "rtc_text")
        placement_rules.append(rule)
        rules["rtc_text"] = placement_rules

        # rtc_data
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["rtc_data"].entries + self.model.sections["rtc_rodata"].entries, "rtc_data")
        placement_rules.append(rule)
        rules["rtc_data"] = placement_rules

        # rtc_bss
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["rtc_bss"].entries, "rtc_bss")
        placement_rules.append(rule)
        rules["rtc_bss"] = placement_rules

        return rules

    def _compare_rules(self, expected, actual):
        self.assertEqual(set(expected.keys()), set(actual.keys()))

        for (target, rules) in actual.items():

            message = "target: " + target

            actual_target_rules = rules
            expected_target_rules = expected[target]

            self.assertEqual(len(actual_target_rules), len(expected_target_rules))

            for actual_target_rule in actual_target_rules:
                self.assertTrue(actual_target_rule in expected_target_rules, message + str(actual_target_rule))

            for expected_target_rule in expected_target_rules:
                self.assertTrue(expected_target_rule in actual_target_rules, message + str(expected_target_rule))

    def _get_default(self, target, rules):
        return rules[target][0]

    def test_rule_generation_blank(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        self.assertEqual(expected, actual)

    def test_rule_generation_nominal_1(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            * (noflash)
        """
        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "*", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "*", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1)
        flash_rodata_default.add_exclusion(dram0_data_E1)

        # Add to the placement rules list
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_2(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            timers (rtc)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        # Add the exclusions
        flash_text_default.add_exclusion(rtc_text_E1)
        flash_rodata_default.add_exclusion(rtc_data_E1)
        dram0_data_default.add_exclusion(rtc_data_E1)
        dram0_bss_default.add_exclusion(rtc_bss_E1)

        # Add the rules
        expected["rtc_text"].append(rtc_text_E1)
        expected["rtc_data"].append(rtc_data_E1)
        expected["rtc_bss"].append(rtc_bss_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_3(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            timers (rtc)
            * (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", "*", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "*", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E2)
        flash_rodata_default.add_exclusion(dram0_data_E2)

        dram0_data_default.add_exclusion(rtc_data_E1)
        dram0_bss_default.add_exclusion(rtc_bss_E1)

        iram0_text_E2.add_exclusion(rtc_text_E1)
        dram0_data_E2.add_exclusion(rtc_data_E1)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E2)
        expected["dram0_data"].append(dram0_data_E2)

        expected["rtc_text"].append(rtc_text_E1)
        expected["rtc_data"].append(rtc_data_E1)
        expected["rtc_bss"].append(rtc_bss_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_4(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine (rtc)
            event_groups (noflash)
            timers (rtc)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)
        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        # Add the exclusions
        flash_text_default.add_exclusion(rtc_text_E3)
        flash_rodata_default.add_exclusion(rtc_data_E3)
        dram0_data_default.add_exclusion(rtc_data_E3)
        dram0_bss_default.add_exclusion(rtc_bss_E3)

        flash_text_default.add_exclusion(iram0_text_E2)
        flash_rodata_default.add_exclusion(dram0_data_E2)

        flash_text_default.add_exclusion(rtc_text_E1)
        flash_rodata_default.add_exclusion(rtc_data_E1)
        dram0_data_default.add_exclusion(rtc_data_E1)
        dram0_bss_default.add_exclusion(rtc_bss_E1)

        # Add the rules
        expected["rtc_text"].append(rtc_text_E3)
        expected["rtc_data"].append(rtc_data_E3)
        expected["rtc_bss"].append(rtc_bss_E3)

        expected["iram0_text"].append(iram0_text_E2)
        expected["dram0_data"].append(dram0_data_E2)

        expected["rtc_text"].append(rtc_text_E1)
        expected["rtc_data"].append(rtc_data_E1)
        expected["rtc_bss"].append(rtc_bss_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_5(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine (rtc)
            event_groups (noflash)
            timers (rtc)
            * (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E4 = PlacementRule("libfreertos.a", "*", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E4 = PlacementRule("libfreertos.a", "*", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E4)
        flash_rodata_default.add_exclusion(dram0_data_E4)

        iram0_text_E4.add_exclusion(rtc_text_E3)
        dram0_data_E4.add_exclusion(rtc_data_E3)
        dram0_data_default.add_exclusion(rtc_data_E3)
        dram0_bss_default.add_exclusion(rtc_bss_E3)

        iram0_text_E4.add_exclusion(iram0_text_E2)
        dram0_data_E4.add_exclusion(dram0_data_E2)

        iram0_text_E4.add_exclusion(rtc_text_E1)
        dram0_data_E4.add_exclusion(rtc_data_E1)
        dram0_data_default.add_exclusion(rtc_data_E1)
        dram0_bss_default.add_exclusion(rtc_bss_E1)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E4)
        expected["dram0_data"].append(dram0_data_E4)

        expected["rtc_text"].append(rtc_text_E3)
        expected["rtc_data"].append(rtc_data_E3)
        expected["rtc_bss"].append(rtc_bss_E3)

        expected["iram0_text"].append(iram0_text_E2)
        expected["dram0_data"].append(dram0_data_E2)

        expected["rtc_text"].append(rtc_text_E1)
        expected["rtc_data"].append(rtc_data_E1)
        expected["rtc_bss"].append(rtc_bss_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_6(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckPendingReadyList (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["rodata"].entries, "dram0_data")

        iram0_text_E1_extra = PlacementRule("libfreertos.a", "croutine", None, [".text.*", ".literal.*"], "flash_text")
        dram0_data_E1_extra = PlacementRule("libfreertos.a", "croutine", None, [".rodata.*"], "flash_rodata")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1_extra, self.sections_info)
        flash_rodata_default.add_exclusion(dram0_data_E1_extra, self.sections_info)

        iram0_text_E1_extra.add_exclusion(iram0_text_E1, self.sections_info)
        dram0_data_E1_extra.add_exclusion(dram0_data_E1, self.sections_info)

        # Add the rules
        expected["flash_text"].append(iram0_text_E1_extra)
        expected["flash_rodata"].append(dram0_data_E1_extra)

        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_7(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckPendingReadyList (noflash)
            croutine:prvCheckDelayedList (noflash)
            croutine:xCoRoutineCreate (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["rodata"].entries, "dram0_data")

        iram0_text_E2 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["rodata"].entries, "dram0_data")

        iram0_text_E3 = PlacementRule("libfreertos.a", "croutine", "xCoRoutineCreate", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E3 = PlacementRule("libfreertos.a", "croutine", "xCoRoutineCreate", self.model.sections["rodata"].entries, "dram0_data")

        flash_text_extra = PlacementRule("libfreertos.a", "croutine", None, [".text.*", ".literal.*"], "flash_text")
        flash_rodata_extra = PlacementRule("libfreertos.a", "croutine", None, [".rodata.*"], "flash_rodata")

        # Add the exclusions
        flash_text_default.add_exclusion(flash_text_extra, self.sections_info)
        flash_rodata_default.add_exclusion(flash_rodata_extra, self.sections_info)

        flash_text_extra.add_exclusion(iram0_text_E1, self.sections_info)
        flash_rodata_extra.add_exclusion(dram0_data_E1, self.sections_info)

        flash_text_extra.add_exclusion(iram0_text_E2, self.sections_info)
        flash_rodata_extra.add_exclusion(dram0_data_E2, self.sections_info)

        flash_text_extra.add_exclusion(iram0_text_E3, self.sections_info)
        flash_rodata_extra.add_exclusion(dram0_data_E3, self.sections_info)

        # Add the rules
        expected["flash_text"].append(flash_text_extra)
        expected["flash_rodata"].append(flash_rodata_extra)

        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        expected["iram0_text"].append(iram0_text_E2)
        expected["dram0_data"].append(dram0_data_E2)

        expected["iram0_text"].append(iram0_text_E3)
        expected["dram0_data"].append(dram0_data_E3)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_8(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckPendingReadyList (noflash)
            croutine:prvCheckDelayedList (rtc)
            croutine:xCoRoutineCreate (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E2 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E2 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                    self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E2 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                   self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E3 = PlacementRule("libfreertos.a", "croutine", "xCoRoutineCreate", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E3 = PlacementRule("libfreertos.a", "croutine", "xCoRoutineCreate", self.model.sections["rodata"].entries, "dram0_data")

        flash_text_extra = PlacementRule("libfreertos.a", "croutine", None, [".text.*", ".literal.*"], "flash_text")
        flash_rodata_extra = PlacementRule("libfreertos.a", "croutine", None, [".rodata.*"], "flash_rodata")
        dram0_data_extra = PlacementRule("libfreertos.a", "croutine", None, [".data.*"], "dram0_data")
        dram0_bss_extra = PlacementRule("libfreertos.a", "croutine", None, [".bss.*"], "dram0_bss")

        # Add the exclusions
        flash_text_default.add_exclusion(flash_text_extra, self.sections_info)
        flash_rodata_default.add_exclusion(flash_rodata_extra, self.sections_info)
        dram0_data_default.add_exclusion(dram0_data_extra, self.sections_info)
        dram0_bss_default.add_exclusion(dram0_bss_extra, self.sections_info)

        flash_text_extra.add_exclusion(iram0_text_E1, self.sections_info)
        flash_rodata_extra.add_exclusion(dram0_data_E1, self.sections_info)

        flash_text_extra.add_exclusion(rtc_text_E2, self.sections_info)
        dram0_data_extra.add_exclusion(rtc_data_E2, self.sections_info)
        flash_rodata_extra.add_exclusion(rtc_data_E2, self.sections_info)
        dram0_bss_extra.add_exclusion(rtc_bss_E2, self.sections_info)

        flash_text_extra.add_exclusion(iram0_text_E3, self.sections_info)
        flash_rodata_extra.add_exclusion(dram0_data_E3, self.sections_info)

        # Add the rules
        expected["flash_text"].append(flash_text_extra)
        expected["flash_rodata"].append(flash_rodata_extra)
        expected["dram0_data"].append(dram0_data_extra)
        expected["dram0_bss"].append(dram0_bss_extra)

        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        expected["rtc_text"].append(rtc_text_E2)
        expected["rtc_data"].append(rtc_data_E2)
        expected["rtc_bss"].append(rtc_bss_E2)

        expected["iram0_text"].append(iram0_text_E3)
        expected["dram0_data"].append(dram0_data_E3)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_9(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckDelayedList (rtc)
            croutine (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                    self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                   self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["rodata"].entries, "dram0_data")

        dram0_data_extra = PlacementRule("libfreertos.a", "croutine", None, [".data.*"], "dram0_data")
        dram0_bss_extra = PlacementRule("libfreertos.a", "croutine", None, [".bss.*"], "dram0_bss")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E2, self.sections_info)
        flash_rodata_default.add_exclusion(dram0_data_E2, self.sections_info)

        dram0_data_default.add_exclusion(dram0_data_extra, self.sections_info)
        dram0_bss_default.add_exclusion(dram0_bss_extra, self.sections_info)

        dram0_data_extra.add_exclusion(rtc_data_E1, self.sections_info)
        dram0_bss_extra.add_exclusion(rtc_bss_E1, self.sections_info)

        iram0_text_E2.add_exclusion(rtc_text_E1, self.sections_info)
        dram0_data_E2.add_exclusion(rtc_data_E1, self.sections_info)

        # Add the rules
        expected["dram0_data"].append(dram0_data_extra)
        expected["dram0_bss"].append(dram0_bss_extra)

        expected["iram0_text"].append(iram0_text_E2)
        expected["dram0_data"].append(dram0_data_E2)

        expected["rtc_text"].append(rtc_text_E1)
        expected["rtc_data"].append(rtc_data_E1)
        expected["rtc_bss"].append(rtc_bss_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_10(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckDelayedList (rtc)
            * (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        iram0_text_default = self._get_default("iram0_text", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                    self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                   self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", None, None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", None, None, self.model.sections["rodata"].entries, "dram0_data")

        iram0_text_extra = PlacementRule("libfreertos.a", "croutine", None, [".text.*", ".literal.*"], "iram0_text")
        dram0_data_extra = PlacementRule("libfreertos.a", "croutine", None, [".data.*", ".rodata.*"], "dram0_data")
        dram0_bss_extra = PlacementRule("libfreertos.a", "croutine", None, [".bss.*"], "dram0_bss")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E2, self.sections_info)
        flash_rodata_default.add_exclusion(dram0_data_E2, self.sections_info)

        iram0_text_default.add_exclusion(iram0_text_extra, self.sections_info)
        dram0_data_default.add_exclusion(dram0_data_extra, self.sections_info)
        dram0_bss_default.add_exclusion(dram0_bss_extra, self.sections_info)

        iram0_text_E2.add_exclusion(iram0_text_extra, self.sections_info)
        dram0_data_E2.add_exclusion(dram0_data_extra, self.sections_info)

        iram0_text_extra.add_exclusion(rtc_text_E1, self.sections_info)
        dram0_data_extra.add_exclusion(rtc_data_E1, self.sections_info)
        dram0_bss_extra.add_exclusion(rtc_bss_E1, self.sections_info)

        # Add the rules
        expected["iram0_text"].append(iram0_text_extra)
        expected["dram0_data"].append(dram0_data_extra)
        expected["dram0_bss"].append(dram0_bss_extra)

        expected["iram0_text"].append(iram0_text_E2)
        expected["dram0_data"].append(dram0_data_E2)

        expected["rtc_text"].append(rtc_text_E1)
        expected["rtc_data"].append(rtc_data_E1)
        expected["rtc_bss"].append(rtc_bss_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_11(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckDelayedList (noflash)
            croutine (rtc)
            * (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        iram0_text_default = self._get_default("iram0_text", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E3 = PlacementRule("libfreertos.a", None, None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E3 = PlacementRule("libfreertos.a", None, None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E3, self.sections_info)
        flash_rodata_default.add_exclusion(dram0_data_E3, self.sections_info)
        iram0_text_default.add_exclusion(rtc_text_E2, self.sections_info)
        dram0_data_default.add_exclusion(rtc_data_E2, self.sections_info)
        dram0_bss_default.add_exclusion(rtc_bss_E2, self.sections_info)

        iram0_text_E3.add_exclusion(rtc_text_E2, self.sections_info)
        dram0_data_E3.add_exclusion(rtc_data_E2, self.sections_info)

        rtc_text_E2.add_exclusion(iram0_text_E1, self.sections_info)
        rtc_data_E2.add_exclusion(dram0_data_E1, self.sections_info)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        expected["rtc_text"].append(rtc_text_E2)
        expected["rtc_data"].append(rtc_data_E2)
        expected["rtc_bss"].append(rtc_bss_E2)

        expected["iram0_text"].append(iram0_text_E3)
        expected["dram0_data"].append(dram0_data_E3)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_12(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckDelayedList (rtc)
            croutine (noflash)
            * (rtc)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                    self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                   self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E3 = PlacementRule("libfreertos.a", None, None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", None, None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", None, None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        rtc_data_extra = PlacementRule("libfreertos.a", "croutine", None, [".data.*"], "rtc_data")
        rtc_bss_extra = PlacementRule("libfreertos.a", "croutine", None, [".bss.*"], "rtc_bss")

        # Add the exclusions
        flash_text_default.add_exclusion(rtc_text_E3, self.sections_info)
        flash_rodata_default.add_exclusion(rtc_data_E3, self.sections_info)
        dram0_data_default.add_exclusion(rtc_data_E3, self.sections_info)
        dram0_bss_default.add_exclusion(rtc_bss_E3, self.sections_info)

        rtc_text_E3.add_exclusion(iram0_text_E2, self.sections_info)
        rtc_data_E3.add_exclusion(dram0_data_E2, self.sections_info)
        rtc_data_E3.add_exclusion(rtc_data_extra, self.sections_info)
        rtc_bss_E3.add_exclusion(rtc_bss_extra, self.sections_info)

        rtc_data_extra.add_exclusion(rtc_data_E1, self.sections_info)
        rtc_bss_extra.add_exclusion(rtc_bss_E1, self.sections_info)
        iram0_text_E2.add_exclusion(rtc_text_E1, self.sections_info)
        dram0_data_E2.add_exclusion(rtc_data_E1, self.sections_info)

        # Add the rules
        expected["rtc_data"].append(rtc_data_extra)
        expected["rtc_bss"].append(rtc_bss_extra)

        expected["rtc_text"].append(rtc_text_E1)
        expected["rtc_data"].append(rtc_data_E1)
        expected["rtc_bss"].append(rtc_bss_E1)

        expected["iram0_text"].append(iram0_text_E2)
        expected["dram0_data"].append(dram0_data_E2)

        expected["rtc_text"].append(rtc_text_E3)
        expected["rtc_data"].append(rtc_data_E3)
        expected["rtc_bss"].append(rtc_bss_E3)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_13(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckDelayedList (noflash)
            event_groups:xEventGroupCreate (noflash)
            croutine (rtc)
            event_groups (rtc)
            * (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["rodata"].entries, "dram0_data")

        iram0_text_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate", self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        rtc_text_E4 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E4 = PlacementRule("libfreertos.a", "event_groups", None,
                                    self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E4 = PlacementRule("libfreertos.a", "event_groups", None,
                                   self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E5 = PlacementRule("libfreertos.a", None, None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E5 = PlacementRule("libfreertos.a", None, None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E5, self.sections_info)
        flash_rodata_default.add_exclusion(dram0_data_E5, self.sections_info)
        dram0_bss_default.add_exclusion(rtc_bss_E3, self.sections_info)
        dram0_data_default.add_exclusion(rtc_data_E3, self.sections_info)
        dram0_bss_default.add_exclusion(rtc_bss_E4, self.sections_info)
        dram0_data_default.add_exclusion(rtc_data_E4, self.sections_info)

        iram0_text_E5.add_exclusion(rtc_text_E3, self.sections_info)
        dram0_data_E5.add_exclusion(rtc_data_E3, self.sections_info)
        iram0_text_E5.add_exclusion(rtc_text_E4, self.sections_info)
        dram0_data_E5.add_exclusion(rtc_data_E4, self.sections_info)

        rtc_text_E4.add_exclusion(iram0_text_E2, self.sections_info)
        rtc_data_E4.add_exclusion(dram0_data_E2, self.sections_info)

        rtc_text_E3.add_exclusion(iram0_text_E1, self.sections_info)
        rtc_data_E3.add_exclusion(dram0_data_E1, self.sections_info)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        expected["iram0_text"].append(iram0_text_E2)
        expected["dram0_data"].append(dram0_data_E2)

        expected["rtc_text"].append(rtc_text_E3)
        expected["rtc_data"].append(rtc_data_E3)
        expected["rtc_bss"].append(rtc_bss_E3)

        expected["rtc_text"].append(rtc_text_E4)
        expected["rtc_data"].append(rtc_data_E4)
        expected["rtc_bss"].append(rtc_bss_E4)

        expected["iram0_text"].append(iram0_text_E5)
        expected["dram0_data"].append(dram0_data_E5)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_14(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckDelayedList (noflash)
            event_groups:xEventGroupCreate (rtc)
            croutine (rtc)
            event_groups (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)
        dram0_data_default = self._get_default("dram0_data", expected)
        dram0_bss_default = self._get_default("dram0_bss", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate",
                                    self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate",
                                   self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        rtc_text_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries + self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries + self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E4 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E4 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["rodata"].entries, "dram0_data")

        dram0_data_extra = PlacementRule("libfreertos.a", "event_groups", None, [".data.*"], "dram0_data")
        dram0_bss_extra = PlacementRule("libfreertos.a", "event_groups", None, [".bss.*"], "dram0_bss")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E4, self.sections_info)
        flash_rodata_default.add_exclusion(dram0_data_E4, self.sections_info)
        dram0_data_default.add_exclusion(dram0_data_extra, self.sections_info)
        dram0_bss_default.add_exclusion(dram0_bss_extra, self.sections_info)

        flash_text_default.add_exclusion(rtc_text_E3, self.sections_info)
        flash_rodata_default.add_exclusion(rtc_data_E3, self.sections_info)
        dram0_data_default.add_exclusion(rtc_data_E3, self.sections_info)
        dram0_bss_default.add_exclusion(rtc_bss_E3, self.sections_info)

        iram0_text_E4.add_exclusion(rtc_text_E2, self.sections_info)
        dram0_data_E4.add_exclusion(rtc_data_E2, self.sections_info)
        dram0_data_extra.add_exclusion(rtc_data_E2, self.sections_info)
        dram0_bss_extra.add_exclusion(rtc_bss_E2, self.sections_info)

        rtc_text_E3.add_exclusion(iram0_text_E1, self.sections_info)
        rtc_data_E3.add_exclusion(dram0_data_E1, self.sections_info)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        expected["rtc_text"].append(rtc_text_E2)
        expected["rtc_data"].append(rtc_data_E2)
        expected["rtc_bss"].append(rtc_bss_E2)

        expected["rtc_text"].append(rtc_text_E3)
        expected["rtc_data"].append(rtc_data_E3)
        expected["rtc_bss"].append(rtc_bss_E3)

        expected["iram0_text"].append(iram0_text_E4)
        expected["dram0_data"].append(dram0_data_E4)

        expected["dram0_data"].append(dram0_data_extra)
        expected["dram0_bss"].append(dram0_bss_extra)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_15(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine (noflash_data)
            croutine (noflash_text)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1)
        flash_rodata_default.add_exclusion(dram0_data_E1)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_nominal_16(self):
        normal = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine (noflash_data)
            croutine (noflash)
        """

        self._add_mapping(normal)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)
        flash_rodata_default = self._get_default("flash_rodata", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1)
        flash_rodata_default.add_exclusion(dram0_data_E1)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        self._compare_rules(expected, actual)

    def test_rule_generation_conflict(self):
        conflict_mapping = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine (conflict)
            croutine (noflash)
        """

        conflict_scheme = """
        [scheme:conflict]
        entries:
            rodata -> dram0_data
            bss -> dram0_data
        """

        self._add_scheme(conflict_scheme)
        self._add_mapping(conflict_mapping)

        with self.assertRaises(GenerationException):
            self.model.generate_rules(self.sdkconfig, self.sections_info)

    def test_rule_generation_condition(self):
        generation_with_condition = """
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

        self._add_mapping(generation_with_condition)

        for perf_level in range(0, 4):
            self.sdkconfig.config.syms["PERFORMANCE_LEVEL"].set_value(str(perf_level))

            actual = self.model.generate_rules(self.sdkconfig, self.sections_info)
            expected = self._generate_default_rules()

            flash_text_default = self._get_default("flash_text", expected)
            flash_rodata_default = self._get_default("flash_rodata", expected)

            if perf_level < 4:
                for append_no in range(1, perf_level + 1):
                    iram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["text"].entries, "iram0_text")
                    dram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["rodata"].entries, "dram0_data")

                    flash_text_default.add_exclusion(iram_rule)
                    flash_rodata_default.add_exclusion(dram_rule)

                    expected["iram0_text"].append(iram_rule)
                    expected["dram0_data"].append(dram_rule)

            self._compare_rules(expected, actual)

    def test_rule_generation_multiple_mapping_definitions(self):
        generation_with_condition1 = """
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
        """

        generation_with_condition2 = """
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

        self._add_mapping(generation_with_condition1)
        self._add_mapping(generation_with_condition2)

        for perf_level in range(0, 4):
            self.sdkconfig.config.syms["PERFORMANCE_LEVEL"].set_value(str(perf_level))

            actual = self.model.generate_rules(self.sdkconfig, self.sections_info)
            expected = self._generate_default_rules()

            flash_text_default = self._get_default("flash_text", expected)
            flash_rodata_default = self._get_default("flash_rodata", expected)

            if perf_level < 4:
                for append_no in range(1, perf_level + 1):
                    iram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["text"].entries, "iram0_text")
                    dram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["rodata"].entries, "dram0_data")

                    flash_text_default.add_exclusion(iram_rule)
                    flash_rodata_default.add_exclusion(dram_rule)

                    expected["iram0_text"].append(iram_rule)
                    expected["dram0_data"].append(dram_rule)

            self._compare_rules(expected, actual)

    def test_rules_order(self):
        # The fragments are structured such that ldgen will:
        #  - parse freertos2 mapping first
        #  - entry for prvCheckPendingReadyList is parsed first before prvCheckDelayedList
        # We expect that despite this, ldgen will output rules in a set order:
        # by increasing specificity and alphabetically
        rules_order_mapping_1 = """
        [mapping]
        archive: libfreertos2.a
        entries:
            croutine2 (noflash_text)
            croutine (noflash_text)
        """

        rules_order_mapping_2 = """
        [mapping]
        archive: libfreertos.a
        entries:
            croutine:prvCheckPendingReadyList (noflash_text)
            croutine:prvCheckDelayedList (noflash_text)
        """

        self._add_mapping(rules_order_mapping_2)
        self._add_mapping(rules_order_mapping_1)

        actual = self.model.generate_rules(self.sdkconfig, self.sections_info)

        expected = self._generate_default_rules()

        flash_text_default = self._get_default("flash_text", expected)

        iram0_text_E1 = PlacementRule("libfreertos2.a", "croutine2", None, self.model.sections["text"].entries, "iram0_text")
        iram0_text_E2 = PlacementRule("libfreertos2.a", "croutine", None, self.model.sections["text"].entries, "iram0_text")
        iram0_text_E3 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["text"].entries, "iram0_text")
        iram0_text_E4 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "iram0_text")

        flash_text_extra = PlacementRule("libfreertos.a", "croutine", None, [".text.*", ".literal.*"], "flash_text")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1, self.sections_info)
        flash_text_default.add_exclusion(iram0_text_E2, self.sections_info)

        flash_text_default.add_exclusion(flash_text_extra, self.sections_info)
        flash_text_extra.add_exclusion(iram0_text_E3, self.sections_info)
        flash_text_extra.add_exclusion(iram0_text_E4, self.sections_info)

        # Add the rules, arranged by expected order
        expected["flash_text"].append(flash_text_extra)
        expected["iram0_text"].append(iram0_text_E4)
        expected["iram0_text"].append(iram0_text_E3)
        expected["iram0_text"].append(iram0_text_E2)
        expected["iram0_text"].append(iram0_text_E1)

        # Perform general comparison for all sections
        self._compare_rules(expected, actual)

        # Perform ordered comparison
        self.assertListEqual(actual["flash_text"], expected["flash_text"])


if __name__ == "__main__":
    unittest.main()

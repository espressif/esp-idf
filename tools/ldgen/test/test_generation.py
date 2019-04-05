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

from fragments import FragmentFile

from sdkconfig import SDKConfig
from io import StringIO


class GenerationModelTest(unittest.TestCase):

    def setUp(self):
        self.model = GenerationModel()
        self.sections_info = None
        self.script_model = None

        self.sdkconfig = SDKConfig("data/Kconfig", "data/sdkconfig")

        with open("data/sample.lf") as fragment_file_obj:
            fragment_file = FragmentFile(fragment_file_obj, self.sdkconfig)
            self.model.add_fragments_from_file(fragment_file)

        self.sections_info = SectionsInfo()

        with open("data/sections.info") as sections_info_file_obj:
            self.sections_info.add_sections_info(sections_info_file_obj)

        with open("data/template.ld") as template_file_obj:
            self.script_model = TemplateModel(template_file_obj)

    @staticmethod
    def create_fragment_file(contents, name="test_fragment.lf"):
        f = StringIO(contents)
        f.name = name
        return f

    def add_fragments(self, text):
        fragment_file = self.create_fragment_file(text)
        fragment_file = FragmentFile(fragment_file, self.sdkconfig)
        self.model.add_fragments_from_file(fragment_file)

    def write(self, expected, actual):
        self.script_model.fill(expected)
        self.script_model.write(open("expected.ld", "w"))

        self.script_model.fill(actual)
        self.script_model.write(open("actual.ld", "w"))

    def generate_default_rules(self):
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
        rule = PlacementRule(None, None, None, self.model.sections["data"].entries | self.model.sections["dram"].entries, "dram0_data")
        placement_rules.append(rule)
        rules["dram0_data"] = placement_rules

        # dram0_bss
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "dram0_bss")
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
        rule = PlacementRule(None, None, None, self.model.sections["rtc_data"].entries | self.model.sections["rtc_rodata"].entries, "rtc_data")
        placement_rules.append(rule)
        rules["rtc_data"] = placement_rules

        # rtc_bss
        placement_rules = list()
        rule = PlacementRule(None, None, None, self.model.sections["rtc_bss"].entries, "rtc_bss")
        placement_rules.append(rule)
        rules["rtc_bss"] = placement_rules

        return rules

    def compare_rules(self, expected, actual):
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

    def get_default(self, target, rules):
        return rules[target][0]

    def test_rule_generation_default(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (default)
"""

        self.add_fragments(normal)
        actual = self.model.generate_rules(self.sections_info)
        expected = self.generate_default_rules()

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_1(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    * (noflash)
"""
        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "*", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "*", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1)
        flash_rodata_default.add_exclusion(dram0_data_E1)

        # Add to the placement rules list
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_2(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    timers (rtc)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

        # Add the exclusions
        flash_text_default.add_exclusion(rtc_text_E1)
        flash_rodata_default.add_exclusion(rtc_data_E1)
        dram0_data_default.add_exclusion(rtc_data_E1)
        dram0_bss_default.add_exclusion(rtc_bss_E1)

        # Add the rules
        expected["rtc_text"].append(rtc_text_E1)
        expected["rtc_data"].append(rtc_data_E1)
        expected["rtc_bss"].append(rtc_bss_E1)

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_3(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    timers (rtc)
    * (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_4(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (rtc)
    event_groups (noflash)
    timers (rtc)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_5(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (rtc)
    event_groups (noflash)
    timers (rtc)
    * (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", "timers", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_6(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckPendingReadyList (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_7(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckPendingReadyList (noflash)
    croutine:prvCheckDelayedList (noflash)
    croutine:xCoRoutineCreate (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_8(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckPendingReadyList (noflash)
    croutine:prvCheckDelayedList (rtc)
    croutine:xCoRoutineCreate (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckPendingReadyList", self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E2 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E2 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                    self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E2 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                   self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_9(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckDelayedList (rtc)
    croutine (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                    self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                   self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_10(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckDelayedList (rtc)
    * (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        iram0_text_default = self.get_default("iram0_text", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                    self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                   self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_11(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckDelayedList (noflash)
    croutine (rtc)
    * (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        iram0_text_default = self.get_default("iram0_text", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_12(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckDelayedList (rtc)
    croutine (noflash)
    * (rtc)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        rtc_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                    self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList",
                                   self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

        iram0_text_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E3 = PlacementRule("libfreertos.a", None, None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", None, None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", None, None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_13(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckDelayedList (noflash)
    event_groups:xEventGroupCreate (noflash)
    croutine (rtc)
    event_groups (rtc)
    * (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["rodata"].entries, "dram0_data")

        iram0_text_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate", self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

        rtc_text_E4 = PlacementRule("libfreertos.a", "event_groups", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E4 = PlacementRule("libfreertos.a", "event_groups", None,
                                    self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E4 = PlacementRule("libfreertos.a", "event_groups", None,
                                   self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_14(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine:prvCheckDelayedList (noflash)
    event_groups:xEventGroupCreate (rtc)
    croutine (rtc)
    event_groups (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)
        dram0_data_default = self.get_default("dram0_data", expected)
        dram0_bss_default = self.get_default("dram0_bss", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", "prvCheckDelayedList", self.model.sections["rodata"].entries, "dram0_data")

        rtc_text_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate", self.model.sections["text"].entries, "rtc_text")
        rtc_data_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate",
                                    self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E2 = PlacementRule("libfreertos.a", "event_groups", "xEventGroupCreate",
                                   self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

        rtc_text_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "rtc_text")
        rtc_data_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["data"].entries | self.model.sections["rodata"].entries, "rtc_data")
        rtc_bss_E3 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["bss"].entries | self.model.sections["common"].entries, "rtc_bss")

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

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_15(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash_data)
    croutine (noflash_text)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1)
        flash_rodata_default.add_exclusion(dram0_data_E1)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        self.compare_rules(expected, actual)

    def test_rule_generation_nominal_16(self):
        normal = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (noflash_data)
    croutine (noflash)
"""

        self.add_fragments(normal)

        actual = self.model.generate_rules(self.sections_info)

        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)
        flash_rodata_default = self.get_default("flash_rodata", expected)

        iram0_text_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["text"].entries, "iram0_text")
        dram0_data_E1 = PlacementRule("libfreertos.a", "croutine", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1)
        flash_rodata_default.add_exclusion(dram0_data_E1)

        # Add the rules
        expected["iram0_text"].append(iram0_text_E1)
        expected["dram0_data"].append(dram0_data_E1)

        self.compare_rules(expected, actual)

    def test_rule_generation_conflict(self):
        conflict_mapping = u"""
[mapping:test]
archive: libfreertos.a
entries:
    croutine (conflict)
    croutine (noflash)

[scheme:conflict]
entries:
    rodata -> dram0_data
    bss -> dram0_data
"""
        self.add_fragments(conflict_mapping)

        with self.assertRaises(GenerationException):
            self.model.generate_rules(self.sections_info)

    def test_rule_generation_condition(self):
        generation_with_condition = u"""
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
    else: # PERFORMANCE_LEVEL = 0
        * (default)
"""

        for perf_level in range(0, 4):
            self.sdkconfig.config.syms["PERFORMANCE_LEVEL"].set_value(str(perf_level))

            self.model.mappings = {}
            self.add_fragments(generation_with_condition)

            actual = self.model.generate_rules(self.sections_info)
            expected = self.generate_default_rules()

            if perf_level < 4:
                for append_no in range(1, perf_level + 1):
                    flash_text_default = self.get_default("flash_text", expected)
                    flash_rodata_default = self.get_default("flash_rodata", expected)

                    iram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["text"].entries, "iram0_text")
                    dram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["rodata"].entries, "dram0_data")

                    flash_text_default.add_exclusion(iram_rule)
                    flash_rodata_default.add_exclusion(dram_rule)

                    expected["iram0_text"].append(iram_rule)
                    expected["dram0_data"].append(dram_rule)

            self.compare_rules(expected, actual)

    def test_rule_generation_empty_entries(self):
        normal = u"""
[mapping:test]
archive: lib.a
entries:
    if PERFORMANCE_LEVEL >= 1: # is false, generate no special rules
        obj.a (noflash)
"""

        self.add_fragments(normal)
        actual = self.model.generate_rules(self.sections_info)
        expected = self.generate_default_rules()  # only default rules
        self.compare_rules(expected, actual)

    def test_conditional_sections_1(self):
        generation_with_condition = u"""
[sections:cond_text_data]
entries:
    if PERFORMANCE_LEVEL >= 1:
        .text+
        .literal+
    else:
        .data+

[scheme:cond_noflash]
entries:
    if PERFORMANCE_LEVEL >= 1:
        cond_text_data -> iram0_text
    else:
        cond_text_data -> dram0_data

[mapping:test]
archive: lib.a
entries:
    * (cond_noflash)
"""

        self.sdkconfig.config.syms["PERFORMANCE_LEVEL"].set_value("1")
        self.add_fragments(generation_with_condition)

        actual = self.model.generate_rules(self.sections_info)
        expected = self.generate_default_rules()

        flash_text_default = self.get_default("flash_text", expected)

        iram0_text_E1 = PlacementRule("lib.a", "*", None, self.model.sections["text"].entries, "iram0_text")

        # Add the exclusions
        flash_text_default.add_exclusion(iram0_text_E1)

        # Add to the placement rules list
        expected["iram0_text"].append(iram0_text_E1)

        self.compare_rules(expected, actual)

    def test_conditional_sections_2(self):
        generation_with_condition = u"""
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

[mapping:test]
archive: lib.a
entries:
    * (cond_noflash)
"""

        self.sdkconfig.config.syms["PERFORMANCE_LEVEL"].set_value("0")
        self.add_fragments(generation_with_condition)

        actual = self.model.generate_rules(self.sections_info)
        expected = self.generate_default_rules()

        flash_rodata_default = self.get_default("flash_rodata", expected)

        dram0_data_E1 = PlacementRule("lib.a", "*", None, self.model.sections["rodata"].entries, "dram0_data")

        # Add the exclusions
        flash_rodata_default.add_exclusion(dram0_data_E1)

        # Add to the placement rules list
        expected["dram0_data"].append(dram0_data_E1)

        self.compare_rules(expected, actual)

    def test_rule_generation_condition_with_deprecated_mapping(self):
        generation_with_condition = u"""
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

        for perf_level in range(0, 4):
            self.sdkconfig.config.syms["PERFORMANCE_LEVEL"].set_value(str(perf_level))

            self.model.mappings = {}
            self.add_fragments(generation_with_condition)

            actual = self.model.generate_rules(self.sections_info)
            expected = self.generate_default_rules()

            if perf_level < 4:
                for append_no in range(1, perf_level + 1):
                    flash_text_default = self.get_default("flash_text", expected)
                    flash_rodata_default = self.get_default("flash_rodata", expected)

                    iram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["text"].entries, "iram0_text")
                    dram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["rodata"].entries, "dram0_data")

                    flash_text_default.add_exclusion(iram_rule)
                    flash_rodata_default.add_exclusion(dram_rule)

                    expected["iram0_text"].append(iram_rule)
                    expected["dram0_data"].append(dram_rule)

            self.compare_rules(expected, actual)

    def test_rule_generation_multiple_deprecated_mapping_definitions(self):
        multiple_deprecated_definitions = u"""
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

        for perf_level in range(0, 4):
            self.sdkconfig.config.syms["PERFORMANCE_LEVEL"].set_value(str(perf_level))

            self.model.mappings = {}
            self.add_fragments(multiple_deprecated_definitions)

            actual = self.model.generate_rules(self.sections_info)
            expected = self.generate_default_rules()

            if perf_level < 4:
                for append_no in range(1, perf_level + 1):
                    flash_text_default = self.get_default("flash_text", expected)
                    flash_rodata_default = self.get_default("flash_rodata", expected)

                    iram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["text"].entries, "iram0_text")
                    dram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["rodata"].entries, "dram0_data")

                    flash_text_default.add_exclusion(iram_rule)
                    flash_rodata_default.add_exclusion(dram_rule)

                    expected["iram0_text"].append(iram_rule)
                    expected["dram0_data"].append(dram_rule)

            self.compare_rules(expected, actual)

    def test_rule_generation_multiple_mapping_definitions(self):
        multiple_deprecated_definitions = u"""
[mapping:base]
archive: lib.a
entries:
    if PERFORMANCE_LEVEL = 1:
        obj1 (noflash)
    elif PERFORMANCE_LEVEL = 2:
        obj1 (noflash)
    elif PERFORMANCE_LEVEL = 3:
        obj1 (noflash)
    else:
        * (default)

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
    else:
        * (default)
"""

        for perf_level in range(0, 4):
            self.sdkconfig.config.syms["PERFORMANCE_LEVEL"].set_value(str(perf_level))

            self.model.mappings = {}
            self.add_fragments(multiple_deprecated_definitions)

            actual = self.model.generate_rules(self.sections_info)
            expected = self.generate_default_rules()

            if perf_level < 4:
                for append_no in range(1, perf_level + 1):
                    flash_text_default = self.get_default("flash_text", expected)
                    flash_rodata_default = self.get_default("flash_rodata", expected)

                    iram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["text"].entries, "iram0_text")
                    dram_rule = PlacementRule("lib.a", "obj" + str(append_no), None, self.model.sections["rodata"].entries, "dram0_data")

                    flash_text_default.add_exclusion(iram_rule)
                    flash_rodata_default.add_exclusion(dram_rule)

                    expected["iram0_text"].append(iram_rule)
                    expected["dram0_data"].append(dram_rule)

            self.compare_rules(expected, actual)


if __name__ == "__main__":
    unittest.main()

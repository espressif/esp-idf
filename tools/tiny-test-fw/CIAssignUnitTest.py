"""
Command line tool to assign unit tests to CI test jobs.
"""

import re
import os
import sys
import argparse

import yaml

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader

try:
    from Utility import CIAssignTest
except ImportError:
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path:
        sys.path.insert(0, test_fw_path)
    from Utility import CIAssignTest


class Group(CIAssignTest.Group):
    SORT_KEYS = ["config", "test environment", "multi_device", "multi_stage", "tags", "chip_target"]
    MAX_CASE = 30
    ATTR_CONVERT_TABLE = {
        "execution_time": "execution time"
    }
    CI_JOB_MATCH_KEYS = ["test environment"]

    def __init__(self, case):
        super(Group, self).__init__(case)
        for tag in self._get_case_attr(case, "tags"):
            self.ci_job_match_keys.add(tag)

    @staticmethod
    def _get_case_attr(case, attr):
        if attr in Group.ATTR_CONVERT_TABLE:
            attr = Group.ATTR_CONVERT_TABLE[attr]
        return case[attr]

    def _create_extra_data(self, test_function):
        """
        For unit test case, we need to copy some attributes of test cases into config file.
        So unit test function knows how to run the case.
        """
        case_data = []
        for case in self.case_list:
            one_case_data = {
                "config": self._get_case_attr(case, "config"),
                "name": self._get_case_attr(case, "summary"),
                "reset": self._get_case_attr(case, "reset"),
                "timeout": self._get_case_attr(case, "timeout"),
            }

            if test_function in ["run_multiple_devices_cases", "run_multiple_stage_cases"]:
                try:
                    one_case_data["child case num"] = self._get_case_attr(case, "child case num")
                except KeyError as e:
                    print("multiple devices/stages cases must contains at least two test functions")
                    print("case name: {}".format(one_case_data["name"]))
                    raise e

            case_data.append(one_case_data)
        return case_data

    def _map_test_function(self):
        """
        determine which test function to use according to current test case

        :return: test function name to use
        """
        if self.filters["multi_device"] == "Yes":
            test_function = "run_multiple_devices_cases"
        elif self.filters["multi_stage"] == "Yes":
            test_function = "run_multiple_stage_cases"
        else:
            test_function = "run_unit_test_cases"
        return test_function

    def output(self):
        """
        output data for job configs

        :return: {"Filter": case filter, "CaseConfig": list of case configs for cases in this group}
        """
        test_function = self._map_test_function()

        output_data = {
            # we don't need filter for test function, as UT uses a few test functions for all cases
            "CaseConfig": [
                {
                    "name": test_function,
                    "extra_data": self._create_extra_data(test_function),
                }
            ],
        }

        target = self._get_case_attr(self.case_list[0], "chip_target")
        if target is not None:
            target_dut = {
                "esp32": "ESP32DUT",
                "esp32s2beta": "ESP32S2DUT",
                "esp8266": "ESP8266DUT",
            }[target]
            output_data.update({
                "Filter": {
                    "overwrite": {
                        "dut": {
                            "path": "IDF/IDFDUT.py",
                            "class": target_dut,
                        }
                    }
                }
            })
        return output_data


class UnitTestAssignTest(CIAssignTest.AssignTest):
    CI_TEST_JOB_PATTERN = re.compile(r"^UT_.+")

    def __init__(self, test_case_path, ci_config_file):
        CIAssignTest.AssignTest.__init__(self, test_case_path, ci_config_file, case_group=Group)

    def _search_cases(self, test_case_path, case_filter=None):
        """
        For unit test case, we don't search for test functions.
        The unit test cases is stored in a yaml file which is created in job build-idf-test.
        """

        try:
            with open(test_case_path, "r") as f:
                raw_data = yaml.load(f, Loader=Loader)
            test_cases = raw_data["test cases"]
        except IOError:
            print("Test case path is invalid. Should only happen when use @bot to skip unit test.")
            test_cases = []
        # filter keys are lower case. Do map lower case keys with original keys.
        try:
            key_mapping = {x.lower(): x for x in test_cases[0].keys()}
        except IndexError:
            key_mapping = dict()
        if case_filter:
            for key in case_filter:
                filtered_cases = []
                for case in test_cases:
                    try:
                        mapped_key = key_mapping[key]
                        # bot converts string to lower case
                        if isinstance(case[mapped_key], str):
                            _value = case[mapped_key].lower()
                        else:
                            _value = case[mapped_key]
                        if _value in case_filter[key]:
                            filtered_cases.append(case)
                    except KeyError:
                        # case don't have this key, regard as filter success
                        filtered_cases.append(case)
                test_cases = filtered_cases
        return test_cases


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("test_case",
                        help="test case folder or file")
    parser.add_argument("ci_config_file",
                        help="gitlab ci config file")
    parser.add_argument("output_path",
                        help="output path of config files")
    args = parser.parse_args()

    assign_test = UnitTestAssignTest(args.test_case, args.ci_config_file)
    assign_test.assign_cases()
    assign_test.output_configs(args.output_path)

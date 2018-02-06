"""
Command line tool to assign unit tests to CI test jobs.
"""

import re
import os
import sys
import argparse

import yaml

test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path:
    sys.path.insert(0, test_fw_path)

from Utility import CIAssignTest


class Group(CIAssignTest.Group):
    SORT_KEYS = ["Test App", "SDK", "test environment"]
    MAX_CASE = 30
    ATTR_CONVERT_TABLE = {
        "execution_time": "execution time"
    }

    @staticmethod
    def _get_case_attr(case, attr):
        if attr in Group.ATTR_CONVERT_TABLE:
            attr = Group.ATTR_CONVERT_TABLE[attr]
        return case[attr]

    @staticmethod
    def _get_ut_config(test_app):
        # we format test app "UT_ + config" when parsing test cases
        # now we need to extract config
        assert test_app[:3] == "UT_"
        return test_app[3:]

    def _create_extra_data(self):
        case_data = []
        for case in self.case_list:
            if self._get_case_attr(case, "cmd set") == "multiple_devices_case":
                case_data.append({
                    "config": self._get_ut_config(self._get_case_attr(case, "Test App")),
                    "name": self._get_case_attr(case, "summary"),
                    "child case num": self._get_case_attr(case, "child case num")
                })
            else:
                case_data.append({
                    "config": self._get_ut_config(self._get_case_attr(case, "Test App")),
                    "name": self._get_case_attr(case, "summary"),
                    "reset": self._get_case_attr(case, "reset") ,
                })
        return case_data

    def output(self):
        """
        output data for job configs

        :return: {"Filter": case filter, "CaseConfig": list of case configs for cases in this group}
        """
        output_data = {
            # we don't need filter for test function, as UT uses a few test functions for all cases
            "CaseConfig": [
                {
                    "name": self.case_list[0]["cmd set"] if isinstance(self.case_list[0]["cmd set"], str) else self.case_list[0]["cmd set"][0],
                    "extra_data": self._create_extra_data(),
                }
            ]
        }
        return output_data


class UnitTestAssignTest(CIAssignTest.AssignTest):
    CI_TEST_JOB_PATTERN = re.compile(r"^UT_.+")

    def __init__(self, test_case_path, ci_config_file):
        CIAssignTest.AssignTest.__init__(self, test_case_path, ci_config_file, case_group=Group)

    @staticmethod
    def _search_cases(test_case_path, case_filter=None):
        """
        For unit test case, we don't search for test functions.
        The unit test cases is stored in a yaml file which is created in job build-idf-test.
        """

        with open(test_case_path, "r") as f:
            raw_data = yaml.load(f)
        test_cases = raw_data["test cases"]
        if case_filter:
            for key in case_filter:
                filtered_cases = []
                for case in test_cases:
                    try:
                        # bot converts string to lower case
                        if isinstance(case[key], str):
                            _value = case[key].lower()
                        else:
                            _value = case[key]
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

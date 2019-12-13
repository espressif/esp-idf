"""
Command line tool to assign unit tests to CI test jobs.
"""

import re
import argparse

import yaml

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader

from tiny_test_fw.Utility import CIAssignTest


class Group(CIAssignTest.Group):
    SORT_KEYS = ["test environment", "tags"]
    MAX_CASE = 50
    ATTR_CONVERT_TABLE = {
        "execution_time": "execution time"
    }
    # when IDF support multiple chips, SDK will be moved into tags, we can remove it
    CI_JOB_MATCH_KEYS = ["test environment", "SDK"]

    def __init__(self, case):
        super(Group, self).__init__(case)
        for tag in self._get_case_attr(case, "tags"):
            self.ci_job_match_keys.add(tag)

    @staticmethod
    def _get_case_attr(case, attr):
        if attr in Group.ATTR_CONVERT_TABLE:
            attr = Group.ATTR_CONVERT_TABLE[attr]
        return case[attr]

    def add_extra_case(self, case):
        """ If current group contains all tags required by case, then add succeed """
        added = False
        if self.accept_new_case():
            for key in self.filters:
                if self._get_case_attr(case, key) != self.filters[key]:
                    if key == "tags":
                        if self._get_case_attr(case, key).issubset(self.filters[key]):
                            continue
                    break
            else:
                self.case_list.append(case)
                added = True
        return added

    def _create_extra_data(self, test_cases, test_function):
        """
        For unit test case, we need to copy some attributes of test cases into config file.
        So unit test function knows how to run the case.
        """
        case_data = []
        for case in test_cases:
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

    def _divide_case_by_test_function(self):
        """
        divide cases of current test group by test function they need to use

        :return: dict of list of cases for each test functions
        """
        case_by_test_function = {
            "run_multiple_devices_cases": [],
            "run_multiple_stage_cases": [],
            "run_unit_test_cases": [],
        }

        for case in self.case_list:
            if case["multi_device"] == "Yes":
                case_by_test_function["run_multiple_devices_cases"].append(case)
            elif case["multi_stage"] == "Yes":
                case_by_test_function["run_multiple_stage_cases"].append(case)
            else:
                case_by_test_function["run_unit_test_cases"].append(case)
        return case_by_test_function

    def output(self):
        """
        output data for job configs

        :return: {"Filter": case filter, "CaseConfig": list of case configs for cases in this group}
        """
        case_by_test_function = self._divide_case_by_test_function()

        output_data = {
            # we don't need filter for test function, as UT uses a few test functions for all cases
            "CaseConfig": [
                {
                    "name": test_function,
                    "extra_data": self._create_extra_data(test_cases, test_function),
                } for test_function, test_cases in case_by_test_function.iteritems() if test_cases
            ],
        }
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
            for case in test_cases:
                case["tags"] = set(case["tags"])
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
        # sort cases with configs and test functions
        # in later stage cases with similar attributes are more likely to be assigned to the same job
        # it will reduce the count of flash DUT operations
        test_cases.sort(key=lambda x: x["config"] + x["multi_stage"] + x["multi_device"])
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

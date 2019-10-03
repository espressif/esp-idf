from __future__ import print_function
import yaml
import os
import re
import shutil
import subprocess

from copy import deepcopy
import CreateSectionTable

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader

TEST_CASE_PATTERN = {
    "initial condition": "UTINIT1",
    "chip_target": "esp32",
    "level": "Unit",
    "execution time": 0,
    "auto test": "Yes",
    "category": "Function",
    "test point 1": "basic function",
    "version": "v1 (2016-12-06)",
    "test environment": "UT_T1_1",
    "reset": "",
    "expected result": "1. set succeed",
    "cmd set": "test_unit_test_case",
    "Test App": "UT",
}


class Parser(object):
    """ parse unit test cases from build files and create files for test bench """

    TAG_PATTERN = re.compile(r"([^=]+)(=)?(.+)?")
    DESCRIPTION_PATTERN = re.compile(r"\[([^]\[]+)\]")
    CONFIG_PATTERN = re.compile(r"{([^}]+)}")
    TEST_GROUPS_PATTERN = re.compile(r"TEST_GROUPS=(.*)$")

    # file path (relative to idf path)
    TAG_DEF_FILE = os.path.join("tools", "unit-test-app", "tools", "TagDefinition.yml")
    MODULE_DEF_FILE = os.path.join("tools", "unit-test-app", "tools", "ModuleDefinition.yml")
    CONFIG_DEPENDENCY_FILE = os.path.join("tools", "unit-test-app", "tools", "ConfigDependency.yml")
    MODULE_ARTIFACT_FILE = os.path.join("components", "idf_test", "ModuleDefinition.yml")
    TEST_CASE_FILE = os.path.join("components", "idf_test", "unit_test", "TestCaseAll.yml")
    UT_BIN_FOLDER = os.path.join("tools", "unit-test-app", "output")
    UT_CONFIG_FOLDER = os.path.join("tools", "unit-test-app", "configs")
    ELF_FILE = "unit-test-app.elf"
    SDKCONFIG_FILE = "sdkconfig"
    STRIP_CONFIG_PATTERN = re.compile(r"(.+?)(_\d+)?$")

    def __init__(self, idf_path=os.getenv("IDF_PATH")):
        self.test_env_tags = {}
        self.unit_jobs = {}
        self.file_name_cache = {}
        self.idf_path = idf_path
        self.tag_def = yaml.load(open(os.path.join(idf_path, self.TAG_DEF_FILE), "r"), Loader=Loader)
        self.module_map = yaml.load(open(os.path.join(idf_path, self.MODULE_DEF_FILE), "r"), Loader=Loader)
        self.config_dependencies = yaml.load(open(os.path.join(idf_path, self.CONFIG_DEPENDENCY_FILE), "r"),
                                             Loader=Loader)
        # used to check if duplicated test case names
        self.test_case_names = set()
        self.parsing_errors = []

    def parse_test_cases_for_one_config(self, configs_folder, config_output_folder, config_name):
        """
        parse test cases from elf and save test cases need to be executed to unit test folder
        :param configs_folder: folder where per-config sdkconfig framents are located (i.e. tools/unit-test-app/configs)
        :param config_output_folder: build folder of this config
        :param config_name: built unit test config name
        """
        test_groups = self.get_test_groups(os.path.join(configs_folder, config_name))

        elf_file = os.path.join(config_output_folder, self.ELF_FILE)
        subprocess.check_output('xtensa-esp32-elf-objdump -t {} | grep test_desc > case_address.tmp'.format(elf_file),
                                shell=True)
        subprocess.check_output('xtensa-esp32-elf-objdump -s {} > section_table.tmp'.format(elf_file), shell=True)

        table = CreateSectionTable.SectionTable("section_table.tmp")
        tags = self.parse_tags(os.path.join(config_output_folder, self.SDKCONFIG_FILE))
        print("Tags of config %s: %s" % (config_name, tags))
        test_cases = []

        # we could split cases of same config into multiple binaries as we have limited rom space
        # we should regard those configs like `default` and `default_2` as the same config
        match = self.STRIP_CONFIG_PATTERN.match(config_name)
        stripped_config_name = match.group(1)

        with open("case_address.tmp", "rb") as f:
            for line in f:
                # process symbol table like: "3ffb4310 l     O .dram0.data	00000018 test_desc_33$5010"
                line = line.split()
                test_addr = int(line[0], 16)
                section = line[3]

                name_addr = table.get_unsigned_int(section, test_addr, 4)
                desc_addr = table.get_unsigned_int(section, test_addr + 4, 4)
                file_name_addr = table.get_unsigned_int(section, test_addr + 12, 4)
                function_count = table.get_unsigned_int(section, test_addr + 20, 4)
                name = table.get_string("any", name_addr)
                desc = table.get_string("any", desc_addr)
                file_name = table.get_string("any", file_name_addr)

                # Search in tags to set the target
                target_tag_dict = {"ESP32_IDF": "esp32", "ESP32S2BETA_IDF": "esp32s2beta"}
                for tag in target_tag_dict:
                    if tag in tags:
                        target = target_tag_dict[tag]
                        break
                else:
                    target = "esp32"

                tc = self.parse_one_test_case(name, desc, file_name, config_name, stripped_config_name, tags, target)

                # check if duplicated case names
                # we need to use it to select case,
                # if duplicated IDs, Unity could select incorrect case to run
                # and we need to check all cases no matter if it's going te be executed by CI
                # also add app_name here, we allow same case for different apps
                if (tc["summary"] + stripped_config_name) in self.test_case_names:
                    self.parsing_errors.append("duplicated test case ID: " + tc["summary"])
                else:
                    self.test_case_names.add(tc["summary"] + stripped_config_name)

                test_group_included = True
                if test_groups is not None and tc["group"] not in test_groups:
                    test_group_included = False

                if tc["CI ready"] == "Yes" and test_group_included:
                    # update test env list and the cases of same env list
                    if tc["test environment"] in self.test_env_tags:
                        self.test_env_tags[tc["test environment"]].append(tc["ID"])
                    else:
                        self.test_env_tags.update({tc["test environment"]: [tc["ID"]]})

                    if function_count > 1:
                        tc.update({"child case num": function_count})

                    # only add  cases need to be executed
                    test_cases.append(tc)

        os.remove("section_table.tmp")
        os.remove("case_address.tmp")

        return test_cases

    def parse_case_properities(self, tags_raw):
        """
        parse test case tags (properities) with the following rules:
            * first tag is always group of test cases, it's mandatory
            * the rest tags should be [type=value].
                * if the type have default value, then [type] equal to [type=default_value].
                * if the type don't don't exist, then equal to [type=omitted_value]
            default_value and omitted_value are defined in TagDefinition.yml
        :param tags_raw: raw tag string
        :return: tag dict
        """
        tags = self.DESCRIPTION_PATTERN.findall(tags_raw)
        assert len(tags) > 0
        p = dict([(k, self.tag_def[k]["omitted"]) for k in self.tag_def])
        p["module"] = tags[0]

        # Use the original value of the first tag as test group name
        p["group"] = p["module"]

        if p["module"] not in self.module_map:
            p["module"] = "misc"

        # parsing rest tags, [type=value], =value is optional
        for tag in tags[1:]:
            match = self.TAG_PATTERN.search(tag)
            assert match is not None
            tag_type = match.group(1)
            tag_value = match.group(3)
            if match.group(2) == "=" and tag_value is None:
                # [tag_type=] means tag_value is empty string
                tag_value = ""
            if tag_type in p:
                if tag_value is None:
                    p[tag_type] = self.tag_def[tag_type]["default"]
                else:
                    p[tag_type] = tag_value
            else:
                # ignore not defined tag type
                pass
        return p

    @staticmethod
    def parse_tags_internal(sdkconfig, config_dependencies, config_pattern):
        required_tags = []

        def compare_config(config):
            return config in sdkconfig

        def process_condition(condition):
            matches = config_pattern.findall(condition)
            if matches:
                for config in matches:
                    compare_result = compare_config(config)
                    # replace all configs in condition with True or False according to compare result
                    condition = re.sub(config_pattern, str(compare_result), condition, count=1)
                # Now the condition is a python condition, we can use eval to compute its value
                ret = eval(condition)
            else:
                # didn't use complex condition. only defined one condition for the tag
                ret = compare_config(condition)
            return ret

        for tag in config_dependencies:
            if process_condition(config_dependencies[tag]):
                required_tags.append(tag)

        return required_tags

    def parse_tags(self, sdkconfig_file):
        """
        Some test configs could requires different DUTs.
        For example, if CONFIG_ESP32_SPIRAM_SUPPORT is enabled, we need WROVER-Kit to run test.
        This method will get tags for runners according to ConfigDependency.yml(maps tags to sdkconfig).

        We support to the following syntax::

            # define the config which requires the tag
            'tag_a': 'config_a="value_a"'
            # define the condition for the tag
            'tag_b': '{config A} and (not {config B} or (not {config C} and {config D}))'

        :param sdkconfig_file: sdk config file of the unit test config
        :return: required tags for runners
        """

        with open(sdkconfig_file, "r") as f:
            configs_raw_data = f.read()

        configs = configs_raw_data.splitlines(False)

        return self.parse_tags_internal(configs, self.config_dependencies, self.CONFIG_PATTERN)

    def get_test_groups(self, config_file):
        """
        If the config file includes TEST_GROUPS variable, return its value as a list of strings.
        :param config_file file under configs/ directory for given configuration
        :return: list of test groups, or None if TEST_GROUPS wasn't set
        """
        with open(config_file, "r") as f:
            for line in f:
                match = self.TEST_GROUPS_PATTERN.match(line)
                if match is not None:
                    return match.group(1).split(' ')
        return None

    def parse_one_test_case(self, name, description, file_name, config_name, stripped_config_name, tags, target):
        """
        parse one test case
        :param name: test case name (summary)
        :param description: test case description (tag string)
        :param file_name: the file defines this test case
        :param config_name: built unit test app name
        :param stripped_config_name: strip suffix from config name because they're the same except test components
        :param tags: tags to select runners
        :return: parsed test case
        """
        prop = self.parse_case_properities(description)

        test_case = deepcopy(TEST_CASE_PATTERN)
        test_case.update({"config": config_name,
                          "module": self.module_map[prop["module"]]['module'],
                          "group": prop["group"],
                          "CI ready": "No" if prop["ignore"] == "Yes" else "Yes",
                          "ID": "[{}] {}".format(stripped_config_name, name),
                          "test point 2": prop["module"],
                          "steps": name,
                          "test environment": prop["test_env"],
                          "reset": prop["reset"],
                          "sub module": self.module_map[prop["module"]]['sub module'],
                          "summary": name,
                          "multi_device": prop["multi_device"],
                          "multi_stage": prop["multi_stage"],
                          "timeout": int(prop["timeout"]),
                          "tags": tags,
                          "chip_target": target})
        return test_case

    def dump_test_cases(self, test_cases):
        """
        dump parsed test cases to YAML file for test bench input
        :param test_cases: parsed test cases
        """
        filename = os.path.join(self.idf_path, self.TEST_CASE_FILE)
        try:
            os.mkdir(os.path.dirname(filename))
        except OSError:
            pass
        with open(os.path.join(filename), "w+") as f:
            yaml.dump({"test cases": test_cases}, f, allow_unicode=True, default_flow_style=False)

    def copy_module_def_file(self):
        """ copy module def file to artifact path """
        src = os.path.join(self.idf_path, self.MODULE_DEF_FILE)
        dst = os.path.join(self.idf_path, self.MODULE_ARTIFACT_FILE)
        shutil.copy(src, dst)

    def parse_test_cases(self):
        """ parse test cases from multiple built unit test apps """
        test_cases = []

        output_folder = os.path.join(self.idf_path, self.UT_BIN_FOLDER)
        configs_folder = os.path.join(self.idf_path, self.UT_CONFIG_FOLDER)
        test_configs = os.listdir(output_folder)
        for config in test_configs:
            config_output_folder = os.path.join(output_folder, config)
            if os.path.exists(config_output_folder):
                test_cases.extend(self.parse_test_cases_for_one_config(configs_folder, config_output_folder, config))
        test_cases.sort(key=lambda x: x["config"] + x["summary"])
        self.dump_test_cases(test_cases)


def test_parser():
    parser = Parser()
    # test parsing tags
    # parsing module only and module in module list
    prop = parser.parse_case_properities("[esp32]")
    assert prop["module"] == "esp32"
    # module not in module list
    prop = parser.parse_case_properities("[not_in_list]")
    assert prop["module"] == "misc"
    # parsing a default tag, a tag with assigned value
    prop = parser.parse_case_properities("[esp32][ignore][test_env=ABCD][not_support1][not_support2=ABCD]")
    assert prop["ignore"] == "Yes" and prop["test_env"] == "ABCD" \
        and "not_support1" not in prop and "not_supported2" not in prop
    # parsing omitted value
    prop = parser.parse_case_properities("[esp32]")
    assert prop["ignore"] == "No" and prop["test_env"] == "UT_T1_1"
    # parsing with incorrect format
    try:
        parser.parse_case_properities("abcd")
        assert False
    except AssertionError:
        pass
    # skip invalid data parse, [type=] assigns empty string to type
    prop = parser.parse_case_properities("[esp32]abdc aaaa [ignore=]")
    assert prop["module"] == "esp32" and prop["ignore"] == ""
    # skip mis-paired []
    prop = parser.parse_case_properities("[esp32][[ignore=b]][]][test_env=AAA]]")
    assert prop["module"] == "esp32" and prop["ignore"] == "b" and prop["test_env"] == "AAA"

    config_dependency = {
        'a': '123',
        'b': '456',
        'c': 'not {123}',
        'd': '{123} and not {456}',
        'e': '{123} and not {789}',
        'f': '({123} and {456}) or ({123} and {789})'
    }
    sdkconfig = ["123", "789"]
    tags = parser.parse_tags_internal(sdkconfig, config_dependency, parser.CONFIG_PATTERN)
    assert sorted(tags) == ['a', 'd', 'f']  # sorted is required for older Python3, e.g. 3.4.8


def main():
    test_parser()

    idf_path = os.getenv("IDF_PATH")

    parser = Parser(idf_path)
    parser.parse_test_cases()
    parser.copy_module_def_file()
    if len(parser.parsing_errors) > 0:
        for error in parser.parsing_errors:
            print(error)
        exit(-1)


if __name__ == '__main__':
    main()

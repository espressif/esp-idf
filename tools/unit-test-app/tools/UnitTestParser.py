from __future__ import print_function

import argparse
import os
import re
import shutil
import sys
from copy import deepcopy

import yaml

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader  # type: ignore

try:
    from ElfUnitTestParser import parse_elf_test_cases
except ImportError:
    sys.path.append(os.path.expandvars(os.path.join('$IDF_PATH', 'tools', 'unit-test-app', 'tools')))
    from ElfUnitTestParser import parse_elf_test_cases


TEST_CASE_PATTERN = {
    'initial condition': 'UTINIT1',
    'chip_target': 'esp32',
    'level': 'Unit',
    'execution time': 0,
    'auto test': 'Yes',
    'category': 'Function',
    'test point 1': 'basic function',
    'version': 'v1 (2016-12-06)',
    'test environment': 'UT_T1_1',
    'reset': '',
    'expected result': '1. set succeed',
    'cmd set': 'test_unit_test_case',
    'Test App': 'UT',
}


class Parser(object):
    """ parse unit test cases from build files and create files for test bench """

    TAG_PATTERN = re.compile(r'([^=]+)(=)?(.+)?')
    DESCRIPTION_PATTERN = re.compile(r'\[([^]\[]+)\]')
    CONFIG_PATTERN = re.compile(r'{([^}]+)}')
    TEST_GROUPS_PATTERN = re.compile(r'TEST_GROUPS=(.*)$')

    # file path (relative to idf path)
    TAG_DEF_FILE = os.path.join('tools', 'unit-test-app', 'tools', 'TagDefinition.yml')
    MODULE_DEF_FILE = os.path.join('tools', 'unit-test-app', 'tools', 'ModuleDefinition.yml')
    CONFIG_DEPENDENCY_FILE = os.path.join('tools', 'unit-test-app', 'tools', 'ConfigDependency.yml')
    MODULE_ARTIFACT_FILE = os.path.join('components', 'idf_test', 'ModuleDefinition.yml')
    TEST_CASE_FILE_DIR = os.path.join('components', 'idf_test', 'unit_test')
    UT_CONFIG_FOLDER = os.path.join('tools', 'unit-test-app', 'configs')
    ELF_FILE = 'unit-test-app.elf'
    SDKCONFIG_FILE = 'sdkconfig'
    STRIP_CONFIG_PATTERN = re.compile(r'(.+?)(_\d+)?$')

    def __init__(self, binary_folder, node_index):
        idf_path = os.getenv('IDF_PATH')
        idf_target = os.getenv('IDF_TARGET')
        self.test_env_tags = {}
        self.unit_jobs = {}
        self.file_name_cache = {}
        self.idf_path = idf_path
        self.idf_target = idf_target
        self.node_index = node_index
        self.ut_bin_folder = binary_folder
        self.tag_def = yaml.load(open(os.path.join(idf_path, self.TAG_DEF_FILE), 'r'), Loader=Loader)
        self.module_map = yaml.load(open(os.path.join(idf_path, self.MODULE_DEF_FILE), 'r'), Loader=Loader)
        self.config_dependencies = yaml.load(open(os.path.join(idf_path, self.CONFIG_DEPENDENCY_FILE), 'r'),
                                             Loader=Loader)
        # used to check if duplicated test case names
        self.test_case_names = set()
        self.parsing_errors = []

    def parse_test_cases_for_one_config(self, configs_folder, config_output_folder, config_name):
        """
        parse test cases from elf and save test cases need to be executed to unit test folder
        :param configs_folder: folder where per-config sdkconfig fragments are located (i.e. tools/unit-test-app/configs)
        :param config_output_folder: build folder of this config
        :param config_name: built unit test config name
        """
        tags = self.parse_tags(os.path.join(config_output_folder, self.SDKCONFIG_FILE))
        print('Tags of config %s: %s' % (config_name, tags))

        test_groups = self.get_test_groups(os.path.join(configs_folder, config_name))

        elf_file = os.path.join(config_output_folder, self.ELF_FILE)
        bin_test_cases = parse_elf_test_cases(elf_file, self.idf_target)

        test_cases = []
        for bin_tc in bin_test_cases:
            # we could split cases of same config into multiple binaries as we have limited rom space
            # we should regard those configs like `default` and `default_2` as the same config
            match = self.STRIP_CONFIG_PATTERN.match(config_name)
            stripped_config_name = match.group(1)

            tc = self.parse_one_test_case(bin_tc['name'], bin_tc['desc'], config_name, stripped_config_name, tags)

            # check if duplicated case names
            # we need to use it to select case,
            # if duplicated IDs, Unity could select incorrect case to run
            # and we need to check all cases no matter if it's going te be executed by CI
            # also add app_name here, we allow same case for different apps
            if (tc['summary'] + stripped_config_name) in self.test_case_names:
                self.parsing_errors.append('{} ({}): duplicated test case ID: {}'.format(stripped_config_name, config_name, tc['summary']))
            else:
                self.test_case_names.add(tc['summary'] + stripped_config_name)

            test_group_included = True
            if test_groups is not None and tc['group'] not in test_groups:
                test_group_included = False

            if tc['CI ready'] == 'Yes' and test_group_included:
                # update test env list and the cases of same env list
                if tc['test environment'] in self.test_env_tags:
                    self.test_env_tags[tc['test environment']].append(tc['ID'])
                else:
                    self.test_env_tags.update({tc['test environment']: [tc['ID']]})

                if bin_tc['function_count'] > 1:
                    tc.update({'child case num': bin_tc['function_count']})

                # only add  cases need to be executed
                test_cases.append(tc)

        return test_cases

    def parse_case_properties(self, tags_raw):
        """
        parse test case tags (properties) with the following rules:
            * first tag is always group of test cases, it's mandatory
            * the rest tags should be [type=value].
                * if the type have default value, then [type] equal to [type=default_value].
                * if the type don't exist, then equal to [type=omitted_value]
            default_value and omitted_value are defined in TagDefinition.yml
        :param tags_raw: raw tag string
        :return: tag dict
        """
        tags = self.DESCRIPTION_PATTERN.findall(tags_raw)
        assert len(tags) > 0
        p = dict([(k, self.tag_def[k]['omitted']) for k in self.tag_def])
        p['module'] = tags[0]

        # Use the original value of the first tag as test group name
        p['group'] = p['module']

        if p['module'] not in self.module_map:
            p['module'] = 'misc'

        # parsing rest tags, [type=value], =value is optional
        for tag in tags[1:]:
            match = self.TAG_PATTERN.search(tag)
            assert match is not None
            tag_type = match.group(1)
            tag_value = match.group(3)
            if match.group(2) == '=' and tag_value is None:
                # [tag_type=] means tag_value is empty string
                tag_value = ''
            if tag_type in p:
                if tag_value is None:
                    p[tag_type] = self.tag_def[tag_type]['default']
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
        For example, if CONFIG_SPIRAM is enabled, we need kit with PSRAM to run test.
        This method will get tags for runners according to ConfigDependency.yml(maps tags to sdkconfig).

        We support to the following syntax::

            # define the config which requires the tag
            'tag_a': 'config_a="value_a"'
            # define the condition for the tag
            'tag_b': '{config A} and (not {config B} or (not {config C} and {config D}))'

        :param sdkconfig_file: sdk config file of the unit test config
        :return: required tags for runners
        """
        with open(sdkconfig_file, 'r') as f:
            configs_raw_data = f.read()

        configs = configs_raw_data.splitlines(False)

        return self.parse_tags_internal(configs, self.config_dependencies, self.CONFIG_PATTERN)

    def get_test_groups(self, config_file):
        """
        If the config file includes TEST_GROUPS variable, return its value as a list of strings.
        :param config_file file under configs/ directory for given configuration
        :return: list of test groups, or None if TEST_GROUPS wasn't set
        """
        with open(config_file, 'r') as f:
            for line in f:
                match = self.TEST_GROUPS_PATTERN.match(line)
                if match is not None:
                    return match.group(1).split(' ')
        return None

    def parse_one_test_case(self, name, description, config_name, stripped_config_name, tags):
        """
        parse one test case
        :param name: test case name (summary)
        :param description: test case description (tag string)
        :param config_name: built unit test app name
        :param stripped_config_name: strip suffix from config name because they're the same except test components
        :param tags: tags to select runners
        :return: parsed test case
        """
        prop = self.parse_case_properties(description)

        test_case = deepcopy(TEST_CASE_PATTERN)
        test_case.update({'config': config_name,
                          'module': self.module_map[prop['module']]['module'],
                          'group': prop['group'],
                          'CI ready': 'No' if prop['ignore'] == 'Yes' else 'Yes',
                          'ID': '[{}] {}'.format(stripped_config_name, name),
                          'test point 2': prop['module'],
                          'steps': name,
                          'test environment': prop['test_env'],
                          'reset': prop['reset'],
                          'sub module': self.module_map[prop['module']]['sub module'],
                          'summary': name,
                          'multi_device': prop['multi_device'],
                          'multi_stage': prop['multi_stage'],
                          'timeout': int(prop['timeout']),
                          'tags': tags,
                          'chip_target': self.idf_target})
        return test_case

    def dump_test_cases(self, test_cases):
        """
        dump parsed test cases to YAML file for test bench input
        :param test_cases: parsed test cases
        """
        filename = os.path.join(self.idf_path, self.TEST_CASE_FILE_DIR,
                                '{}_{}.yml'.format(self.idf_target, self.node_index))
        try:
            os.mkdir(os.path.dirname(filename))
        except OSError:
            pass
        with open(os.path.join(filename), 'w+') as f:
            yaml.dump({'test cases': test_cases}, f, allow_unicode=True, default_flow_style=False)

    def copy_module_def_file(self):
        """ copy module def file to artifact path """
        src = os.path.join(self.idf_path, self.MODULE_DEF_FILE)
        dst = os.path.join(self.idf_path, self.MODULE_ARTIFACT_FILE)
        shutil.copy(src, dst)

    def parse_test_cases(self):
        """ parse test cases from multiple built unit test apps """
        test_cases = []

        configs_folder = os.path.join(self.idf_path, self.UT_CONFIG_FOLDER)
        config_output_prefix = f'build_{self.idf_target}_'
        test_configs = []
        for item in os.listdir(self.ut_bin_folder):
            if os.path.isdir(os.path.join(self.ut_bin_folder, item)) and item.startswith(config_output_prefix):
                test_configs.append(item.split(config_output_prefix)[1])

        for config in test_configs:
            config_output_folder = os.path.join(self.ut_bin_folder, f'{config_output_prefix}{config}')
            if os.path.exists(config_output_folder):
                test_cases.extend(self.parse_test_cases_for_one_config(configs_folder, config_output_folder, config))
        test_cases.sort(key=lambda x: x['config'] + x['summary'])
        self.dump_test_cases(test_cases)


def test_parser(binary_folder, node_index):
    ut_parser = Parser(binary_folder, node_index)
    # test parsing tags
    # parsing module only and module in module list
    prop = ut_parser.parse_case_properties('[esp32]')
    assert prop['module'] == 'esp32'
    # module not in module list
    prop = ut_parser.parse_case_properties('[not_in_list]')
    assert prop['module'] == 'misc'
    # parsing a default tag, a tag with assigned value
    prop = ut_parser.parse_case_properties('[esp32][ignore][test_env=ABCD][not_support1][not_support2=ABCD]')
    assert prop['ignore'] == 'Yes' and prop['test_env'] == 'ABCD' \
           and 'not_support1' not in prop and 'not_supported2' not in prop
    # parsing omitted value
    prop = ut_parser.parse_case_properties('[esp32]')
    assert prop['ignore'] == 'No' and prop['test_env'] == 'UT_T1_1'
    # parsing with incorrect format
    try:
        ut_parser.parse_case_properties('abcd')
        assert False
    except AssertionError:
        pass
    # skip invalid data parse, [type=] assigns empty string to type
    prop = ut_parser.parse_case_properties('[esp32]abdc aaaa [ignore=]')
    assert prop['module'] == 'esp32' and prop['ignore'] == ''
    # skip mis-paired []
    prop = ut_parser.parse_case_properties('[esp32][[ignore=b]][]][test_env=AAA]]')
    assert prop['module'] == 'esp32' and prop['ignore'] == 'b' and prop['test_env'] == 'AAA'

    config_dependency = {
        'a': '123',
        'b': '456',
        'c': 'not {123}',
        'd': '{123} and not {456}',
        'e': '{123} and not {789}',
        'f': '({123} and {456}) or ({123} and {789})'
    }
    sdkconfig = ['123', '789']
    tags = ut_parser.parse_tags_internal(sdkconfig, config_dependency, ut_parser.CONFIG_PATTERN)
    assert sorted(tags) == ['a', 'd', 'f']  # sorted is required for older Python3, e.g. 3.4.8


def main(binary_folder, node_index):
    assert os.getenv('IDF_PATH'), 'IDF_PATH must be set to use this script'
    assert os.getenv('IDF_TARGET'), 'IDF_TARGET must be set to use this script'
    test_parser(binary_folder, node_index)

    ut_parser = Parser(binary_folder, node_index)
    ut_parser.parse_test_cases()
    ut_parser.copy_module_def_file()
    if len(ut_parser.parsing_errors) > 0:
        for error in ut_parser.parsing_errors:
            print(error)
        exit(1)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('bin_dir', help='Binary Folder')
    parser.add_argument('node_index', type=int, default=1,
                        help='Node index, should only be set in CI')
    args = parser.parse_args()
    main(args.bin_dir, args.node_index)

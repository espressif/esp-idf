#!/usr/bin/env python
# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""
Check patterns-build_components including all components files except 'test*/'
"""

import glob
import os
from typing import List, Set

import yaml
from idf_ci_utils import IDF_PATH

RULES_FILE = os.path.join(IDF_PATH, '.gitlab', 'ci', 'rules.yml')


def get_component_files_from_patterns(patterns: List[str]) -> Set[str]:
    files: Set[str] = set()
    for pattern in patterns:
        # NOTE: version 3.10: Added the root_dir and dir_fd parameters.
        files = files | set(glob.iglob(pattern, recursive=True))
    return files


def get_components_submodule_files(submodules: List[str]) -> Set[str]:
    patterns = [f'{submodule}/**/*' for submodule in submodules]
    return get_component_files_from_patterns(patterns)


def get_components_test_files() -> Set[str]:
    patterns = [
        'components/*/test/**/*',
        'components/*/test_apps/**/*',
        'components/*/host_test/**/*',
        # other test files
        'components/efuse/test_efuse_host/**/*',
        'components/esp_coex/test_md5/**/*',
        'components/esp_gdbstub/test_gdbstub_host/**/*',
        'components/esp_system/test_eh_frame_parser/**/*',
        'components/esp_wifi/test_md5/**/*',
        'components/fatfs/test_fatfsgen/**/*',
        'components/heap/test_multi_heap_host/**/*',
        'components/nvs_flash/test_nvs_host/**/*',
        'components/partition_table/test_gen_esp32part_host/**/*',
        'components/spiffs/test_spiffsgen/**/*',
    ]
    return get_component_files_from_patterns(patterns)


def get_all_components_files() -> Set[str]:
    return get_component_files_from_patterns(['components/**/*'])


def main() -> None:
    with open(RULES_FILE, 'r') as f:
        rules = yaml.load(f.read(), Loader=yaml.FullLoader)
        patterns_components = [p for p in rules['.patterns-build_components'] if p.startswith('components/')]
        submodules = [p for p in rules['.patterns-submodule'] if p.startswith('components/')]
        # resolve components in patterns_components
        patterns_components = [f'{p}/**/*' if p in submodules else p for p in patterns_components]

    # get different components files
    submodule_files = get_components_submodule_files(submodules)
    rules_files = get_component_files_from_patterns(patterns_components)

    # first check submodules in rules_files
    if check_submodule_res := submodule_files.difference(rules_files):
        # only show one file in the failure message
        print(f'Found submodule should in .patterns-build_components but not: {check_submodule_res.pop()}')
        exit(1)

    rules_files = rules_files.difference(submodule_files)
    components_test_files = get_components_test_files().difference(submodule_files)
    components_build_files = get_all_components_files().difference(submodule_files).difference(components_test_files)

    if check_test_files := components_test_files & rules_files:
        # rules should not include test files
        print(f'Found test files should not in .patterns-build_components: {check_test_files.pop()}')
        exit(1)
    if check_difference := components_build_files.difference(rules_files):
        # rules should include all build files
        print(f'Found components files should in .patterns-build_components but not: {check_difference.pop()}')
        exit(1)
    if check_difference := rules_files.difference(components_build_files):
        print(f'Found components files should not in .patterns-build_components: {check_difference.pop()}')
        exit(1)


if __name__ == '__main__':
    main()

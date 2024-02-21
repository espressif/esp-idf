# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os.path
from pathlib import Path

from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


def test_dependency_lock(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    replace_in_file(
        test_app_copy / 'CMakeLists.txt',
        search='# placeholder_after_include_project_cmake',
        replace='idf_build_set_property(DEPENDENCIES_LOCK dependencies.lock.${IDF_TARGET})',
    )

    idf_py('add-dependency', 'example/cmp')
    idf_py('fullclean')
    idf_py('reconfigure')
    assert os.path.isfile(test_app_copy / 'dependencies.lock.esp32')
    assert not os.path.isfile(test_app_copy / 'dependencies.lock')


def test_trimmed_components_still_passed_to_cmake(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    replace_in_file(
        test_app_copy / 'CMakeLists.txt',
        search='# placeholder_after_include_project_cmake',
        replace='set(COMPONENTS main)',
    )

    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        search='# placeholder_inside_idf_component_register',
        replace='REQUIRES foo',
    )

    os.makedirs(test_app_copy / 'components')
    idf_py('create-component', '-C', 'components', 'foo')
    idf_py('add-dependency', '--component', 'foo', 'example/cmp')

    idf_py('reconfigure')

    with open('dependencies.lock', 'r') as f:
        fs = f.read()

    assert '  example/cmp:' in fs

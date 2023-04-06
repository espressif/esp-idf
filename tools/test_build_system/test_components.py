# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import shutil
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc, replace_in_file


def test_component_extra_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Setting EXTRA_COMPONENT_DIRS works')
    shutil.move(test_app_copy / 'main', test_app_copy / 'different_main' / 'main')
    replace_in_file((test_app_copy / 'CMakeLists.txt'), '# placeholder_before_include_project_cmake',
                    'set(EXTRA_COMPONENT_DIRS {})'.format(Path('different_main', 'main')))
    ret = idf_py('reconfigure')
    assert str(test_app_copy / 'different_main' / 'main') in ret.stdout
    assert str(test_app_copy / 'main') not in ret.stdout


@pytest.mark.usefixtures('test_app_copy')
def test_component_nonexistent_extra_dirs_not_allowed(idf_py: IdfPyFunc) -> None:
    ret = idf_py('reconfigure', '-DEXTRA_COMPONENT_DIRS="nonexistent_dir"', check=False)
    assert ret.returncode != 0


def test_component_names_contain_spaces(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Component names may contain spaces')
    (test_app_copy / 'extra component').mkdir()
    (test_app_copy / 'extra component' / 'CMakeLists.txt').write_text('idf_component_register')
    idf_py('-DEXTRA_COMPONENT_DIRS="extra component;main"')

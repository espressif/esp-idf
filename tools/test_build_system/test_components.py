# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
import logging
import shutil
import sys
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict, IdfPyFunc, append_to_file, replace_in_file


@pytest.mark.skipif(sys.platform == 'win32', reason='Failing on Windows runner. TODO')
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


def test_component_can_not_be_empty_dir(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Empty directory not treated as a component')
    empty_component_dir = (test_app_copy / 'components' / 'esp32')
    empty_component_dir.mkdir(parents=True)
    idf_py('reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json', 'r'))
    assert str(empty_component_dir) not in data.get('build_component_paths')


@pytest.mark.skipif(sys.platform == 'win32', reason='Failing on Windows runner. TODO')
def test_component_subdirs_not_added_to_component_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('If a component directory is added to COMPONENT_DIRS, its subdirectories are not added')
    (test_app_copy / 'main' / 'test').mkdir(parents=True)
    (test_app_copy / 'main' / 'test' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py('reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json', 'r'))
    assert str(test_app_copy / 'main' / 'test') not in data.get('build_component_paths')
    assert str(test_app_copy / 'main') in data.get('build_component_paths')


@pytest.mark.skipif(sys.platform == 'win32', reason='Failing on Windows runner. TODO')
def test_component_sibling_dirs_not_added_to_component_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('If a component directory is added to COMPONENT_DIRS, its sibling directories are not added')
    mycomponents_subdir = (test_app_copy / 'mycomponents')
    (mycomponents_subdir / 'mycomponent').mkdir(parents=True)
    (mycomponents_subdir / 'mycomponent' / 'CMakeLists.txt').write_text('idf_component_register()')

    # first test by adding single component directory to EXTRA_COMPONENT_DIRS
    (mycomponents_subdir / 'esp32').mkdir(parents=True)
    (mycomponents_subdir / 'esp32' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py('-DEXTRA_COMPONENT_DIRS={}'.format(str(mycomponents_subdir / 'mycomponent')), 'reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json', 'r'))
    assert str(mycomponents_subdir / 'esp32') not in data.get('build_component_paths')
    assert str(mycomponents_subdir / 'mycomponent') in data.get('build_component_paths')
    shutil.rmtree(mycomponents_subdir / 'esp32')

    # now the same thing, but add a components directory
    (test_app_copy / 'esp32').mkdir()
    (test_app_copy / 'esp32' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py('-DEXTRA_COMPONENT_DIRS={}'.format(str(mycomponents_subdir)), 'reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json', 'r'))
    assert str(test_app_copy / 'esp32') not in data.get('build_component_paths')
    assert str(mycomponents_subdir / 'mycomponent') in data.get('build_component_paths')


@pytest.mark.skipif(sys.platform == 'win32', reason='Failing on Windows runner. TODO')
def test_component_properties_are_set(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Component properties are set')
    append_to_file(test_app_copy / 'CMakeLists.txt', '\n'.join(['',
                                                                'idf_component_get_property(srcs main SRCS)',
                                                                'message(STATUS SRCS:${srcs})']))
    ret = idf_py('reconfigure')
    assert 'SRCS:{}'.format(test_app_copy / 'main' / 'build_test_app.c') in ret.stdout, 'Component properties should be set'


@pytest.mark.skipif(sys.platform == 'win32', reason='Failing on Windows runner. TODO')
def test_component_overriden_dir(idf_py: IdfPyFunc, test_app_copy: Path, default_idf_env: EnvDict) -> None:
    logging.info('Getting component overriden dir')
    (test_app_copy / 'components' / 'hal').mkdir(parents=True)
    (test_app_copy / 'components' / 'hal' / 'CMakeLists.txt').write_text('\n'.join([
        'idf_component_get_property(overriden_dir ${COMPONENT_NAME} COMPONENT_OVERRIDEN_DIR)',
        'message(STATUS overriden_dir:${overriden_dir})']))
    ret = idf_py('reconfigure')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    # no registration, overrides registration as well
    assert 'overriden_dir:{}'.format(idf_path / 'components' / 'hal') in ret.stdout, 'Failed to get overriden dir'
    append_to_file((test_app_copy / 'components' / 'hal' / 'CMakeLists.txt'), '\n'.join([
        '',
        'idf_component_register(KCONFIG ${overriden_dir}/Kconfig)',
        'idf_component_get_property(kconfig ${COMPONENT_NAME} KCONFIG)',
        'message(STATUS kconfig:${overriden_dir}/Kconfig)']))
    ret = idf_py('reconfigure', check=False)
    assert 'kconfig:{}'.format(idf_path / 'components' / 'hal') in ret.stdout, 'Failed to verify original `main` directory'


@pytest.mark.skipif(sys.platform == 'win32', reason='Failing on Windows runner. TODO')
def test_components_prioritizer_over_extra_components_dir(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Project components prioritized over EXTRA_COMPONENT_DIRS')
    (test_app_copy / 'extra_dir' / 'my_component').mkdir(parents=True)
    (test_app_copy / 'extra_dir' / 'my_component' / 'CMakeLists.txt').write_text('idf_component_register()')
    replace_in_file(test_app_copy / 'CMakeLists.txt',
                    '# placeholder_before_include_project_cmake',
                    'set(EXTRA_COMPONENT_DIRS extra_dir)')
    ret = idf_py('reconfigure')
    assert str(test_app_copy / 'extra_dir' / 'my_component') in ret.stdout, 'Unable to find component specified in EXTRA_COMPONENT_DIRS'
    (test_app_copy / 'components' / 'my_component').mkdir(parents=True)
    (test_app_copy / 'components' / 'my_component' / 'CMakeLists.txt').write_text('idf_component_register()')
    ret = idf_py('reconfigure')
    assert str(test_app_copy / 'components' / 'my_component') in ret.stdout, 'Project components should be prioritized over EXTRA_COMPONENT_DIRS'


def test_exclude_components_not_passed(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Components in EXCLUDE_COMPONENTS not passed to idf_component_manager')
    idf_py('create-component', '-C', 'components', 'to_be_excluded')
    (test_app_copy / 'components' / 'to_be_excluded' / 'idf_component.yml').write_text('invalid syntax..')
    ret = idf_py('reconfigure', check=False)
    assert ret.returncode == 2, 'Reconfigure should have failed due to invalid syntax in idf_component.yml'
    idf_py('-DEXCLUDE_COMPONENTS=to_be_excluded', 'reconfigure')


def test_version_in_component_cmakelist(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Use IDF version variables in component CMakeLists.txt file')
    replace_in_file((test_app_copy / 'main' / 'CMakeLists.txt'), '# placeholder_before_idf_component_register',
                    '\n'.join(['if (NOT IDF_VERSION_MAJOR)', ' message(FATAL_ERROR "IDF version not set")', 'endif()']))
    idf_py('reconfigure')

# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import shutil
from pathlib import Path

import pytest
from test_build_system_helpers import append_to_file
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


def test_component_extra_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Setting EXTRA_COMPONENT_DIRS works')
    shutil.move(test_app_copy / 'main', test_app_copy / 'different_main' / 'main')
    replace_in_file((test_app_copy / 'CMakeLists.txt'), '# placeholder_before_include_project_cmake',
                    'set(EXTRA_COMPONENT_DIRS {})'.format(Path('different_main', 'main').as_posix()))
    ret = idf_py('reconfigure')
    assert str((test_app_copy / 'different_main' / 'main').as_posix()) in ret.stdout
    assert str((test_app_copy / 'main').as_posix()) not in ret.stdout


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


def test_component_subdirs_not_added_to_component_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('If a component directory is added to COMPONENT_DIRS, its subdirectories are not added')
    (test_app_copy / 'main' / 'test').mkdir(parents=True)
    (test_app_copy / 'main' / 'test' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py('reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json', 'r'))
    assert str((test_app_copy / 'main' / 'test').as_posix()) not in data.get('build_component_paths')
    assert str((test_app_copy / 'main').as_posix()) in data.get('build_component_paths')


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
    assert str((mycomponents_subdir / 'esp32').as_posix()) not in data.get('build_component_paths')
    assert str((mycomponents_subdir / 'mycomponent').as_posix()) in data.get('build_component_paths')
    shutil.rmtree(mycomponents_subdir / 'esp32')

    # now the same thing, but add a components directory
    (test_app_copy / 'esp32').mkdir()
    (test_app_copy / 'esp32' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py('-DEXTRA_COMPONENT_DIRS={}'.format(str(mycomponents_subdir)), 'reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json', 'r'))
    assert str((test_app_copy / 'esp32').as_posix()) not in data.get('build_component_paths')
    assert str((mycomponents_subdir / 'mycomponent').as_posix()) in data.get('build_component_paths')


def test_component_properties_are_set(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Component properties are set')
    append_to_file(test_app_copy / 'CMakeLists.txt', '\n'.join(['',
                                                                'idf_component_get_property(srcs main SRCS)',
                                                                'message(STATUS SRCS:${srcs})']))
    ret = idf_py('reconfigure')
    assert 'SRCS:{}'.format((test_app_copy / 'main' / 'build_test_app.c').as_posix()) in ret.stdout, 'Component properties should be set'


def test_get_property_for_unknown_component(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Getting property of unknown component fails gracefully')
    append_to_file(test_app_copy / 'CMakeLists.txt', '\n'.join(['',
                                                                'idf_component_get_property(VAR UNKNOWN PROP)']))
    ret = idf_py('reconfigure', check=False)
    assert "Failed to resolve component 'UNKNOWN'" in ret.stderr, ('idf_component_get_property '
                                                                   'for unknown component should fail gracefully')


def test_set_property_for_unknown_component(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Setting property of unknown component fails gracefully')
    append_to_file(test_app_copy / 'CMakeLists.txt', '\n'.join(['',
                                                                'idf_component_set_property(UNKNOWN PROP VAL)']))
    ret = idf_py('reconfigure', check=False)
    assert "Failed to resolve component 'UNKNOWN'" in ret.stderr, ('idf_component_set_property '
                                                                   'for unknown component should fail gracefully')


def test_component_overridden_dir(idf_py: IdfPyFunc, test_app_copy: Path, default_idf_env: EnvDict) -> None:
    logging.info('Getting component overridden dir')
    (test_app_copy / 'components' / 'hal').mkdir(parents=True)
    (test_app_copy / 'components' / 'hal' / 'CMakeLists.txt').write_text('\n'.join([
        'idf_component_get_property(overridden_dir ${COMPONENT_NAME} COMPONENT_OVERRIDEN_DIR)',
        'message(STATUS overridden_dir:${overridden_dir})', 'idf_component_register()']))
    ret = idf_py('reconfigure')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    # no registration, overrides registration as well
    assert 'overridden_dir:{}'.format((idf_path / 'components' / 'hal').as_posix()) in ret.stdout, 'Failed to get overridden dir'
    append_to_file((test_app_copy / 'components' / 'hal' / 'CMakeLists.txt'), '\n'.join([
        '',
        'idf_component_register(KCONFIG ${overridden_dir}/Kconfig)',
        'idf_component_get_property(kconfig ${COMPONENT_NAME} KCONFIG)',
        'message(STATUS kconfig:${overridden_dir}/Kconfig)']))
    ret = idf_py('reconfigure', check=False)
    assert 'kconfig:{}'.format((idf_path / 'components' / 'hal').as_posix()) in ret.stdout, 'Failed to verify original `main` directory'


def test_components_prioritizer_over_extra_components_dir(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Project components prioritized over EXTRA_COMPONENT_DIRS')
    (test_app_copy / 'extra_dir' / 'my_component').mkdir(parents=True)
    (test_app_copy / 'extra_dir' / 'my_component' / 'CMakeLists.txt').write_text('idf_component_register()')
    replace_in_file(test_app_copy / 'CMakeLists.txt',
                    '# placeholder_before_include_project_cmake',
                    'set(EXTRA_COMPONENT_DIRS extra_dir)')
    ret = idf_py('reconfigure')
    assert str((test_app_copy / 'extra_dir' / 'my_component').as_posix()) in ret.stdout, 'Unable to find component specified in EXTRA_COMPONENT_DIRS'
    (test_app_copy / 'components' / 'my_component').mkdir(parents=True)
    (test_app_copy / 'components' / 'my_component' / 'CMakeLists.txt').write_text('idf_component_register()')
    ret = idf_py('reconfigure')
    assert str((test_app_copy / 'components' / 'my_component').as_posix()) in ret.stdout, 'Project components should be prioritized over EXTRA_COMPONENT_DIRS'


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


def test_unknown_component_error(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('When unknown component name is specified, correct error is shown')
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        search='# placeholder_inside_idf_component_register',
        replace='REQUIRES unknown',
    )
    ret = idf_py('reconfigure', check=False)
    assert 'Failed to resolve component \'unknown\' required by component \'main\'' in ret.stderr

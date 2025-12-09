# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import os
import re
import shutil
from collections.abc import Generator
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import append_to_file
from test_build_system_helpers import replace_in_file


@pytest.fixture(autouse=True)
def create_idf_components(request: pytest.FixtureRequest) -> Generator:
    """
    Auto create/remove components under IDF_PATH.
    Use it by applying the marker to the test function:

    >>> @pytest.mark.with_idf_components(['cmp1', 'cmp2', ...])
    """
    mark = request.node.get_closest_marker('with_idf_components')
    if mark is None:
        yield
    else:
        idf_path = Path(os.environ['IDF_PATH'])
        created_paths = []
        for name in mark.args[0]:
            (idf_path / 'components' / name).mkdir(parents=True)
            (idf_path / 'components' / name / 'CMakeLists.txt').write_text('idf_component_register()')
            created_paths.append(str((idf_path / 'components' / name).as_posix()))

        yield

        for path in created_paths:
            shutil.rmtree(path)


def test_component_extra_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Setting EXTRA_COMPONENT_DIRS works')
    shutil.move(test_app_copy / 'main', test_app_copy / 'different_main' / 'main')
    replace_in_file(
        (test_app_copy / 'CMakeLists.txt'),
        '# placeholder_before_include_project_cmake',
        'set(EXTRA_COMPONENT_DIRS {})'.format(Path('different_main', 'main').as_posix()),
    )
    idf_py('reconfigure')

    # Check project_description.json for component paths
    data = json.load(open(test_app_copy / 'build' / 'project_description.json'))
    assert str((test_app_copy / 'different_main' / 'main').as_posix()) in data.get('build_component_paths')
    assert str((test_app_copy / 'main').as_posix()) not in data.get('build_component_paths')


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
    empty_component_dir = test_app_copy / 'components' / 'esp32'
    empty_component_dir.mkdir(parents=True)
    idf_py('reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json'))
    assert str(empty_component_dir) not in data.get('build_component_paths')


def test_component_subdirs_not_added_to_component_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('If a component directory is added to COMPONENT_DIRS, its subdirectories are not added')
    (test_app_copy / 'main' / 'test').mkdir(parents=True)
    (test_app_copy / 'main' / 'test' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py('reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json'))
    assert str((test_app_copy / 'main' / 'test').as_posix()) not in data.get('build_component_paths')
    assert str((test_app_copy / 'main').as_posix()) in data.get('build_component_paths')


def test_component_sibling_dirs_not_added_to_component_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('If a component directory is added to COMPONENT_DIRS, its sibling directories are not added')
    mycomponents_subdir = test_app_copy / 'mycomponents'
    (mycomponents_subdir / 'mycomponent').mkdir(parents=True)
    (mycomponents_subdir / 'mycomponent' / 'CMakeLists.txt').write_text('idf_component_register()')

    # Add PRIV_REQUIRES to main component for cmakev2 compatibility
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'PRIV_REQUIRES mycomponent',
    )

    # first test by adding single component directory to EXTRA_COMPONENT_DIRS
    (mycomponents_subdir / 'esp32').mkdir(parents=True)
    (mycomponents_subdir / 'esp32' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py('-DEXTRA_COMPONENT_DIRS={}'.format(str(mycomponents_subdir / 'mycomponent')), 'reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json'))
    assert str((mycomponents_subdir / 'esp32').as_posix()) not in data.get('build_component_paths')
    assert str((mycomponents_subdir / 'mycomponent').as_posix()) in data.get('build_component_paths')
    shutil.rmtree(mycomponents_subdir / 'esp32')

    # now the same thing, but add a components directory
    (test_app_copy / 'esp32').mkdir()
    (test_app_copy / 'esp32' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py(f'-DEXTRA_COMPONENT_DIRS={str(mycomponents_subdir)}', 'reconfigure')
    data = json.load(open(test_app_copy / 'build' / 'project_description.json'))
    assert str((test_app_copy / 'esp32').as_posix()) not in data.get('build_component_paths')
    assert str((mycomponents_subdir / 'mycomponent').as_posix()) in data.get('build_component_paths')


def test_component_properties_are_set(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Component properties are set')
    append_to_file(
        test_app_copy / 'CMakeLists.txt',
        '\n'.join(['', 'idf_component_get_property(srcs main SRCS)', 'message(STATUS SRCS:${srcs})']),
    )
    ret = idf_py('reconfigure')
    assert 'SRCS:{}'.format((test_app_copy / 'main' / 'build_test_app.c').as_posix()) in ret.stdout, (
        'Component properties should be set'
    )


def test_get_property_for_unknown_component(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Getting property of unknown component fails gracefully')
    append_to_file(test_app_copy / 'CMakeLists.txt', '\n'.join(['', 'idf_component_get_property(VAR UNKNOWN PROP)']))
    ret = idf_py('reconfigure', check=False)
    assert "Failed to resolve component 'UNKNOWN'" in ret.stderr, (
        'idf_component_get_property for unknown component should fail gracefully'
    )


def test_set_property_for_unknown_component(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Setting property of unknown component fails gracefully')
    append_to_file(test_app_copy / 'CMakeLists.txt', '\n'.join(['', 'idf_component_set_property(UNKNOWN PROP VAL)']))
    ret = idf_py('reconfigure', check=False)
    assert "Failed to resolve component 'UNKNOWN'" in ret.stderr, (
        'idf_component_set_property for unknown component should fail gracefully'
    )


def test_component_overridden_dir(idf_py: IdfPyFunc, test_app_copy: Path, default_idf_env: EnvDict) -> None:
    logging.info('Getting component overridden dir')
    (test_app_copy / 'components' / 'hal').mkdir(parents=True)
    (test_app_copy / 'components' / 'hal' / 'CMakeLists.txt').write_text(
        '\n'.join(
            [
                'idf_component_get_property(overridden_dir ${COMPONENT_NAME} COMPONENT_OVERRIDEN_DIR)',
                'message(STATUS overridden_dir:${overridden_dir})',
                'idf_component_register()',
            ]
        )
    )
    ret = idf_py('reconfigure')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    # no registration, overrides registration as well
    assert 'overridden_dir:{}'.format((idf_path / 'components' / 'hal').as_posix()) in ret.stdout, (
        'Failed to get overridden dir'
    )
    append_to_file(
        (test_app_copy / 'components' / 'hal' / 'CMakeLists.txt'),
        '\n'.join(
            [
                '',
                'idf_component_register(KCONFIG ${overridden_dir}/Kconfig)',
                'idf_component_get_property(kconfig ${COMPONENT_NAME} KCONFIG)',
                'message(STATUS kconfig:${overridden_dir}/Kconfig)',
            ]
        ),
    )
    ret = idf_py('reconfigure', check=False)
    assert 'kconfig:{}'.format((idf_path / 'components' / 'hal').as_posix()) in ret.stdout, (
        'Failed to verify original `main` directory'
    )


def test_project_components_overrides_extra_components(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Project components override components defined in EXTRA_COMPONENT_DIRS')
    (test_app_copy / 'extra_dir' / 'my_component').mkdir(parents=True)
    (test_app_copy / 'extra_dir' / 'my_component' / 'CMakeLists.txt').write_text('idf_component_register()')

    # Add PRIV_REQUIRES to main component for cmakev2 compatibility
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'PRIV_REQUIRES my_component',
    )

    replace_in_file(
        test_app_copy / 'CMakeLists.txt',
        '# placeholder_before_include_project_cmake',
        'set(EXTRA_COMPONENT_DIRS extra_dir)',
    )
    idf_py('reconfigure')
    with open(test_app_copy / 'build' / 'project_description.json') as f:
        data = json.load(f)
    assert str((test_app_copy / 'extra_dir' / 'my_component').as_posix()) in data.get('build_component_paths')

    (test_app_copy / 'components' / 'my_component').mkdir(parents=True)
    (test_app_copy / 'components' / 'my_component' / 'CMakeLists.txt').write_text('idf_component_register()')
    idf_py('reconfigure')
    with open(test_app_copy / 'build' / 'project_description.json') as f:
        data = json.load(f)
    assert str((test_app_copy / 'components' / 'my_component').as_posix()) in data.get('build_component_paths')
    assert str((test_app_copy / 'extra_dir' / 'my_component').as_posix()) not in data.get('build_component_paths')


@pytest.mark.buildv2_skip(
    'cmakev2 overrides managed components with extra components but both components are included in the build'
)
def test_extra_components_overrides_managed_components(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('components defined in EXTRA_COMPONENT_DIRS override managed components')
    (test_app_copy / 'main' / 'idf_component.yml').write_text("""
    dependencies:
        example/cmp: "*"
        """)
    idf_py('reconfigure')
    with open(test_app_copy / 'build' / 'project_description.json') as f:
        data = json.load(f)
    assert str((test_app_copy / 'managed_components' / 'example__cmp').as_posix()) in data.get('build_component_paths')

    (test_app_copy / 'extra_dir' / 'cmp').mkdir(parents=True)
    (test_app_copy / 'extra_dir' / 'cmp' / 'CMakeLists.txt').write_text('idf_component_register()')
    replace_in_file(
        test_app_copy / 'CMakeLists.txt',
        '# placeholder_before_include_project_cmake',
        'set(EXTRA_COMPONENT_DIRS extra_dir)',
    )
    idf_py('reconfigure')
    with open(test_app_copy / 'build' / 'project_description.json') as f:
        data = json.load(f)
    assert str((test_app_copy / 'extra_dir' / 'cmp').as_posix()) in data.get('build_component_paths')
    assert str((test_app_copy / 'managed_components' / 'example__cmp').as_posix()) not in data.get(
        'build_component_paths'
    )


@pytest.mark.with_idf_components(['cmp'])
def test_managed_components_overrides_idf_components(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Managed components override components defined in IDF_PATH/components')
    # created idf component 'cmp' in marker
    idf_path = Path(os.environ['IDF_PATH'])

    # Add PRIV_REQUIRES to main component for cmakev2 compatibility
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'PRIV_REQUIRES cmp',
    )

    idf_py('reconfigure')
    with open(test_app_copy / 'build' / 'project_description.json') as f:
        data = json.load(f)
    assert str((idf_path / 'components' / 'cmp').as_posix()) in data.get('build_component_paths')

    (test_app_copy / 'main' / 'idf_component.yml').write_text("""
dependencies:
    example/cmp: "*"
    """)
    idf_py('reconfigure')
    with open(test_app_copy / 'build' / 'project_description.json') as f:
        data = json.load(f)
    assert str((test_app_copy / 'managed_components' / 'example__cmp').as_posix()) in data.get('build_component_paths')
    assert str((idf_path / 'components' / 'cmp').as_posix()) not in data.get('build_component_paths')


@pytest.mark.buildv2_skip('cmakev2 does not override with last added components in the same way as cmakev1')
def test_manifest_local_source_overrides_extra_components(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    (test_app_copy / '..' / 'extra_dir' / 'cmp').mkdir(parents=True)
    (test_app_copy / '..' / 'extra_dir' / 'cmp' / 'CMakeLists.txt').write_text('idf_component_register()')
    replace_in_file(
        test_app_copy / 'CMakeLists.txt',
        '# placeholder_before_include_project_cmake',
        'set(EXTRA_COMPONENT_DIRS ../extra_dir)',
    )
    idf_py('reconfigure')
    with open(test_app_copy / 'build' / 'project_description.json') as f:
        data = json.load(f)
    assert str((test_app_copy / '..' / 'extra_dir' / 'cmp').resolve().as_posix()) in data.get('build_component_paths')

    (test_app_copy / '..' / 'cmp').mkdir(parents=True)
    (test_app_copy / '..' / 'cmp' / 'CMakeLists.txt').write_text('idf_component_register()')
    with open(test_app_copy / 'main' / 'idf_component.yml', 'w') as f:
        f.write("""
dependencies:
    cmp:
        path: '../../cmp'
        """)
    idf_py('reconfigure')
    with open(test_app_copy / 'build' / 'project_description.json') as f:
        data = json.load(f)
    assert str((test_app_copy / '..' / 'cmp').resolve().as_posix()) in data.get('build_component_paths')
    assert str((test_app_copy / '..' / 'extra_dir' / 'cmp').resolve().as_posix()) not in data.get(
        'build_component_paths'
    )


@pytest.mark.buildv2_skip('cmakev2 does not support EXCLUDE_COMPONENTS')
def test_exclude_components_not_passed(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Components in EXCLUDE_COMPONENTS not passed to idf_component_manager')
    idf_py('create-component', '-C', 'components', 'to_be_excluded')
    (test_app_copy / 'components' / 'to_be_excluded' / 'idf_component.yml').write_text('invalid syntax..')
    ret = idf_py('reconfigure', check=False)
    assert ret.returncode == 2, 'Reconfigure should have failed due to invalid syntax in idf_component.yml'
    idf_py('-DEXCLUDE_COMPONENTS=to_be_excluded', 'reconfigure')


def test_version_in_component_cmakelist(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Use IDF version variables in component CMakeLists.txt file')
    replace_in_file(
        (test_app_copy / 'main' / 'CMakeLists.txt'),
        '# placeholder_before_idf_component_register',
        '\n'.join(['if (NOT IDF_VERSION_MAJOR)', ' message(FATAL_ERROR "IDF version not set")', 'endif()']),
    )
    idf_py('reconfigure')


def test_unknown_component_error(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('When unknown component name is specified, correct error is shown')
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        search='# placeholder_inside_idf_component_register',
        replace='REQUIRES unknown',
    )
    ret = idf_py('reconfigure', check=False)
    assert "Failed to resolve component 'unknown'" in ret.stderr


@pytest.mark.buildv2_skip('not yet implemented in cmakev2')
def test_component_with_improper_dependency(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # test for __component_validation_check_include_dirs and __component_validation_check_sources
    # Checks that the following warnings are produced:
    #  - Include directory X belongs to component Y but is being used by component Z
    #  - Source file A belongs to component Y but is being built by component Z
    logging.info(
        'Check for warnings when component includes source files or include directories that belong to other components'
    )
    idf_py('create-component', '-C', 'components', 'my_comp')

    # Create a source file and include directory in my_comp
    (test_app_copy / 'components' / 'my_comp' / 'my_comp.c').write_text('void my_func() {}')
    (test_app_copy / 'components' / 'my_comp' / 'include').mkdir(exist_ok=True)
    (test_app_copy / 'components' / 'my_comp' / 'include' / 'my_comp.h').write_text('#pragma once')

    # Make main component try to use files from my_comp
    replace_in_file(
        (test_app_copy / 'main' / 'CMakeLists.txt'),
        '# placeholder_inside_idf_component_register',
        '"../components/my_comp/my_comp.c"\n                       INCLUDE_DIRS "../components/my_comp/include"',
    )
    ret = idf_py('reconfigure')

    inc_dir = (test_app_copy / 'components' / 'my_comp' / 'include').as_posix()
    src_file = (test_app_copy / 'components' / 'my_comp' / 'my_comp.c').as_posix()

    # Check for new validation warnings
    re_include = re.compile(
        rf"Include directory\s+'{re.escape(inc_dir)}'\s+belongs to component\s+my_comp\s+but is being used by "
        rf'component\s+main'
    )
    re_source = re.compile(
        rf"Source file\s+'{re.escape(src_file)}'\s+belongs to component\s+my_comp\s+but is being built by "
        rf'component\s+main'
    )

    assert re_include.search(ret.stderr) is not None, f'Expected include directory warning not found in: {ret.stderr}'
    assert re_source.search(ret.stderr) is not None, f'Expected source file warning not found in: {ret.stderr}'


@pytest.mark.buildv2_skip('not yet implemented in cmakev2')
def test_component_validation_not_run_in_subprojects(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # test that component validation doesn't run in subprojects like bootloader
    logging.info('Check that component validation warnings are not shown in subprojects')

    # Create a component that would trigger validation warnings
    idf_py('create-component', '-C', 'components', 'test_comp')
    (test_app_copy / 'components' / 'test_comp' / 'test_comp.c').write_text('void test_func() {}')
    (test_app_copy / 'components' / 'test_comp' / 'include').mkdir(exist_ok=True)
    (test_app_copy / 'components' / 'test_comp' / 'include' / 'test_comp.h').write_text('#pragma once')

    # Make main component try to use files from test_comp
    replace_in_file(
        (test_app_copy / 'main' / 'CMakeLists.txt'),
        '# placeholder_inside_idf_component_register',
        '"../components/test_comp/test_comp.c"\n                       INCLUDE_DIRS "../components/test_comp/include"',
    )

    # Build the project - this will trigger bootloader build as well
    ret = idf_py('build')

    # Check that validation warnings appear in the main build output
    inc_dir = (test_app_copy / 'components' / 'test_comp' / 'include').as_posix()
    src_file = (test_app_copy / 'components' / 'test_comp' / 'test_comp.c').as_posix()

    re_include = re.compile(
        rf"Include directory\s+'{re.escape(inc_dir)}'\s+belongs to component\s+test_comp\s+but is being used by "
        rf'component\s+main'
    )
    re_source = re.compile(
        rf"Source file\s+'{re.escape(src_file)}'\s+belongs to component\s+test_comp\s+but is being built by "
        rf'component\s+main'
    )

    # The warnings should appear in the main build, not in bootloader build
    assert re_include.search(ret.stderr) is not None, f'Expected include directory warning not found in: {ret.stderr}'
    assert re_source.search(ret.stderr) is not None, f'Expected source file warning not found in: {ret.stderr}'

    # Verify that the build completed successfully despite the warnings
    assert ret.returncode == 0, 'Build should complete successfully with validation warnings'


@pytest.mark.buildv2_skip('not yet implemented in cmakev2')
def test_component_validation_private_include_dirs(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # test that component validation works for private include directories
    logging.info('Check that component validation warnings are shown for private include directories')

    # Create a component with private include directory
    idf_py('create-component', '-C', 'components', 'private_comp')
    (test_app_copy / 'components' / 'private_comp' / 'private').mkdir(exist_ok=True)
    (test_app_copy / 'components' / 'private_comp' / 'private' / 'private.h').write_text('#pragma once')

    # Make main component try to use private include directory from private_comp
    replace_in_file(
        (test_app_copy / 'main' / 'CMakeLists.txt'),
        '# placeholder_inside_idf_component_register',
        'PRIV_INCLUDE_DIRS "../components/private_comp/private"',
    )

    ret = idf_py('reconfigure')

    # Check for private include directory warning
    priv_inc_dir = (test_app_copy / 'components' / 'private_comp' / 'private').as_posix()
    re_priv_include = re.compile(
        rf"Private include directory\s+'{re.escape(priv_inc_dir)}'\s+belongs to "
        rf'component\s+private_comp\s+but is being used by component\s+main'
    )

    assert re_priv_include.search(ret.stderr) is not None, (
        f'Expected private include directory warning not found in: {ret.stderr}'
    )


@pytest.mark.buildv2_skip('not yet implemented in cmakev2')
def test_component_validation_finds_right_component(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # test that __component_validation_get_component_for_path finds the correct component for a given path
    #
    # components/my_comp/test_apps/components/my_subcomp/src/test.c
    # The component for test.c should be my_subcomp, not my_comp

    idf_py('create-component', '-C', 'components', 'my_comp')

    nested_components_dir = test_app_copy / 'components' / 'my_comp' / 'test_apps' / 'components'
    my_subcomp_dir = nested_components_dir / 'my_subcomp'
    (my_subcomp_dir / 'src').mkdir(parents=True)
    (my_subcomp_dir / 'include').mkdir(parents=True)

    # Files of the nested component
    (my_subcomp_dir / 'src' / 'test.c').write_text('void test_func() {}')
    (my_subcomp_dir / 'include' / 'test.h').write_text('#pragma once')
    (my_subcomp_dir / 'CMakeLists.txt').write_text('idf_component_register(SRCS "src/test.c" INCLUDE_DIRS "include")')

    # Make sure build system discovers the nested component by adding its parent directory to EXTRA_COMPONENT_DIRS
    replace_in_file(
        test_app_copy / 'CMakeLists.txt',
        '# placeholder_before_include_project_cmake',
        f'set(EXTRA_COMPONENT_DIRS {nested_components_dir.as_posix()})',
    )

    # Make main component try to use files from my_subcomp via absolute-like relative paths
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        '"../components/my_comp/test_apps/components/my_subcomp/src/test.c"\n'
        '                       INCLUDE_DIRS "../components/my_comp/test_apps/components/my_subcomp/include"',
    )

    ret = idf_py('reconfigure')

    inc_dir = (my_subcomp_dir / 'include').as_posix()
    src_file = (my_subcomp_dir / 'src' / 'test.c').as_posix()

    # The warnings must attribute ownership to my_subcomp (deepest component), not my_comp
    re_include = re.compile(
        rf"Include directory\s+'{re.escape(inc_dir)}'\s+belongs to component\s+my_subcomp\s+but is being used by "
        rf'component\s+main'
    )
    re_source = re.compile(
        rf"Source file\s+'{re.escape(src_file)}'\s+belongs to component\s+my_subcomp\s+but is being built by "
        rf'component\s+main'
    )

    assert re_include.search(ret.stderr) is not None, f'Expected include directory warning not found in: {ret.stderr}'
    assert re_source.search(ret.stderr) is not None, f'Expected source file warning not found in: {ret.stderr}'

    # components/my_comp/test_apps/components/my_subcomp/include/test.h
    # The component for test.h should be my_subcomp, not my_comp
    # Modify main to also list the header as a source to exercise file-level ownership
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '"../components/my_comp/test_apps/components/my_subcomp/src/test.c"\n'
        '                       INCLUDE_DIRS "../components/my_comp/test_apps/components/my_subcomp/include"',
        '"../components/my_comp/test_apps/components/my_subcomp/src/test.c" '
        '"../components/my_comp/test_apps/components/my_subcomp/include/test.h"\n'
        '                       INCLUDE_DIRS "../components/my_comp/test_apps/components/my_subcomp/include"',
    )

    ret = idf_py('reconfigure')

    header_path = (my_subcomp_dir / 'include' / 'test.h').as_posix()
    re_header = re.compile(
        rf"Source file\s+'{re.escape(header_path)}'\s+belongs to component\s+my_subcomp\s+but is being built by "
        rf'component\s+main'
    )
    assert re_header.search(ret.stderr) is not None, (
        f'Expected header file ownership warning not found in: {ret.stderr}'
    )


def test_component_validation_with_common_platform_example(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # Test the following structure which should not produce a warning::
    #
    # my_project/
    # ├── common/                         # Common product code for all platforms (not a component)
    # │   ├── include/
    # │   │   ├── product_config.h
    # │   │   └── business_logic.h
    # │   └── src/
    # │       └── business_logic.c
    # └── env/
    #     ├── esp-idf/
    #     │   ├── main/                   # main component
    #     │   │   ├── idf_main.c          # includes product_config.h and business_logic.h
    #     │   │   └── CMakeLists.txt      # adds ../../../common/include to include dirs
    #     │   └── CMakeLists.txt
    #     └── other_rtos/
    #
    #
    # Implementation: create a sibling 'common' directory outside the IDF project and
    # make the main component include headers and a source file from it. This should
    # NOT produce component ownership warnings because the paths don't belong to any component.

    # Create common directory with headers and source outside the project root
    common_dir = (test_app_copy / '..' / 'common').resolve()
    (common_dir / 'include').mkdir(parents=True, exist_ok=True)
    (common_dir / 'src').mkdir(parents=True, exist_ok=True)
    (common_dir / 'include' / 'product_config.h').write_text('#pragma once\n')
    (common_dir / 'include' / 'business_logic.h').write_text('#pragma once\n')
    (common_dir / 'src' / 'business_logic.c').write_text('void bl(void) {}\n')

    # From main component dir to common dir is ../../common
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        '"../../common/src/business_logic.c"\n                       INCLUDE_DIRS "../../common/include"',
    )

    # Optionally create a main source that includes the headers (not required for validation)
    (test_app_copy / 'main' / 'idf_main.c').write_text(
        '#include "product_config.h"\n#include "business_logic.h"\nvoid app_main(void) {}\n'
    )

    ret = idf_py('reconfigure')

    inc_dir_abs = (common_dir / 'include').as_posix()
    src_file_abs = (common_dir / 'src' / 'business_logic.c').as_posix()

    re_include = re.compile(rf"Include directory\s+'{re.escape(inc_dir_abs)}'\s+belongs to component")
    re_source = re.compile(rf"Source file\s+'{re.escape(src_file_abs)}'\s+belongs to component")

    assert re_include.search(ret.stderr) is None, (
        f'Unexpected include directory ownership warning for common path: {ret.stderr}'
    )
    assert re_source.search(ret.stderr) is None, (
        f'Unexpected source file ownership warning for common path: {ret.stderr}'
    )

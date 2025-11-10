# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import os
import re
import shutil
import sys
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import append_to_file
from test_build_system_helpers import file_contains
from test_build_system_helpers import run_cmake
from test_build_system_helpers import run_cmake_and_build


# This test checks multiple targets in one test function. It would be better to have each target
# tested in a isolated test case, but that would mean doing idf_copy each time, and copying takes most of the time
@pytest.mark.usefixtures('idf_copy')
def test_build_custom_cmake_project(test_app_copy: Path) -> None:
    # Test is compatible with any target. Random targets in the list are selected for performance reasons
    idf_path = Path(os.environ['IDF_PATH'])
    for target in ['esp32', 'esp32c2', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32s2', 'esp32s3']:
        logging.info(f'Test build ESP-IDF as a library to a custom CMake projects for {target}')
        run_cmake_and_build(
            str(idf_path / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib'),
            '-G',
            'Ninja',
            '-DCMAKE_TOOLCHAIN_FILE={}'.format(idf_path / 'tools' / 'cmake' / f'toolchain-{target}.cmake'),
            f'-DTARGET={target}',
        )
        assert file_contains((test_app_copy / 'build' / 'compile_commands.json'), '"command"')
        shutil.rmtree(test_app_copy / 'build')
        os.remove(idf_path / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib' / 'sdkconfig')


@pytest.mark.skipif(
    sys.platform == 'win32',
    reason='On Win project is not buildable without system compiler on the host machine. (Win CI runners)',
)
@pytest.mark.usefixtures('idf_copy')
@pytest.mark.usefixtures('test_app_copy')
def test_build_custom_cmake_project_host() -> None:
    logging.info('Test build ESP-IDF as a library to a custom CMake projects for host')
    idf_path = Path(os.environ['IDF_PATH'])
    run_cmake_and_build(str(idf_path / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib'), '-G', 'Ninja')


def test_build_cmake_library_with_toolchain_flags(test_app_copy: Path) -> None:
    logging.info('Building a project with CMake library imported with modified toolchain flags')
    idf_path = Path(os.environ['IDF_PATH'])

    import_lib_path = idf_path / 'examples' / 'build_system' / 'cmake' / 'import_lib'
    run_cmake_and_build(
        str(import_lib_path),
        '-G',
        'Ninja',
        f'-DSDKCONFIG_DEFAULTS={import_lib_path / "sdkconfig.defaults"}',
    )


def check_flag_in_compile_commands(build_dir: Path, flag_to_find: str) -> None:
    with open(build_dir / 'build' / 'compile_commands.json', encoding='utf-8') as f:
        compile_commands = json.load(f)
        # check if compile_commands is an array
        if not isinstance(compile_commands, list):
            assert False, f'compile_commands is not a list: {compile_commands}'
        assert len(compile_commands) != 0, 'compile_commands is empty'
        for entry in compile_commands:
            command = entry['command']
            assert isinstance(command, str), f'command is not a string: {command}'
            flag_is_found = flag_to_find in command
            if flag_is_found:
                continue  # Flag found in command, no need to check response files
            # check if command contains response file paths starts with @
            response_file_paths = re.findall(r'@([^\s]+)', command)
            for response_file_path in response_file_paths:
                # check if the flag file contains flag_to_find
                try:
                    # Strip surrounding quotes and normalize the path
                    response_file_path = response_file_path.strip('"\'\\')
                    response_file_path = Path(response_file_path).resolve()
                    with open(response_file_path, encoding='utf-8') as f:
                        flags = f.read()
                        if flag_to_find in flags:
                            flag_is_found = True
                            break
                except FileNotFoundError:
                    assert False, f'{response_file_path} does not exist'
            if not flag_is_found:
                assert False, f'{flag_to_find} not found in {command}'


def test_build_cmake_library_psram_workaround(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info(
        'Building a project with CMake library imported and PSRAM workaround, all files compile with workaround'
    )
    (test_app_copy / 'sdkconfig.defaults').write_text(
        '\n'.join(['CONFIG_SPIRAM=y', 'CONFIG_SPIRAM_CACHE_WORKAROUND=y'])
    )
    idf_py('reconfigure')
    check_flag_in_compile_commands(test_app_copy, '-mfix-esp32-psram-cache-issue')


def test_build_cmake_library_psram_strategies(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    for strategy in ['DUPLDST', 'NOPS', 'MEMW']:
        logging.info(f'Test for external libraries in custom CMake projects with PSRAM strategy {strategy}')
        (test_app_copy / 'sdkconfig.defaults').write_text(
            '\n'.join(
                [
                    'CONFIG_SPIRAM=y',
                    f'CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_{strategy}=y',
                    'CONFIG_SPIRAM_CACHE_WORKAROUND=y',
                ]
            )
        )
        idf_py('reconfigure')
        check_flag_in_compile_commands(test_app_copy, f'-mfix-esp32-psram-cache-strategy={strategy.lower()}')
        (test_app_copy / 'sdkconfig').unlink()


def test_defaults_unspecified_build_args(idf_copy: Path) -> None:
    logging.info('Defaults set properly for unspecified idf_build_process args')
    idf_as_lib_path = idf_copy / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib'
    append_to_file(
        idf_as_lib_path / 'CMakeLists.txt',
        '\n'.join(['idf_build_get_property(project_dir PROJECT_DIR)', 'message("Project directory: ${project_dir}")']),
    )
    ret = run_cmake(
        '..',
        '-G',
        'Ninja',
        '-DCMAKE_TOOLCHAIN_FILE={}'.format(str(idf_copy / 'tools' / 'cmake' / 'toolchain-esp32.cmake')),
        '-DTARGET=esp32',
        workdir=idf_as_lib_path,
    )
    assert 'Project directory: {}'.format(str(idf_as_lib_path.as_posix())) in ret.stderr


def test_build_example_on_host(default_idf_env: EnvDict) -> None:
    logging.info('Test if it can build the example to run on host')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    idf_as_lib_path = Path(idf_path, 'examples', 'build_system', 'cmake', 'idf_as_lib')
    try:
        target = 'esp32'
        run_cmake(
            '..',
            f'-DCMAKE_TOOLCHAIN_FILE={idf_path}/tools/cmake/toolchain-{target}.cmake',
            f'-DTARGET={target}',
            '-GNinja',
            workdir=idf_as_lib_path,
        )

        run_cmake('--build', '.', workdir=idf_as_lib_path)
    finally:
        shutil.rmtree(idf_as_lib_path / 'build', ignore_errors=True)

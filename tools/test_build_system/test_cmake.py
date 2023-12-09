# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import re
import shutil
import sys
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict, IdfPyFunc, append_to_file, file_contains, run_cmake, run_cmake_and_build


@pytest.mark.skipif(sys.platform == 'win32', reason='Failing on Windows runner. TODO')
def test_build_custom_cmake_project(test_app_copy: Path) -> None:
    for target in ['esp32', 'esp32s3', 'esp32c6', 'esp32h2']:
        logging.info(f'Test build ESP-IDF as a library to a custom CMake projects for {target}')
        idf_path = Path(os.environ['IDF_PATH'])
        run_cmake_and_build(str(idf_path / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib'),
                            '-DCMAKE_TOOLCHAIN_FILE={}'.format(idf_path / 'tools' / 'cmake' / f'toolchain-{target}.cmake'), f'-DTARGET={target}')
        assert file_contains((test_app_copy / 'build' / 'compile_commands.json'), '"command"')
        shutil.rmtree(test_app_copy / 'build')


def test_build_cmake_library_psram_workaround(test_app_copy: Path) -> None:
    logging.info('Building a project with CMake library imported and PSRAM workaround, all files compile with workaround')
    idf_path = Path(os.environ['IDF_PATH'])
    (test_app_copy / 'sdkconfig.defaults').write_text('\n'.join(['CONFIG_SPIRAM=y',
                                                                 'CONFIG_SPIRAM_CACHE_WORKAROUND=y']))
    run_cmake('-G', 'Ninja', '-DSDKCONFIG_DEFAULTS={}'.format(test_app_copy / 'sdkconfig.defaults'),
              str(idf_path / 'examples' / 'build_system' / 'cmake' / 'import_lib'))
    with open((test_app_copy / 'build' / 'compile_commands.json'), 'r', encoding='utf-8') as f:
        data = f.read()
        res = re.findall(r'.*\"command\".*', data)
        for r in res:
            assert 'mfix-esp32-psram-cache-issue' in r, 'All commands in compile_commands.json should use PSRAM cache workaround'


def test_build_cmake_library_psram_strategies(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    for strategy in ['DUPLDST', 'NOPS', 'MEMW']:
        logging.info(f'Test for external libraries in custom CMake projects with PSRAM strategy {strategy}')
        (test_app_copy / 'sdkconfig.defaults').write_text('\n'.join(['CONFIG_SPIRAM=y',
                                                                    f'CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_{strategy}=y',
                                                                     'CONFIG_SPIRAM_CACHE_WORKAROUND=y']))
        idf_py('reconfigure')
        with open((test_app_copy / 'build' / 'compile_commands.json'), 'r', encoding='utf-8') as f:
            data = f.read()
            res = re.findall(r'.*\"command\".*', data)
            for r in res:
                assert f'mfix-esp32-psram-cache-strategy={strategy.lower()}' in r, ('All commands in compile_commands.json '
                                                                                    'should use PSRAM cache workaround strategy')
        (test_app_copy / 'sdkconfig').unlink()


@pytest.mark.skipif(sys.platform == 'win32', reason='Failing on Windows runner. TODO')
@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.usefixtures('idf_copy')
def test_defaults_for_unspecified_idf_build_process_args(default_idf_env: EnvDict) -> None:
    logging.info('Defaults set properly for unspecified idf_build_process args')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    idf_as_lib_path = idf_path / 'examples' / 'build_system' / 'cmake' / 'idf_as_lib'
    append_to_file(idf_as_lib_path / 'CMakeLists.txt', '\n'.join(['idf_build_get_property(project_dir PROJECT_DIR)',
                                                                  'message("Project directory: ${project_dir}")']))
    ret = run_cmake('..',
                    '-DCMAKE_TOOLCHAIN_FILE={}'.format(str(idf_path / 'tools' / 'cmake' / 'toolchain-esp32.cmake')),
                    '-DTARGET=esp32',
                    workdir=idf_as_lib_path)
    assert 'Project directory: {}'.format(str(idf_as_lib_path)) in ret.stderr


def test_build_example_on_host(default_idf_env: EnvDict) -> None:
    logging.info('Test if it can build the example to run on host')
    idf_path = Path(default_idf_env.get('IDF_PATH'))
    idf_as_lib_path = Path(idf_path, 'examples', 'build_system', 'cmake', 'idf_as_lib')
    try:
        target = 'esp32'
        run_cmake('..',
                  f'-DCMAKE_TOOLCHAIN_FILE={idf_path}/tools/cmake/toolchain-{target}.cmake',
                  f'-DTARGET={target}',
                  '-GNinja',
                  workdir=idf_as_lib_path)

        run_cmake('--build',
                  '.',
                  workdir=idf_as_lib_path)
    finally:
        shutil.rmtree(idf_as_lib_path / 'build', ignore_errors=True)

# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import shutil
from pathlib import Path
from typing import List
from typing import Optional

import pytest
from test_build_system_helpers import EnvDict
from test_build_system_helpers import file_contains
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import run_cmake

ESP32C3_TARGET = 'esp32c3'
ESP32C2_TARGET = 'esp32c2'
ESP32S3_TARGET = 'esp32s3'
ESP32S2_TARGET = 'esp32s2'
ESP32_TARGET = 'esp32'


def clean_app(test_app_copy: Path) -> None:
    shutil.rmtree(test_app_copy / 'build')
    (test_app_copy / 'sdkconfig').unlink()


@pytest.mark.usefixtures('test_app_copy')
def test_target_from_environment_cmake(default_idf_env: EnvDict) -> None:
    logging.info('Can override IDF_TARGET from environment')
    env = default_idf_env
    env.update({'IDF_TARGET': ESP32S2_TARGET})
    run_cmake('-G', 'Ninja', '..', env=env)
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))


def test_target_from_environment_idf_py(idf_py: IdfPyFunc, default_idf_env: EnvDict, test_app_copy: Path) -> None:
    def reconfigure_and_check_return_values(errmsg: str, opts: Optional[List[str]] = None) -> None:
        opts = opts or []
        ret = idf_py(*opts, 'reconfigure', check=False)
        assert ret.returncode == 2
        assert errmsg in ret.stderr

    idf_py('set-target', ESP32S2_TARGET)
    default_idf_env.update({'IDF_TARGET': ESP32_TARGET})

    cfg_path = (test_app_copy / 'sdkconfig').as_posix()

    logging.info("idf.py fails if IDF_TARGET settings don't match the environment")
    reconfigure_and_check_return_values("Project sdkconfig '{}' was generated for target '{}', but environment "
                                        "variable IDF_TARGET is set to '{}'.".format(cfg_path, ESP32S2_TARGET,
                                                                                     ESP32_TARGET))

    logging.info("idf.py fails if IDF_TARGET settings in CMakeCache.txt don't match the environment")
    (test_app_copy / 'sdkconfig').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32_TARGET))
    reconfigure_and_check_return_values("Target settings are not consistent: '{}' in the environment, "
                                        "'{}' in CMakeCache.txt.".format(ESP32_TARGET, ESP32S2_TARGET))

    logging.info("idf.py fails if IDF_TARGET settings in CMakeCache.txt don't match the sdkconfig")
    default_idf_env.pop('IDF_TARGET')
    reconfigure_and_check_return_values("Project sdkconfig '{}' was generated for target '{}', but CMakeCache.txt "
                                        "contains '{}'.".format(cfg_path, ESP32_TARGET, ESP32S2_TARGET))

    logging.info('idf.py fails if IDF_TARGET is set differently in environment and with -D option')
    (test_app_copy / 'sdkconfig').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    default_idf_env.update({'IDF_TARGET': ESP32S2_TARGET})
    reconfigure_and_check_return_values("Target '{}' specified on command line is not consistent with target '{}' "
                                        'in the environment.'.format(ESP32_TARGET, ESP32S2_TARGET),
                                        ['-D', 'IDF_TARGET={}'.format(ESP32_TARGET)])

    logging.info('idf.py fails if IDF_TARGET is set differently in CMakeCache.txt and with -D option')
    default_idf_env.pop('IDF_TARGET')
    reconfigure_and_check_return_values("Target '{}' specified on command line is not consistent with "
                                        "target '{}' in CMakeCache.txt.".format(ESP32_TARGET, ESP32S2_TARGET),
                                        ['-D', 'IDF_TARGET={}'.format(ESP32_TARGET)])


def test_target_consistency_cmake(default_idf_env: EnvDict, test_app_copy: Path) -> None:
    def reconfigure_and_check_return_values(errmsg: str, opts: Optional[List[str]] = None) -> None:
        opts = opts or []
        ret = run_cmake(*opts, '-G', 'Ninja', '..', env=default_idf_env, check=False)
        assert ret.returncode == 1
        assert errmsg in ret.stderr

    run_cmake('-G', 'Ninja', '..')

    cfg_path = (test_app_copy / 'sdkconfig').as_posix()

    logging.info("cmake fails if IDF_TARGET settings don't match the environment")
    default_idf_env.update({'IDF_TARGET': ESP32S2_TARGET})
    reconfigure_and_check_return_values(f"IDF_TARGET '{ESP32_TARGET}' in CMake cache does not "
                                        f"match currently selected IDF_TARGET '{ESP32S2_TARGET}'")

    logging.info("cmake fails if IDF_TARGET settings don't match the sdkconfig")
    default_idf_env.pop('IDF_TARGET')
    (test_app_copy / 'sdkconfig').write_text(f'CONFIG_IDF_TARGET="{ESP32S2_TARGET}"')
    reconfigure_and_check_return_values(f"Target '{ESP32S2_TARGET}' in sdkconfig '{cfg_path}' does not "
                                        f"match currently selected IDF_TARGET '{ESP32_TARGET}'.")

    logging.info("cmake fails if IDF_TOOLCHAIN settings don't match the environment")
    (test_app_copy / 'sdkconfig').write_text(f'CONFIG_IDF_TARGET="{ESP32_TARGET}"')
    default_idf_env.update({'IDF_TOOLCHAIN': 'clang'})
    reconfigure_and_check_return_values("IDF_TOOLCHAIN 'gcc' in CMake cache does not match "
                                        "currently selected IDF_TOOLCHAIN 'clang'")

    logging.info("cmake fails if IDF_TARGET settings don't match CMAKE_TOOLCHAIN_FILE")
    default_idf_env.pop('IDF_TOOLCHAIN')
    reconfigure_and_check_return_values("CMAKE_TOOLCHAIN_FILE 'toolchain-esp32' does not "
                                        f"match currently selected IDF_TARGET '{ESP32S2_TARGET}'",
                                        ['-D', f'IDF_TARGET={ESP32S2_TARGET}',
                                         '-D', 'SDKCONFIG=custom_sdkconfig'])


def test_target_precedence(idf_py: IdfPyFunc, default_idf_env: EnvDict, test_app_copy: Path) -> None:
    logging.info('IDF_TARGET takes precedence over the value of CONFIG_IDF_TARGET in sdkconfig.defaults')
    (test_app_copy / 'sdkconfig.defaults').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    default_idf_env.update({'IDF_TARGET': ESP32_TARGET})
    idf_py('reconfigure')
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32_TARGET))
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET_{}=y'.format(ESP32_TARGET.upper()))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32_TARGET))


def test_target_using_D_parameter(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Can set target using idf.py -D')
    idf_py('-DIDF_TARGET={}'.format(ESP32S2_TARGET), 'reconfigure')
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))

    logging.info('Can set target using -D as subcommand parameter for idf.py')
    clean_app(test_app_copy)
    idf_py('reconfigure', '-DIDF_TARGET={}'.format(ESP32S2_TARGET))
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))


@pytest.mark.usefixtures('test_app_copy')
def test_target_using_settarget_parameter_alternative_name(idf_py: IdfPyFunc) -> None:
    logging.info('idf.py understands alternative target names')
    idf_py('set-target', ESP32S2_TARGET.upper())
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))


@pytest.mark.usefixtures('test_app_copy')
def test_target_using_settarget_parameter(idf_py: IdfPyFunc, default_idf_env: EnvDict) -> None:
    logging.info('Can set target using idf.py set-target')
    idf_py('set-target', ESP32S2_TARGET)
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))

    logging.info('Can guess target from sdkconfig, if CMakeCache does not exist')
    idf_py('fullclean')
    idf_py('reconfigure')
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))

    logging.info('Can set target if IDF_TARGET env is set and old sdkconfig exists')
    default_idf_env.update({'IDF_TARGET': ESP32_TARGET})
    idf_py('set-target', ESP32_TARGET)
    default_idf_env.pop('IDF_TARGET')
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32_TARGET))


def test_target_using_sdkconfig(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Can set the default target using sdkconfig.defaults')
    (test_app_copy / 'sdkconfig.defaults').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    idf_py('reconfigure')
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET_{}=y'.format(ESP32S2_TARGET.upper()))


def test_target_guessing(idf_py: IdfPyFunc, test_app_copy: Path, default_idf_env: EnvDict) -> None:
    """
    Tests are performed from the lowest to the highest priority, while
    configs, except from the sdkconfig, and parameters of previous tests are
    preserved.
    """

    logging.info('Can guess target from sdkconfig.defaults')
    (test_app_copy / 'sdkconfig.defaults').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32_TARGET))
    idf_py('reconfigure')
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32_TARGET))

    logging.info('Can guess target from SDKCONFIG_DEFAULTS environment variable')
    (test_app_copy / 'sdkconfig1').write_text('NOTHING HERE')
    (test_app_copy / 'sdkconfig2').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    clean_app(test_app_copy)
    default_idf_env.update({'SDKCONFIG_DEFAULTS': 'sdkconfig1;sdkconfig2'})
    idf_py('reconfigure')
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))

    logging.info('Can guess target from SDKCONFIG_DEFAULTS using -D')
    (test_app_copy / 'sdkconfig3').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    (test_app_copy / 'sdkconfig4').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32S3_TARGET))
    clean_app(test_app_copy)
    idf_py('-D', 'SDKCONFIG_DEFAULTS=sdkconfig4;sdkconfig3', 'reconfigure')
    assert file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S3_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S3_TARGET))

    logging.info('Can guess target from custom sdkconfig')
    (test_app_copy / 'sdkconfig5').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32C3_TARGET))
    clean_app(test_app_copy)
    idf_py('-D', 'SDKCONFIG=sdkconfig5', '-D', 'SDKCONFIG_DEFAULTS=sdkconfig4;sdkconfig3', 'reconfigure')
    assert file_contains('sdkconfig5', 'CONFIG_IDF_TARGET="{}"'.format(ESP32C3_TARGET))
    assert file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32C3_TARGET))

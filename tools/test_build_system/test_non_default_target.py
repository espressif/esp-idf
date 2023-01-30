# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import shutil
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict, IdfPyFunc, check_file_contains, run_cmake

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
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    check_file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))


def test_target_from_environment_idf_py(idf_py: IdfPyFunc, default_idf_env: EnvDict, test_app_copy: Path) -> None:
    def reconfigure_and_check_return_values(errmsg: str) -> None:
        ret = idf_py('reconfigure', check=False)
        assert ret.returncode == 2
        assert errmsg in ret.stderr

    idf_py('set-target', ESP32S2_TARGET)
    default_idf_env.update({'IDF_TARGET': ESP32_TARGET})

    logging.info("idf.py fails if IDF_TARGET settings don't match the environment")
    reconfigure_and_check_return_values("Project sdkconfig was generated for target '{}', but environment "
                                        "variable IDF_TARGET is set to '{}'.".format(ESP32S2_TARGET, ESP32_TARGET))

    logging.info("idf.py fails if IDF_TARGET settings in CMakeCache.txt don't match the environment")
    (test_app_copy / 'sdkconfig').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32_TARGET))
    reconfigure_and_check_return_values("Target settings are not consistent: '{}' in the environment, "
                                        "'{}' in CMakeCache.txt.".format(ESP32_TARGET, ESP32S2_TARGET))

    logging.info("idf.py fails if IDF_TARGET settings in CMakeCache.txt don't match the sdkconfig")
    default_idf_env.pop('IDF_TARGET')
    reconfigure_and_check_return_values("Project sdkconfig was generated for target '{}', but CMakeCache.txt "
                                        "contains '{}'.".format(ESP32_TARGET, ESP32S2_TARGET))


def test_target_precedence(idf_py: IdfPyFunc, default_idf_env: EnvDict, test_app_copy: Path) -> None:
    logging.info('IDF_TARGET takes precedence over the value of CONFIG_IDF_TARGET in sdkconfig.defaults')
    (test_app_copy / 'sdkconfig.defaults').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    default_idf_env.update({'IDF_TARGET': ESP32_TARGET})
    idf_py('reconfigure')
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32_TARGET))
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET_{}=y'.format(ESP32_TARGET.upper()))
    check_file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32_TARGET))


def test_target_using_D_parameter(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Can set target using idf.py -D')
    idf_py('-DIDF_TARGET={}'.format(ESP32S2_TARGET), 'reconfigure')
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    check_file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))

    logging.info('Can set target using -D as subcommand parameter for idf.py')
    clean_app(test_app_copy)
    idf_py('reconfigure', '-DIDF_TARGET={}'.format(ESP32S2_TARGET))
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    check_file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))


@pytest.mark.usefixtures('test_app_copy')
def test_target_using_settarget_parameter_alternative_name(idf_py: IdfPyFunc) -> None:
    logging.info('idf.py understands alternative target names')
    idf_py('set-target', ESP32S2_TARGET.upper())
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    check_file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))


@pytest.mark.usefixtures('test_app_copy')
def test_target_using_settarget_parameter(idf_py: IdfPyFunc) -> None:
    logging.info('Can set target using idf.py set-target')
    idf_py('set-target', ESP32S2_TARGET)
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    check_file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))

    logging.info('Can guess target from sdkconfig, if CMakeCache does not exist')
    idf_py('fullclean')
    idf_py('reconfigure')
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    check_file_contains('build/CMakeCache.txt', 'IDF_TARGET:STRING={}'.format(ESP32S2_TARGET))


def test_target_using_sdkconfig(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Can set the default target using sdkconfig.defaults')
    (test_app_copy / 'sdkconfig.defaults').write_text('CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    idf_py('reconfigure')
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET="{}"'.format(ESP32S2_TARGET))
    check_file_contains('sdkconfig', 'CONFIG_IDF_TARGET_{}=y'.format(ESP32S2_TARGET.upper()))

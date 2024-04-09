# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import shutil
import subprocess
import sys
from pathlib import Path

import pytest
from test_build_system_helpers import run_idf_py

# In this test file the test are grouped into 3 bundles
# It would be better to have every test separate,
# but that would mean doing idf_copy each time, and copying takes most of the time


def clean_app_dir(app_path: Path) -> None:
    (app_path / 'sdkconfig').unlink()
    shutil.rmtree(app_path / 'build', ignore_errors=True)


@pytest.mark.idf_copy('esp idf with spaces')
def test_spaces_bundle1(idf_copy: Path) -> None:
    logging.info('Running test spaces bundle 1')
    # test_build
    run_idf_py('build', workdir=(idf_copy / 'examples' / 'get-started' / 'hello_world'))
    # test spiffsgen
    run_idf_py('build', workdir=(idf_copy / 'examples' / 'storage' / 'spiffsgen'))
    # test build ulp_fsm
    run_idf_py('build', workdir=(idf_copy / 'examples' / 'system' / 'ulp' / 'ulp_fsm' / 'ulp'))
    # test build ulp_riscv
    run_idf_py('-DIDF_TARGET=esp32s2', 'build', workdir=(idf_copy / 'examples' / 'system' / 'ulp' / 'ulp_riscv' / 'gpio'))


@pytest.mark.idf_copy('esp idf with spaces')
def test_spaces_bundle2(idf_copy: Path) -> None:
    logging.info('Running test spaces bundle 2')
    # test flash_encryption
    run_idf_py('build', workdir=(idf_copy / 'examples' / 'security' / 'flash_encryption'))
    # test_x509_cert_bundle
    run_idf_py('build', workdir=(idf_copy / 'examples' / 'protocols' / 'https_x509_bundle'))
    # test dfu
    hello_world_app_path = (idf_copy / 'examples' / 'get-started' / 'hello_world')
    run_idf_py('-DIDF_TARGET=esp32s2', 'dfu', workdir=hello_world_app_path)
    clean_app_dir(hello_world_app_path)
    # test uf2
    run_idf_py('uf2', workdir=hello_world_app_path)


@pytest.mark.idf_copy('esp idf with spaces')
def test_spaces_bundle3(idf_copy: Path) -> None:
    logging.info('Running test spaces bundle 3')
    secure_boot_app_path = (idf_copy / 'tools' / 'test_apps' / 'security' / 'secure_boot')
    # test secure_boot_v1
    run_idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.ci.00', 'build',
               workdir=secure_boot_app_path)
    clean_app_dir(secure_boot_app_path)
    # test secure_boot_v2
    run_idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.ci.01', 'build',
               workdir=secure_boot_app_path)
    clean_app_dir(secure_boot_app_path)
    # test app_signing
    run_idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.ci.02', 'build',
               workdir=secure_boot_app_path)
    clean_app_dir(secure_boot_app_path)
    # test secure_boot_release_mode
    run_idf_py('-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.ci.04', '-DIDF_TARGET=esp32s2', 'build',
               workdir=secure_boot_app_path)


@pytest.mark.skipif(sys.platform == 'win32', reason='Unix test')
@pytest.mark.idf_copy('esp idf with spaces')
def test_install_export_unix(idf_copy: Path) -> None:
    logging.info('install and export setup scripts')
    env = dict(**os.environ)
    install_cmd = './install.sh esp32'
    export_cmd = '. ./export.sh'

    logging.debug('running {} in {}'.format(install_cmd, idf_copy))
    subprocess.check_call(install_cmd, env=env, shell=True, cwd=idf_copy)
    logging.debug('running {} in {}'.format(export_cmd, idf_copy))
    # The default shell used by subprocess.Popen on POSIX platforms is '/bin/sh',
    # which in esp-env Docker image is 'dash'. The export script doesn't support
    # IDF_PATH detection when used in dash, so we have to override the shell here.
    subprocess.check_call(export_cmd, env=env, shell=True, cwd=idf_copy, executable='/bin/bash')


@pytest.mark.skipif(sys.platform != 'win32', reason='Windows test')
@pytest.mark.idf_copy('esp idf with spaces')
def test_install_export_win(idf_copy: Path) -> None:
    logging.info('install and export setup scripts')
    env = dict(**os.environ)
    install_cmd = 'install.bat esp32'
    export_cmd = 'export.bat'

    logging.debug('running {} in {}'.format(install_cmd, idf_copy))
    subprocess.check_call(install_cmd, env=env, shell=True, cwd=idf_copy)
    logging.debug('running {} in {}'.format(export_cmd, idf_copy))
    subprocess.check_call(export_cmd, env=env, shell=True, cwd=idf_copy)
